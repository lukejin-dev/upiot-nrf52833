/**
 * UpIoT
 */
#include "sdk_common.h"
#include "board.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include "nrf_log.h"
#include "ble_gap.h"
#include "app_error.h"
#include "crc32.h"
#include "nrf_log_ctrl.h"
#include "nrf_log.h"
#include "nrf_log_default_backends.h"
#include "app_scheduler.h"
#include "app_timer.h"
#include "nrf_pwr_mgmt.h"
#include "gpio.h"
#include "nrf_gpio.h"

#define MAX_LED_NUMBER  8

uint32_t    mLedPins[MAX_LED_NUMBER] = { NC };
uint8_t     mLedCount = 0;

/*!
 * Flag to indicate if the MCU is Initialized
 */
static bool McuInitialized = false;

void BoardCriticalSectionBegin(uint32_t *mask)
{
    *mask = __get_PRIMASK( );
    __disable_irq( );
}

void BoardCriticalSectionEnd(uint32_t *mask)
{
    __set_PRIMASK( *mask );
}

/**
 * Get Device EUI in 64bit.
 * see https://www.thethingsnetwork.org/docs/lorawan/addressing.html
 */
void BoardGetDevEUI(uint8_t *eui)
{
    ble_gap_addr_t device_addr;
    APP_ERROR_CHECK( sd_ble_gap_addr_get( &device_addr ) );

    for ( int i = 0; i < 6; i++ )
    {
        eui[i] = device_addr.addr[5 - i];
    }

    eui[6] = 0x0;    //  reserved for future
    eui[7] = 0x0;    //  reserved for future

    NRF_LOG_INFO("Board Device EUI:");
    NRF_LOG_HEXDUMP_INFO(&eui[0], 8);
}

uint32_t BoardGetDevAddr()
{
    uint8_t eui[8];
    BoardGetDevEUI(eui);
    return crc32_compute(eui, 8, NULL);
}

/*!
 * \brief Gets the board 64 bits unique ID
 *
 * \param [IN] id Pointer to an array that will contain the Unique ID
 */
void BoardGetUniqueId( uint8_t *id )
{
    BoardGetDevEUI(id);
}

void BoardInitMcu( void )
{
    if( McuInitialized == false )
    {
        APP_ERROR_CHECK( NRF_LOG_INIT( NULL ) );
        NRF_LOG_DEFAULT_BACKENDS_INIT();

        APP_SCHED_INIT( APP_TIMER_SCHED_EVENT_DATA_SIZE, 60 );

        APP_ERROR_CHECK( app_timer_init() );

        APP_ERROR_CHECK( nrf_pwr_mgmt_init() );
    }
}

void BoardResetMcu( void )
{
    CRITICAL_SECTION_BEGIN( );

    //Restart system
    NVIC_SystemReset( );
}

void BoardIdleStateHandle( void )
{
    app_sched_execute();

    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}

void BoardInitLeds(uint32_t* pins, uint8_t count)
{
    uint32_t i;
    ASSERT(count < MAX_LED_NUMBER);

    mLedCount = count;
    for (i = 0; i < count; ++i)
    {
        mLedPins[i] = pins[i];
        nrf_gpio_cfg_output(pins[i]);
    }
    BoardLedsOff();
}

void BoardLedsOff(void)
{
    uint32_t i;
    for (i = 0; i < mLedCount; ++i)
    {
        BoardLedOff(i);
    }
}

void BoardLedOff(uint32_t led_idx)
{
    ASSERT(led_idx < mLedCount);
    nrf_gpio_pin_write(mLedPins[led_idx], 1);
}

void BoardLedOn(uint32_t led_idx)
{
    ASSERT(led_idx < mLedCount);
    nrf_gpio_pin_write(mLedPins[led_idx], 0);
}

uint8_t BoardGetBatteryLevel( void )
{
    return 0;
}

void BoardLedToggle(uint32_t led_idx)
{
    ASSERT(led_idx < mLedCount);
    nrf_gpio_pin_toggle(mLedPins[led_idx]);
}