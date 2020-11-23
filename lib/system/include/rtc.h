/*!
 * \file      rtc-board.h
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
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#ifndef __RTC_H__
#define __RTC_H__

#include <stdint.h>
#include <stdbool.h>
#include "timer.h"
#include "nrfx_rtc.h"

// #define RTC_DEBUG

/*!
 * \brief Temperature coefficient of the clock source
 */
#define RTC_TEMP_COEFFICIENT                            ( -0.035 )

/*!
 * \brief Temperature coefficient deviation of the clock source
 */
#define RTC_TEMP_DEV_COEFFICIENT                        ( 0.0035 )

/*!
 * \brief Turnover temperature of the clock source
 */
#define RTC_TEMP_TURNOVER                               ( 25.0 )

/*!
 * \brief Turnover temperature deviation of the clock source
 */
#define RTC_TEMP_DEV_TURNOVER                           ( 5.0 )

/*!
 * \brief Initializes the RTC timer
 *
 * \remark The timer is based on the RTC2
 */
void RtcInit( void );

/*!
 * \brief UnInitializes the RTC timer
 *
 * \remark The timer is based on the RTC2
 */
void RtcDeInit( void );

/*!
 * \brief Start the RTC timer
 *
 * \remark The timer is based on the RTC2
 */
void RtcStart( void );

/*!
 * \brief Stop the RTC timer
 *
 * \remark The timer is based on the RTC2
 */
void RtcStop( void );

uint32_t RtcGetCalendarTime( uint16_t *milliseconds );

/*!
 * \brief Stop the RTC timer
 *
 * \return [out] Current time recorded by RTC
 */
uint32_t RtcGetTimerValue( void );

void RtcBkupWrite( uint32_t data0, uint32_t data1 );

void RtcBkupRead( uint32_t *data0, uint32_t *data1 );

/*!
 * \brief converts time in ms to time in ticks
 *
 * \param[IN] milliseconds Time in milliseconds
 * \retval returns time in timer ticks
 */
uint32_t RtcMs2Tick( TimerTime_t milliseconds );

/*!
 * \brief converts time in ticks to time in ms
 *
 * \param[IN] time in timer ticks
 * \retval returns time in milliseconds
 */
TimerTime_t RtcTick2Ms( uint32_t tick );

/*!
 * \brief Performs a delay of milliseconds by polling RTC
 *
 * \param[IN] milliseconds Delay in ms
 */
void RtcDelayMs( TimerTime_t milliseconds );

/*!
 * \brief Computes the temperature compensation for a period of time on a
 *        specific temperature.
 *
 * \param [IN] period Time period to compensate in milliseconds
 * \param [IN] temperature Current temperature
 *
 * \retval Compensated time period
 */
TimerTime_t RtcTempCompensation( TimerTime_t period, float temperature );

#endif // __RTC_H__
