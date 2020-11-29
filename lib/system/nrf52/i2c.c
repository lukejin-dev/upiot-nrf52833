/*!
 * \file      i2c.c
 *
 * \brief     I2C driver implementation
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
#include <stdbool.h>
#include <string.h>
#include "utilities.h"
#include "nrfx_twim.h"
#include "app_error.h"
#include "i2c.h"
#include "uplog.h"

/*!
 *  The value of the maximal timeout for I2C waiting loops
 */
#define TIMEOUT_MAX                   0x8000
#define TWI_BUFFER_SIZE               64

#if NRFX_CHECK(NRFX_TWIM0_ENABLED)
    static const nrfx_twim_t Twi_ID_0 = NRFX_TWIM_INSTANCE(0);
#else
    static const nrfx_twim_t Twi_ID_0 = { 0 };
#endif
#if NRFX_CHECK(NRFX_TWIM1_ENABLED)
    static const nrfx_twim_t Twi_ID_1 = NRFX_TWIM_INSTANCE(1);
#else
    static const nrfx_twim_t Twi_ID_1 = { 0 };
#endif

static nrfx_twim_config_t Twi_config = NRFX_TWIM_DEFAULT_CONFIG;
static uint8_t Twi_Buffer[TWI_BUFFER_SIZE] = { 0 };

void I2cInit( I2c_t *obj, I2cId_t i2cId, PinNames scl, PinNames sda )
{
    if ( obj == NULL)
    {
        ASSERT( FAIL );
    }

    obj->I2cId = i2cId;
    Twi_config.scl = obj->Scl.pin = scl;
    Twi_config.sda = obj->Sda.pin = sda;

    if( i2cId == I2C_0 )
    {
        obj->Inst = (void*)&Twi_ID_0;
    }
    else if( i2cId == I2C_1 )
    {
        obj->Inst = (void*)&Twi_ID_1;
    }
    else
    {
        ASSERT( FAIL );
    }

    APP_ERROR_CHECK( nrfx_twim_init( obj->Inst, &Twi_config, NULL, NULL ) );

    nrfx_twim_enable( obj->Inst );
}

void I2cDeInit( I2c_t *obj )
{
    nrfx_twim_uninit( obj->Inst );
}

uint32_t I2cWrite( I2c_t *obj, uint8_t deviceAddr, uint8_t *buffer, uint16_t size, bool no_stop )
{
    uint32_t err_code = NRF_SUCCESS;

    if ( obj == NULL)
    {
        ASSERT( FAIL );
    }

    err_code = nrfx_twim_tx( obj->Inst, deviceAddr, buffer, size, no_stop );
#ifdef I2C_DEBUG
    UP_INFO("I2C Write:");
    UP_HEXDUMP_INFO( buffer, size );
#endif
    APP_ERROR_CHECK(err_code);
    return err_code;
}

uint32_t I2cRead( I2c_t *obj, uint8_t deviceAddr, uint8_t *buffer, uint16_t size )
{
    uint32_t err_code = NRF_SUCCESS;

    if ( obj == NULL)
    {
        ASSERT( FAIL );
    }

    err_code = nrfx_twim_rx( obj->Inst, deviceAddr, buffer, size );
#ifdef I2C_DEBUG
    UP_INFO("I2C Read:");
    UP_HEXDUMP_INFO( buffer, size );
#endif
    APP_ERROR_CHECK(err_code);
    return err_code;
}

uint32_t I2cRegWrite( I2c_t *obj, uint8_t deviceAddr, uint8_t reg, uint8_t *buffer, uint16_t size, bool no_stop )
{
    uint32_t err_code = NRF_SUCCESS;

    if ( obj == NULL)
    {
        ASSERT( FAIL );
    }

    memcpy( Twi_Buffer, 0, TWI_BUFFER_SIZE );
    Twi_Buffer[0] = reg;
    memcpy( Twi_Buffer + 1, buffer, size );

    err_code = nrfx_twim_tx( obj->Inst, deviceAddr, Twi_Buffer, size + 1, no_stop );
#ifdef I2C_DEBUG
    UP_INFO("I2C Write:");
    UP_HEXDUMP_INFO( Twi_Buffer, size + 1 );
#endif
    APP_ERROR_CHECK(err_code);
    return err_code;
}

uint32_t I2cRegRead( I2c_t *obj, uint8_t deviceAddr, uint8_t reg, uint8_t *buffer, uint16_t size )
{
    uint32_t err_code = NRF_SUCCESS;

    if ( obj == NULL)
    {
        ASSERT( FAIL );
    }

    memcpy( Twi_Buffer, 0, TWI_BUFFER_SIZE );
    Twi_Buffer[0] = reg;

    err_code = nrfx_twim_tx( obj->Inst, deviceAddr, Twi_Buffer, 1, false );
#ifdef I2C_DEBUG
    UP_INFO("I2C Write:");
    UP_HEXDUMP_INFO( Twi_Buffer, 1 );
#endif
    APP_ERROR_CHECK(err_code);

    err_code = nrfx_twim_rx( obj->Inst, deviceAddr, buffer, size );
#ifdef I2C_DEBUG
    UP_INFO("I2C Read:");
    UP_HEXDUMP_INFO( buffer, size );
#endif
    APP_ERROR_CHECK(err_code);
    return err_code;
}

bool I2cIsBusy( I2c_t *obj )
{
    return nrfx_twim_is_busy( obj->Inst );
}
