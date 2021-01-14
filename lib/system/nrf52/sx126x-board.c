/**
 * UpIoT
 *
 * Author: Lu Ken
 */

#include "sdk_common.h"
#include "gpio.h"
#include "spi.h"
#include "sx126x.h"
#include "sx126x-board.h"
#include "delay.h"
#include "board.h"
#include "board_config.h"

/*!
 * Antenna switch GPIO pins objects
 */
Gpio_t AntPow;
Gpio_t DeviceSel;

/*!
 * Debug GPIO pins objects
 */
#if defined( USE_RADIO_DEBUG )
Gpio_t DbgPinTx;
Gpio_t DbgPinRx;
#endif

#define SPI_INOUTBUF_LEN    64
static uint8_t SpiInOutBuf[SPI_INOUTBUF_LEN] = { 0 };
static PinNames mPinReset = NC;
static PinNames mPinAntSwitch = NC;
static uint32_t mTcxoWakeupTime = 10; // default value

extern SX126x_t SX126x;

void SX126xIoInit(PinNames pinBusy, PinNames pinDio1,
    PinNames pinReset, PinNames pinAntSwitch)
{
    mPinReset = pinReset;
    mPinAntSwitch = pinAntSwitch;

    // GpioInit( &SX126x.Spi.Nss, RADIO_NSS, PIN_OUTPUT, PIN_S0S1, PIN_PULL_UP, 1 );
    GpioInit( &SX126x.BUSY, pinBusy, PIN_INPUT, PIN_S0S1, PIN_NO_PULL, 0 );
    GpioInit( &SX126x.DIO1, pinDio1, PIN_INPUT, PIN_S0S1, PIN_NO_PULL, 0 );
    // GpioInit( &DeviceSel, RADIO_DEVICE_SEL, PIN_INPUT, PIN_S0S1, PIN_NO_PULL, 0 );
}

void SX126xIoIrqInit( DioIrqHandler dioIrq )
{
    GpioSetInterrupt( &SX126x.DIO1, IRQ_RISING_EDGE, IRQ_ACCURACY_LOW, dioIrq );
}

void SX126xIoDeInit( void )
{
    // GpioDeInit( &SX126x.Spi.Nss );
    GpioDeInit( &SX126x.BUSY );
    GpioDeInit( &SX126x.DIO1 );
}

void SX126xIoDbgInit( void )
{
#if defined( USE_RADIO_DEBUG )
    GpioInit( &DbgPinTx, RADIO_DBG_PIN_TX, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
    GpioInit( &DbgPinRx, RADIO_DBG_PIN_RX, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 0 );
#endif
}

void SX126xIoTcxoInit( void )
{
#ifdef USE_TCXO
    CalibrationParams_t calibParam;

    SX126xSetDio3AsTcxoCtrl( TCXO_CTRL_3_0V, SX126xGetBoardTcxoWakeupTime( ) << 6 ); // convert from ms to SX126x time base
    calibParam.Value = 0x7F;
    SX126xCalibrate( calibParam );
#endif
}

void SX126xSetBoardTcxoWakeupTime( uint32_t wtime )
{
    mTcxoWakeupTime = wtime;
}

uint32_t SX126xGetBoardTcxoWakeupTime( void )
{
    return mTcxoWakeupTime;
}

void SX126xReset( void )
{
    ASSERT(mPinReset != NC);

    DelayMs( 10 );
    GpioInit( &SX126x.Reset, mPinReset, PIN_OUTPUT, PIN_S0S1, PIN_NO_PULL, 0 );
    DelayMs( 20 );
    GpioInit( &SX126x.Reset, mPinReset, PIN_INPUT, PIN_S0S1, PIN_NO_PULL, 0 ); // internal pull-up
    DelayMs( 10 );
}

void SX126xWaitOnBusy( void )
{
    while( GpioRead( &SX126x.BUSY ) == 1 );
}

void SX126xWakeup( void )
{
    CRITICAL_SECTION_BEGIN( );

    memcpy( SpiInOutBuf, 0, SPI_INOUTBUF_LEN);
    // GpioWrite( &SX126x.Spi.Nss, 0 );
    SpiInOutBuf[0] = RADIO_GET_STATUS;
    SpiInOutBuf[1] = 0x00;
    // SpiInOut( &SX126x.Spi, RADIO_GET_STATUS );
    // SpiInOut( &SX126x.Spi, 0x00 );
    SpiInOut( &SX126x.Spi, SpiInOutBuf, 2, NULL, 0 );
    // GpioWrite( &SX126x.Spi.Nss, 1 );

    // Wait for chip to be ready.
    SX126xWaitOnBusy( );

    CRITICAL_SECTION_END( );
}

void SX126xWriteCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
    SX126xCheckDeviceReady( );

    memcpy( SpiInOutBuf, 0, SPI_INOUTBUF_LEN);
    // GpioWrite( &SX126x.Spi.Nss, 0 );
    SpiInOutBuf[0] = ( uint8_t )command;
    // SpiInOut( &SX126x.Spi, ( uint8_t )command );
    for( uint16_t i = 0; i < size; i++ )
    {
        SpiInOutBuf[i+1] = buffer[i];
        // SpiInOut( &SX126x.Spi, buffer[i] );
    }
    SpiInOut( &SX126x.Spi, SpiInOutBuf, size + 1 , NULL, 0 );
    // GpioWrite( &SX126x.Spi.Nss, 1 );

    if( command != RADIO_SET_SLEEP )
    {
        SX126xWaitOnBusy( );
    }
}

uint8_t SX126xReadCommand( RadioCommands_t command, uint8_t *buffer, uint16_t size )
{
    uint8_t status = 0;

    SX126xCheckDeviceReady( );

    memcpy( SpiInOutBuf, 0, SPI_INOUTBUF_LEN);
    // GpioWrite( &SX126x.Spi.Nss, 0 );
    SpiInOutBuf[0] = ( uint8_t )command;
    SpiInOutBuf[1] = 0x00;
    SpiInOut( &SX126x.Spi, SpiInOutBuf, 2, buffer, size);
    // SpiInOut( &SX126x.Spi, ( uint8_t )command );
    // SpiInOut( &SX126x.Spi, 0x00 );
    // for( uint16_t i = 0; i < size; i++ )
    // {
    //     buffer[i] = SpiInOut( &SX126x.Spi, 0 );
    // }
    // GpioWrite( &SX126x.Spi.Nss, 1 );

    SX126xWaitOnBusy( );

    return status;
}

void SX126xWriteRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
    SX126xCheckDeviceReady( );
    memcpy( SpiInOutBuf, 0, SPI_INOUTBUF_LEN);
    // GpioWrite( &SX126x.Spi.Nss, 0 );
    SpiInOutBuf[0] = RADIO_WRITE_REGISTER;
    SpiInOutBuf[1] = ( address & 0xFF00 ) >> 8;
    SpiInOutBuf[2] = address & 0x00FF;
    // SpiInOut( &SX126x.Spi, RADIO_WRITE_REGISTER );
    // SpiInOut( &SX126x.Spi, ( address & 0xFF00 ) >> 8 );
    // SpiInOut( &SX126x.Spi, address & 0x00FF );
    for( uint16_t i = 0; i < size; i++ )
    {
        SpiInOutBuf[i+3] = buffer[i];
        // SpiInOut( &SX126x.Spi, buffer[i] );
    }
    SpiInOut( &SX126x.Spi, SpiInOutBuf, size + 3, NULL, 0 );
    // GpioWrite( &SX126x.Spi.Nss, 1 );

    SX126xWaitOnBusy( );
}

void SX126xWriteRegister( uint16_t address, uint8_t value )
{
    SX126xWriteRegisters( address, &value, 1 );
}

void SX126xReadRegisters( uint16_t address, uint8_t *buffer, uint16_t size )
{
    SX126xCheckDeviceReady( );
    memcpy( SpiInOutBuf, 0, SPI_INOUTBUF_LEN);
    // GpioWrite( &SX126x.Spi.Nss, 0 );
    SpiInOutBuf[0] = RADIO_READ_REGISTER;
    SpiInOutBuf[1] = ( address & 0xFF00 ) >> 8;
    SpiInOutBuf[2] = address & 0x00FF;
    SpiInOutBuf[3] = 0x00;
    SpiInOut( &SX126x.Spi, SpiInOutBuf, 4, buffer, size);
    // SpiInOut( &SX126x.Spi, RADIO_READ_REGISTER );
    // SpiInOut( &SX126x.Spi, ( address & 0xFF00 ) >> 8 );
    // SpiInOut( &SX126x.Spi, address & 0x00FF );
    // SpiInOut( &SX126x.Spi, 0 );
    // for( uint16_t i = 0; i < size; i++ )
    // {
    //     buffer[i] = SpiInOut( &SX126x.Spi, 0 );
    // }
    // GpioWrite( &SX126x.Spi.Nss, 1 );

    SX126xWaitOnBusy( );
}

uint8_t SX126xReadRegister( uint16_t address )
{
    uint8_t data;
    SX126xReadRegisters( address, &data, 1 );
    return data;
}

void SX126xWriteBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
    SX126xCheckDeviceReady( );
    memcpy( SpiInOutBuf, 0, SPI_INOUTBUF_LEN);
    // GpioWrite( &SX126x.Spi.Nss, 0 );
    SpiInOutBuf[0] = RADIO_WRITE_BUFFER;
    SpiInOutBuf[1] = offset;
    // SpiInOut( &SX126x.Spi, RADIO_WRITE_BUFFER );
    // SpiInOut( &SX126x.Spi, offset );
    for( uint16_t i = 0; i < size; i++ )
    {
        SpiInOutBuf[i+2] = buffer[i];
        // SpiInOut( &SX126x.Spi, buffer[i] );
    }
    SpiInOut( &SX126x.Spi, SpiInOutBuf, size + 2, NULL, 0 );
    // GpioWrite( &SX126x.Spi.Nss, 1 );

    SX126xWaitOnBusy( );
}

void SX126xReadBuffer( uint8_t offset, uint8_t *buffer, uint8_t size )
{
    SX126xCheckDeviceReady( );
    memcpy( SpiInOutBuf, 0, SPI_INOUTBUF_LEN);
    // GpioWrite( &SX126x.Spi.Nss, 0 );
    SpiInOutBuf[0] = RADIO_READ_BUFFER;
    SpiInOutBuf[1] = offset;
    SpiInOutBuf[2] = 0x00;
    SpiInOut( &SX126x.Spi, SpiInOutBuf, 3, buffer, size);
    // SpiInOut( &SX126x.Spi, RADIO_READ_BUFFER );
    // SpiInOut( &SX126x.Spi, offset );
    // SpiInOut( &SX126x.Spi, 0 );
    // for( uint16_t i = 0; i < size; i++ )
    // {
    //     buffer[i] = SpiInOut( &SX126x.Spi, 0 );
    // }
    // GpioWrite( &SX126x.Spi.Nss, 1 );

    SX126xWaitOnBusy( );
}

void SX126xSetRfTxPower( int8_t power )
{
    SX126xSetTxParams( power, RADIO_RAMP_40_US );
}

uint8_t SX126xGetDeviceId( void )
{
    // if( GpioRead( &DeviceSel ) == 1 )
    // {
    //     return SX1261;
    // }
    // else
    {
        return SX1262;
    }
}

void SX126xAntSwOn( void )
{
    ASSERT(mPinAntSwitch != NC);
    GpioInit( &AntPow, mPinAntSwitch, PIN_OUTPUT, PIN_S0S1, PIN_PULL_UP, 1 );
}

void SX126xAntSwOff( void )
{
    ASSERT(mPinAntSwitch != NC);
    GpioInit( &AntPow, mPinAntSwitch, PIN_INPUT, PIN_S0S1, PIN_NO_PULL, 0 );
}

bool SX126xCheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}

#if defined( USE_RADIO_DEBUG )
void SX126xDbgPinTxWrite( uint8_t state )
{
    GpioWrite( &DbgPinTx, state );
}

void SX126xDbgPinRxWrite( uint8_t state )
{
    GpioWrite( &DbgPinRx, state );
}
#endif

void SX126xIoRfSwitchInit( void )
{
    SX126xSetDio2AsRfSwitchCtrl( true );
}
