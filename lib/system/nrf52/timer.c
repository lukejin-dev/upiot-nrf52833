/*!
 * \file      timer.c
 *
 * \brief     Timer objects and scheduling management implementation
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
#include "utilities.h"
#include "board.h"
#include "rtc.h"
#include "timer.h"

static void TimerEvtCallback( void * p_context )
{
    TimerEvent_t* p_obj = (TimerEvent_t *)p_context;
    p_obj->Callback(NULL);
    p_obj->IsStarted = false;
}

void TimerInit( TimerEvent_t *obj, void ( *callback )( void *context ) )
{
    obj->TimerId = &(obj->Timerobj);
    obj->Timestamp = 0;
    obj->ReloadValue = 0;
    obj->IsStarted = false;
    obj->Callback = callback;
    obj->Context = NULL;
    obj->Next = NULL;
    APP_ERROR_CHECK( app_timer_create( &obj->TimerId, APP_TIMER_MODE_SINGLE_SHOT, TimerEvtCallback) );
}

// void TimerSetContext( TimerEvent_t *obj, void* context )
// {
//     obj->Context = context;
// }

void TimerStart( TimerEvent_t *obj )
{


    CRITICAL_SECTION_BEGIN( );

    APP_ERROR_CHECK_BOOL( obj != NULL );
    APP_ERROR_CHECK_BOOL( obj->ReloadValue != 0 );
    APP_ERROR_CHECK_BOOL( obj->Callback != NULL );

    APP_ERROR_CHECK( app_timer_start( obj->TimerId, obj->ReloadValue, obj ) );

    obj->IsStarted = true;
    CRITICAL_SECTION_END( );
}

bool TimerIsStarted( TimerEvent_t *obj )
{
    return obj->IsStarted;
}

void TimerStop( TimerEvent_t *obj )
{
    CRITICAL_SECTION_BEGIN( );

    if( obj->IsStarted )
    {
        APP_ERROR_CHECK( app_timer_stop( obj->TimerId ) );

        obj->IsStarted = false;
    }

    CRITICAL_SECTION_END( );
}

void TimerReset( TimerEvent_t *obj )
{
    TimerStop( obj );
    TimerStart( obj );
}

void TimerSetValue( TimerEvent_t *obj, uint32_t ms_value )
{
    TimerStop( obj );

    obj->Timestamp = app_timer_cnt_get();
    obj->ReloadValue = APP_TIMER_TICKS( ms_value );
}

TimerTime_t TimerGetCurrentTime( void )
{
    uint32_t now = RtcGetTimerValue( );
    return  RtcTick2Ms( now );
}

TimerTime_t TimerGetElapsedTime( TimerTime_t past )
{
    if ( past == 0 )
    {
        return 0;
    }
    uint32_t nowInTicks = RtcGetTimerValue( );
    uint32_t pastInTicks = RtcMs2Tick( past );

    // Intentional wrap around. Works Ok if tick duration below 1ms
    return RtcTick2Ms( nowInTicks - pastInTicks );
}

TimerTime_t TimerTempCompensation( TimerTime_t period, float temperature )
{
    return RtcTempCompensation( period, temperature );
}
