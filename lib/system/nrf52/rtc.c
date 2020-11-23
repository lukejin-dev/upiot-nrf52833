/*!
 * \file      rtc-board.c
 *
 * \brief     Target board RTC timer and low power modes management
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech - STMicroelectronics
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 *
 * \author    MCD Application Team (C)( STMicroelectronics International )
 */
#include <math.h>
#include "rtc.h"
#include "utilities.h"
#include "systime.h"

static SysTime_t Globle_Systime = { 0 };
static const nrfx_rtc_t Rtc_ID = NRFX_RTC_INSTANCE(2);
static nrfx_rtc_config_t Rtc_Config = NRFX_RTC_DEFAULT_CONFIG;
static uint32_t Rtc_Overflow_Count = 0;

static void RTCEvtCallback( nrfx_rtc_int_type_t int_type )
{
	switch(int_type)
	{
		case NRFX_RTC_INT_OVERFLOW:
		{
            Rtc_Overflow_Count++;
		break;
		}
		default:break;
	}
}

// 从日历中获取当前时钟的Tick数,默认每Tick一次时间 1/1024s
static uint64_t RtcGetCalendarValue( void )
{
    uint64_t system_tick_value = 0;
    uint32_t now_tick_value = nrfx_rtc_counter_get( &Rtc_ID );
    system_tick_value = (uint64_t)Rtc_Overflow_Count * 0x1000000 + now_tick_value;
#ifdef RTC_DEBUG
    NRF_LOG_INFO("now_tick_value = %d, system_tick_value = %ld\r\n", now_tick_value, system_tick_value);
#endif
    return system_tick_value;
}

uint32_t RtcGetCalendarTime( uint16_t *milliseconds )
{
    uint32_t ticks;

    uint64_t calendarValue = RtcGetCalendarValue();

    uint32_t seconds = ( uint32_t )calendarValue >> 10;

    ticks =  ( uint32_t )(calendarValue - seconds * 1024);

    *milliseconds = RtcTick2Ms( ticks );
#ifdef RTC_DEBUG
    NRF_LOG_INFO("seconds = %d, subseconds = %d\r\n", seconds, *milliseconds);
#endif
    return seconds;
}

uint32_t RtcGetTimerValue( void )
{
    uint32_t calendarValue = ( uint32_t )RtcGetCalendarValue();
    return( calendarValue );
}

void RtcInit( void )
{
    // 1000/1024 ms counter period
    Rtc_Config.prescaler = 31;

    APP_ERROR_CHECK( nrfx_rtc_init( &Rtc_ID, &Rtc_Config, RTCEvtCallback ) );

    nrfx_rtc_counter_clear( &Rtc_ID );

    nrfx_rtc_overflow_enable( &Rtc_ID, true );

    RtcStart();
}

void RtcDeInit( void )
{
    nrfx_rtc_uninit( &Rtc_ID );
}

void RtcStart( void )
{
    nrfx_rtc_enable( &Rtc_ID );
}

void RtcStop( void )
{
    nrfx_rtc_disable( &Rtc_ID );
}

void RtcBkupWrite( uint32_t data0, uint32_t data1 )
{
    Globle_Systime.Seconds = data0;
    Globle_Systime.SubSeconds = ( int16_t )data1;
#ifdef RTC_DEBUG
    NRF_LOG_INFO("RtcBkupWrite: Globle_Systime.Seconds = %d , Globle_Systime.SubSeconds = %d\r\n", Globle_Systime.Seconds, Globle_Systime.SubSeconds);
#endif
}

void RtcBkupRead( uint32_t *data0, uint32_t *data1 )
{
    *data0 = Globle_Systime.Seconds;
    *data1 = Globle_Systime.SubSeconds;
#ifdef RTC_DEBUG
    NRF_LOG_INFO("RtcBkupRead: Globle_Systime.Seconds = %d , Globle_Systime.SubSeconds = %d\r\n", *data0, *data1);
#endif
}

/*!
 * \brief converts time in ms to time in ticks
 *
 * \param[IN] milliseconds Time in milliseconds
 * \retval returns time in timer ticks
 */
uint32_t RtcMs2Tick( TimerTime_t milliseconds )
{
    if ( milliseconds == 0 )
    {
        return 0;
    }
    return ( uint32_t ) ( milliseconds * 1024 / 1000 );
}

/*!
 * \brief converts time in ticks to time in ms
 *
 * \param[IN] time in timer ticks
 * \retval returns time in milliseconds
 */
TimerTime_t RtcTick2Ms( uint32_t tick )
{
    if ( tick == 0 )
    {
        return 0;
    }
    return ( uint32_t ) ( tick * 1000 / 1024 );
}

/*!
 * \brief a delay of delay ms by polling RTC
 *
 * \param[IN] delay in ms
 */
void RtcDelayMs( TimerTime_t milliseconds )
{
    uint32_t delayTicks = 0;
    uint32_t refTicks = RtcGetTimerValue( );

    delayTicks = RtcMs2Tick( milliseconds );

    // Wait delay ms
    while( ( ( RtcGetTimerValue( ) - refTicks ) ) < delayTicks )
    {
        __NOP( );
    }
}

TimerTime_t RtcTempCompensation( TimerTime_t period, float temperature )
{
    float k = RTC_TEMP_COEFFICIENT;
    float kDev = RTC_TEMP_DEV_COEFFICIENT;
    float t = RTC_TEMP_TURNOVER;
    float tDev = RTC_TEMP_DEV_TURNOVER;
    float interim = 0.0;
    float ppm = 0.0;

    if( k < 0.0 )
    {
        ppm = ( k - kDev );
    }
    else
    {
        ppm = ( k + kDev );
    }
    interim = ( temperature - ( t - tDev ) );
    ppm *=  interim * interim;

    // Calculate the drift in time
    interim = ( ( float ) period * ppm ) / 1e6;
    // Calculate the resulting time period
    interim += period;
    interim = floor( interim );

    if( interim < 0.0 )
    {
        interim = ( float )period;
    }

    // Calculate the resulting period
    return ( TimerTime_t ) interim;
}
