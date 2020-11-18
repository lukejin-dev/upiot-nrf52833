/**
 * UpIoT
 */

#include "board.h"
#include "spi.h"
#include "nrfx_spim.h"

#define SPI_MAX_BUF_SIZE 64
// static  const nrfx_spim_t   Spi_Id[3] = { NRFX_SPIM_INSTANCE(0), NRFX_SPIM_INSTANCE(1), NRFX_SPIM_INSTANCE(2) };
#if NRFX_CHECK(NRFX_SPIM0_ENABLED)
    static const nrfx_spim_t Spi_ID_0 = NRFX_SPIM_INSTANCE(0);
#else
    static const nrfx_spim_t Spi_ID_0 = { 0 };
#endif
#if NRFX_CHECK(NRFX_SPIM1_ENABLED)
    static const nrfx_spim_t Spi_ID_1 = NRFX_SPIM_INSTANCE(1);
#else
    static const nrfx_spim_t Spi_ID_1 = { 0 };
#endif
#if NRFX_CHECK(NRFX_SPIM2_ENABLED)
    static const nrfx_spim_t Spi_ID_2 = NRFX_SPIM_INSTANCE(2);
#else
    static const nrfx_spim_t Spi_ID_2 = { 0 };
#endif
// static const nrfx_spim_t Spi_ID = NRFX_SPIM_INSTANCE(0);
static uint8_t g_spi_rx_buf[SPI_MAX_BUF_SIZE] = { 0 };
// static uint8_t tx_buf[SPI_MAX_BUF_SIZE] = { 0 };
static  nrfx_spim_config_t   Spi_Config = NRFX_SPIM_DEFAULT_CONFIG;
static  nrfx_spim_xfer_desc_t   Spi_Transfer_Descriptor = { 0 };

// static void SpiEvtCallback( nrfx_spim_evt_t const *p_event, void *p_context )
// {
//     if ( p_event->type == NRFX_SPIM_EVENT_DONE )
//     {
//         NRF_LOG_INFO("Spi Done!\r\n");
//         /* code */
//     }
//     else
//     {
//         NRF_LOG_INFO("Spi Error!!!\r\n");
//         /* code */
//     }
// }

void SpiInit( Spi_t *obj, SpiIds_t spiId, PinNames mosi, PinNames miso, PinNames sclk, PinNames nss )
{
    if ( obj == NULL)
    {
        ASSERT( false );
    }

    CRITICAL_SECTION_BEGIN( );

    obj->SpiId = spiId;

    if( spiId == SPI_0 )
    {
        obj->Spi_Inst = Spi_ID_0;
    }
    else if( spiId == SPI_1 )
    {
        obj->Spi_Inst = Spi_ID_1;
    }
    else if( spiId == SPI_2 )
    {
        obj->Spi_Inst = Spi_ID_2;
    }
    else
    {
        ASSERT( FAIL );
    }

    Spi_Config.sck_pin = obj->Sclk.pin = sclk;
    Spi_Config.mosi_pin = obj->Mosi.pin = mosi;
    Spi_Config.miso_pin = obj->Miso.pin = miso;
    Spi_Config.ss_pin = obj->Nss.pin = nss;

    // SpiFormat( SPI_MODE_0, SPI_FREQ_8M, SPI_BIT_MSB_FIRST );
    Spi_Config.mode = NRF_SPIM_MODE_0;
    Spi_Config.frequency = NRF_SPIM_FREQ_8M;
    Spi_Config.bit_order = NRF_SPIM_BIT_ORDER_MSB_FIRST;

    *(volatile uint32_t *)0x40003FFC = 0;
	*(volatile uint32_t *)0x40003FFC;
	*(volatile uint32_t *)0x40003FFC = 1;

    APP_ERROR_CHECK( nrfx_spim_init( &obj->Spi_Inst, &Spi_Config, NULL, NULL ) );

    CRITICAL_SECTION_END( );
}

void SpiDeInit( Spi_t *obj )
{
    if ( obj == NULL)
    {
        ASSERT( FAIL );
    }

    *(volatile uint32_t *)0x40003FFC = 1;
	*(volatile uint32_t *)0x40003FFC;
	*(volatile uint32_t *)0x40003FFC = 0;

    nrfx_spim_uninit( &obj->Spi_Inst );

    // GpioDeInit( &obj->Mosi );
    // GpioDeInit( &obj->Miso );
    // GpioDeInit( &obj->Sclk );
    // GpioDeInit( &obj->Nss );
}

void SpiFormat( SpiModes_t spiMode, SpiFrequencys_t spiFrequency, SpiBitOrders_t spiBitOrder)
{
    if( spiMode == SPI_MODE_0 )
    {
        Spi_Config.mode = NRF_SPIM_MODE_0;
    }
    else if( spiMode == SPI_MODE_1 )
    {
        Spi_Config.mode = NRF_SPIM_MODE_1;
    }
    else if( spiMode == SPI_MODE_2 )
    {
        Spi_Config.mode = NRF_SPIM_MODE_2;
    }
    else if( spiMode == SPI_MODE_3 )
    {
        Spi_Config.mode = NRF_SPIM_MODE_3;
    }
    else
    {
        ASSERT( FAIL );
    }

    if( spiFrequency == SPI_FREQ_125K )
    {
        Spi_Config.frequency = NRF_SPIM_FREQ_125K;
    }
    else if( spiFrequency == SPI_FREQ_250K )
    {
        Spi_Config.frequency = NRF_SPIM_FREQ_250K;
    }
    else if( spiFrequency == SPI_FREQ_500K )
    {
        Spi_Config.frequency = NRF_SPIM_FREQ_500K;
    }
    else if( spiFrequency == SPI_FREQ_1M )
    {
        Spi_Config.frequency = NRF_SPIM_FREQ_1M;
    }
    else if( spiFrequency == SPI_FREQ_2M )
    {
        Spi_Config.frequency = NRF_SPIM_FREQ_2M;
    }
    else if( spiFrequency == SPI_FREQ_4M )
    {
        Spi_Config.frequency = NRF_SPIM_FREQ_4M;
    }
    else if( spiFrequency == SPI_FREQ_8M )
    {
        Spi_Config.frequency = NRF_SPIM_FREQ_8M;
    }
    else
    {
        ASSERT( FAIL );
    }

    if( spiBitOrder == SPI_BIT_MSB_FIRST )
    {
        Spi_Config.bit_order = NRF_SPIM_BIT_ORDER_MSB_FIRST;
    }
    else if( spiBitOrder == SPI_BIT_LSB_FIRST )
    {
        Spi_Config.bit_order = NRF_SPIM_BIT_ORDER_LSB_FIRST;
    }
    else
    {
        ASSERT( FAIL );
    }
}

void SpiInOut( Spi_t *obj, uint8_t *txData , uint8_t tx_len, uint8_t *rxData, uint8_t rx_len)
{
    if( ( obj == NULL ) )
    {
        ASSERT( FAIL );
    }

    CRITICAL_SECTION_BEGIN( );

	// *(volatile uint32_t *)0x40003FFC = 0;
	// *(volatile uint32_t *)0x40003FFC;
	// *(volatile uint32_t *)0x40003FFC = 1;
    // APP_ERROR_CHECK( nrfx_spim_init( &obj->Spi_Inst, &Spi_Config, NULL, NULL ) );

    Spi_Transfer_Descriptor.p_tx_buffer = txData;
    Spi_Transfer_Descriptor.tx_length = tx_len;
    if( rx_len == 0 || rxData == NULL )
    {
        Spi_Transfer_Descriptor.p_rx_buffer = NULL;
        Spi_Transfer_Descriptor.rx_length = 0;
    }
    else
    {
        Spi_Transfer_Descriptor.p_rx_buffer = g_spi_rx_buf;
        Spi_Transfer_Descriptor.rx_length = rx_len + tx_len;
    }

    APP_ERROR_CHECK( nrfx_spim_xfer( &obj->Spi_Inst, &Spi_Transfer_Descriptor, 0 ) );
#ifdef  SPI_DEBUG
    NRF_LOG_INFO("Spi Send Data:");
    NRF_LOG_HEXDUMP_INFO( txData, tx_len );
    NRF_LOG_INFO("\r\n");
#endif
    if( rxData != NULL )
    {
#ifdef  SPI_DEBUG
        NRF_LOG_INFO("Spi Recv Data:");
        NRF_LOG_HEXDUMP_INFO( g_spi_rx_buf, rx_len + tx_len);
        NRF_LOG_INFO("\r\n");
#endif
        memcpy( rxData, g_spi_rx_buf + tx_len, rx_len );
    }

    // nrfx_spim_uninit( &obj->Spi_Inst );
    // *(volatile uint32_t *)0x40003FFC = 1;
	// *(volatile uint32_t *)0x40003FFC;
	// *(volatile uint32_t *)0x40003FFC = 0;

    CRITICAL_SECTION_END( );
}

void SpiWirte( Spi_t *obj, uint8_t *writeData, uint8_t len )
{
    if( ( obj == NULL ) )
    {
        ASSERT( FAIL );
    }

    CRITICAL_SECTION_BEGIN( );

    // *(volatile uint32_t *)0x40003FFC = 0;
	// *(volatile uint32_t *)0x40003FFC;
	// *(volatile uint32_t *)0x40003FFC = 1;
    // APP_ERROR_CHECK( nrfx_spim_init( &obj->Spi_Inst, &Spi_Config, NULL, NULL ) );

    Spi_Transfer_Descriptor.p_tx_buffer = writeData;
    Spi_Transfer_Descriptor.tx_length = len;
    Spi_Transfer_Descriptor.p_rx_buffer = NULL;
    Spi_Transfer_Descriptor.rx_length = 0;

    APP_ERROR_CHECK( nrfx_spim_xfer( &obj->Spi_Inst, &Spi_Transfer_Descriptor, 0 ) );
#ifdef  SPI_DEBUG
    NRF_LOG_INFO("Spi Send Data:");
    NRF_LOG_HEXDUMP_INFO( writeData, len );
    NRF_LOG_INFO("\r\n");
#endif
    // nrfx_spim_uninit( &obj->Spi_Inst );
    // *(volatile uint32_t *)0x40003FFC = 1;
	// *(volatile uint32_t *)0x40003FFC;
	// *(volatile uint32_t *)0x40003FFC = 0;

    CRITICAL_SECTION_END( );
}

void SpiRead( Spi_t *obj, uint8_t *readData, uint8_t len )
{
    if( ( obj == NULL ) )
    {
        ASSERT( FAIL );
    }

    CRITICAL_SECTION_BEGIN( );

    // *(volatile uint32_t *)0x40003FFC = 0;
	// *(volatile uint32_t *)0x40003FFC;
	// *(volatile uint32_t *)0x40003FFC = 1;
    // APP_ERROR_CHECK( nrfx_spim_init( &obj->Spi_Inst, &Spi_Config, NULL, NULL ) );

    Spi_Transfer_Descriptor.p_tx_buffer = NULL;
    Spi_Transfer_Descriptor.tx_length = 0;
    Spi_Transfer_Descriptor.p_rx_buffer = readData;
    Spi_Transfer_Descriptor.rx_length = len;

    APP_ERROR_CHECK( nrfx_spim_xfer( &obj->Spi_Inst, &Spi_Transfer_Descriptor, 0 ) );
#ifdef  SPI_DEBUG
    NRF_LOG_INFO("Spi Recv Data:");
    NRF_LOG_HEXDUMP_INFO( readData, len );
    NRF_LOG_INFO("\r\n");
#endif
    // nrfx_spim_uninit( &obj->Spi_Inst );
    // *(volatile uint32_t *)0x40003FFC = 1;
	// *(volatile uint32_t *)0x40003FFC;
	// *(volatile uint32_t *)0x40003FFC = 0;

    CRITICAL_SECTION_END( );
}