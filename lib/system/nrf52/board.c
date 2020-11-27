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