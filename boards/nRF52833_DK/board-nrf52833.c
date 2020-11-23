/*!
 * \file      board.c
 *
 * \brief     Target board general functions implementation
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

#include "sdk_common.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_pwr_mgmt.h"
#include "app_scheduler.h"
#include "app_timer.h"
#include "ble_gap.h"

#include "board.h"
#include "log.h"

/*!
 * Unique Devices IDs register set ( STM32L152x )
 */
#define         ID1                                 ( 0x1FF800D0 )
#define         ID2                                 ( 0x1FF800D4 )
#define         ID3                                 ( 0x1FF800E4 )

// /*!
//  * Used to measure and calibrate the system wake-up time from STOP mode
//  */
// static void CalibrateSystemWakeupTime( void );

// /*!
//  * Timer used at first boot to calibrate the SystemWakeupTime
//  */
// static TimerEvent_t CalibrateSystemWakeupTimeTimer;

/*!
 * Flag to indicate if the MCU is Initialized
 */
static bool McuInitialized = false;

// /*!
//  * Flag used to indicate if board is powered from the USB
//  */
// static bool UsbIsConnected = false;

// /*!
//  * Flag to indicate if the SystemWakeupTime is Calibrated
//  */
// static volatile bool SystemWakeupTimeCalibrated = false;

// /*!
//  * Callback indicating the end of the system wake-up time calibration
//  */
// static void OnCalibrateSystemWakeupTimeTimerEvent( void* context )
// {
//     RtcSetMcuWakeUpTime( );
//     SystemWakeupTimeCalibrated = true;
// }

void BoardInitPeriph( void )
{

}

void BoardIdleStateHandle( void )
{
    app_sched_execute();

    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
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

        // UsbIsConnected = true;

        //RtcInit( );
        // if( GetBoardPowerSource( ) == BATTERY_POWER )
        // {
        //     // Disables OFF mode - Enables lowest power mode (STOP)
        //     LpmSetOffMode( LPM_APPLI_ID, LPM_DISABLE );
        // }
    }

#if defined( SX1261MBXBAS ) || defined( SX1262MBXCAS ) || defined( SX1262MBXDAS )
    SpiInit( &SX126x.Spi, SPI_2, RADIO_MOSI, RADIO_MISO, RADIO_SCLK, RADIO_NSS );
    SX126xIoInit( );
#elif defined( SX1272MB2DAS)
    SpiInit( &SX1272.Spi, SPI_2, RADIO_MOSI, RADIO_MISO, RADIO_SCLK, RADIO_NSS );
    SX1272IoInit( );
#elif defined( SX1276MB1LAS ) || defined( SX1276MB1MAS )
    SpiInit( &SX1276.Spi, SPI_2, RADIO_MOSI, RADIO_MISO, RADIO_SCLK, RADIO_NSS );
    SX1276IoInit( );
#endif

    if( McuInitialized == false )
    {
        McuInitialized = true;
#if defined( SX1261MBXBAS ) || defined( SX1262MBXCAS ) || defined( SX1262MBXDAS )
        SX126xIoDbgInit( );
        // WARNING: If necessary the TCXO control is initialized by SX126xInit function.
#elif defined( SX1272MB2DAS)
        SX1272IoDbgInit( );
        SX1272IoTcxoInit( );
#elif defined( SX1276MB1LAS ) || defined( SX1276MB1MAS )
        SX1276IoDbgInit( );
        SX1276IoTcxoInit( );
#endif
        // if( GetBoardPowerSource( ) == BATTERY_POWER )
        // {
        //     CalibrateSystemWakeupTime( );
        // }
    }
}

void BoardResetMcu( void )
{
    CRITICAL_SECTION_BEGIN( );

    //Restart system
    NVIC_SystemReset( );
}

void BoardDeInitMcu( void )
{
#if defined( SX1261MBXBAS ) || defined( SX1262MBXCAS ) || defined( SX1262MBXDAS )
    SpiDeInit( &SX126x.Spi );
    SX126xIoDeInit( );
#elif defined( SX1272MB2DAS)
    SpiDeInit( &SX1272.Spi );
    SX1272IoDeInit( );
#elif defined( SX1276MB1LAS ) || defined( SX1276MB1MAS )
    SpiDeInit( &SX1276.Spi );
    SX1276IoDeInit( );
#endif
}
/**
uint32_t BoardGetDevAddr( uint8_t *appskey, uint8_t *nwkskey )
{
    uint8_t little_devaddr[4];
    uint8_t big_devaddr[4];
    uint8_t dev_eui[8];
    uint32_t devaddr = 0;
    BoardGetDevEui( dev_eui );
    devaddr = crc32_compute( dev_eui, 8, NULL );
    LittleEndian_Uint32ToHex( devaddr, little_devaddr );
    BigEndian_Uint32ToHex( devaddr, big_devaddr );
    if( appskey != NULL && nwkskey != NULL )
    {
        for( int i = 0; i < 4; i++ )
        {
            appskey[i] = little_devaddr[i];
            appskey[i + 4] = big_devaddr[i];
            appskey[i + 8] = little_devaddr[i];
            appskey[i + 12] = big_devaddr[i];
            nwkskey[i] = big_devaddr[i];
            nwkskey[i + 4] = little_devaddr[i];
            nwkskey[i + 8] = big_devaddr[i];
            nwkskey[i + 12] = little_devaddr[i];
        }
    }
    return devaddr;
}

void BoardSetDeviceType(uint8_t devtype)
{
	CurrentDeviceType = devtype;
}
**/

void BoardGetBleMac( uint8_t *mac )
{
    ble_gap_addr_t device_addr;
    APP_ERROR_CHECK( sd_ble_gap_addr_get( &device_addr ) );

    for ( int i = 0; i < 6; i++ )
    {
        mac[i] = device_addr.addr[5 - i];
    }
    UP_INFO("BLE MAC:");
    UP_HEXDUMP_INFO( mac, 6 );
    UP_INFO("\r\n");
}

void BoardGetDevEui( uint8_t *id )
{
    uint8_t ble_mac[6];
    BoardGetBleMac( ble_mac );
    id[7] = ble_mac[5];
    id[6] = ble_mac[4];
    id[5] = ble_mac[3];
    id[4] = ble_mac[2];
    id[3] = ble_mac[1];
    id[2] = ble_mac[0];
    //id[1] = CurrentDeviceType;    // 表示设备类型,  0x01 空气温湿度传感器, 0x03 土壤温湿度传感器， 0x05 一氧化碳传感器, 0x07 倾角传感器, 0x09 超声波水位传感器, 0x10 距离传感器, 0xA2 土壤PH传感器
    id[0] = 0x16;   // 固定字节，表示属于互由的设备
}



uint16_t BoardBatteryMeasureVolage( void )
{
    return 0;
}

uint32_t BoardGetBatteryVoltage( void )
{
    return 0;
}

uint8_t BoardGetBatteryLevel( void )
{
    return 0;
}

void BoardGetUniqueId( uint8_t *id )
{
    id[7] = ( ( *( uint32_t* )ID1 )+ ( *( uint32_t* )ID3 ) ) >> 24;
    id[6] = ( ( *( uint32_t* )ID1 )+ ( *( uint32_t* )ID3 ) ) >> 16;
    id[5] = ( ( *( uint32_t* )ID1 )+ ( *( uint32_t* )ID3 ) ) >> 8;
    id[4] = ( ( *( uint32_t* )ID1 )+ ( *( uint32_t* )ID3 ) );
    id[3] = ( ( *( uint32_t* )ID2 ) ) >> 24;
    id[2] = ( ( *( uint32_t* )ID2 ) ) >> 16;
    id[1] = ( ( *( uint32_t* )ID2 ) ) >> 8;
    id[0] = ( ( *( uint32_t* )ID2 ) );
}

// void CalibrateSystemWakeupTime( void )
// {
//     if( SystemWakeupTimeCalibrated == false )
//     {
//         TimerInit( &CalibrateSystemWakeupTimeTimer, OnCalibrateSystemWakeupTimeTimerEvent );
//         TimerSetValue( &CalibrateSystemWakeupTimeTimer, 1000 );
//         TimerStart( &CalibrateSystemWakeupTimeTimer );
//         while( SystemWakeupTimeCalibrated == false )
//         {

//         }
//     }
// }


// void SysTick_Handler( void )
// {
//     HAL_IncTick( );
//     HAL_SYSTICK_IRQHandler( );
// }

// uint8_t GetBoardPowerSource( void )
// {
//     if( UsbIsConnected == false )
//     {
//         return BATTERY_POWER;
//     }
//     else
//     {
//         return USB_POWER;
//     }
// }

// -----------------------------------------------------------------------------------------------------
// /**
//   * \brief Enters Low Power Stop Mode
//   *
//   * \note ARM exists the function when waking up
//   */
// void LpmEnterStopMode( void)
// {
//     CRITICAL_SECTION_BEGIN( );

//     BoardDeInitMcu( );

//     // Disable the Power Voltage Detector
//     HAL_PWR_DisablePVD( );

//     // Clear wake up flag
//     SET_BIT( PWR->CR, PWR_CR_CWUF );

//     // Enable Ultra low power mode
//     HAL_PWREx_EnableUltraLowPower( );

//     // Enable the fast wake up from Ultra low power mode
//     HAL_PWREx_EnableFastWakeUp( );

//     CRITICAL_SECTION_END( );

//     // Enter Stop Mode
//     HAL_PWR_EnterSTOPMode( PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI );
// }

// /*!
//  * \brief Exists Low Power Stop Mode
//  */
// void LpmExitStopMode( void )
// {
//     // Disable IRQ while the MCU is not running on HSI
//     CRITICAL_SECTION_BEGIN( );

//     // Initilizes the peripherals
//     BoardInitMcu( );

//     CRITICAL_SECTION_END( );
// }

// /*!
//  * \brief Enters Low Power Sleep Mode
//  *
//  * \note ARM exits the function when waking up
//  */
// void LpmEnterSleepMode( void)
// {
//     HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
// }

// void BoardLowPowerHandler( void )
// {
//     __disable_irq( );
//     /*!
//      * If an interrupt has occurred after __disable_irq( ), it is kept pending
//      * and cortex will not enter low power anyway
//      */

//     LpmEnterLowPower( );

//     __enable_irq( );
// }

// #if !defined ( __CC_ARM )

// /*
//  * Function to be used by stdout for printf etc
//  */
// int _write( int fd, const void *buf, size_t count )
// {
//     while( UartPutBuffer( &Uart2, ( uint8_t* )buf, ( uint16_t )count ) != 0 ){ };
//     return count;
// }

// /*
//  * Function to be used by stdin for scanf etc
//  */
// int _read( int fd, const void *buf, size_t count )
// {
//     size_t bytesRead = 0;
//     while( UartGetBuffer( &Uart2, ( uint8_t* )buf, count, ( uint16_t* )&bytesRead ) != 0 ){ };
//     // Echo back the character
//     while( UartPutBuffer( &Uart2, ( uint8_t* )buf, ( uint16_t )bytesRead ) != 0 ){ };
//     return bytesRead;
// }

// #else

// // Keil compiler
// int fputc( int c, FILE *stream )
// {
//     while( UartPutChar( &Uart2, ( uint8_t )c ) != 0 );
//     return c;
// }

// int fgetc( FILE *stream )
// {
//     uint8_t c = 0;
//     while( UartGetChar( &Uart2, &c ) != 0 );
//     // Echo back the character
//     while( UartPutChar( &Uart2, c ) != 0 );
//     return ( int )c;
// }

// #endif
