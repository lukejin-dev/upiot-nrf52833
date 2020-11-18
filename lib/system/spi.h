/*!
 * \file      spi.h
 *
 * \brief     HUWOMO IoT device SPI interface library file
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                _      _
 *               | |    | |  _      _   _      __      _     ___      _________      ___
 *               | |____| | | |    | | \ \    /  \    / / / /   \ \  /  _   _  \  / /   \ \
 *               |  ____  | | |    | |  \ \  / /\ \  / / / /     \ \ | | | | | | / /     \ \
 *               | |    | | | \____/ |   \ \/ /  \ \/ /  \ \     / / | | | | | | \ \     / /
 *               |_|    |_|  \______/     \__/    \__/    \ \___/ /  |_| |_| |_|  \ \___/ /
 *              (C)2018-2019 Huwomo
 *
 * \endcode
 *
 * \author    Chace ( Huwomo )
 *
 */
#ifndef __SPI_H__
#define __SPI_H__

#include "gpio.h"

// #define SPI_DEBUG

/*!
 * SPI peripheral ID
 */
typedef enum
{
    SPI_0 = 0,
    SPI_1,
    SPI_2
}SpiIds_t;

/*!
 * SPI Modes
 */
typedef enum
{
    SPI_MODE_0,   ///< SCK active high, sample on leading edge of clock.
    SPI_MODE_1,   ///< SCK active high, sample on trailing edge of clock.
    SPI_MODE_2,   ///< SCK active low, sample on leading edge of clock.
    SPI_MODE_3    ///< SCK active low, sample on trailing edge of clock.
}SpiModes_t;

/*!
 * SPI Frequency
 */
typedef enum
{
    SPI_FREQ_125K,
    SPI_FREQ_250K,
    SPI_FREQ_500K,
    SPI_FREQ_1M,
    SPI_FREQ_2M,
    SPI_FREQ_4M,
    SPI_FREQ_8M
}SpiFrequencys_t;

/*!
 * SPI bit orders.
 */
typedef enum
{
    SPI_BIT_MSB_FIRST,   ///< Most significant bit shifted out first.
    SPI_BIT_LSB_FIRST    ///< Least significant bit shifted out first.
}SpiBitOrders_t;

/*!
 * SPI object type definition
 */
typedef struct Spi_s
{
    SpiIds_t SpiId;
    nrfx_spim_t Spi_Inst;
    Gpio_t Mosi;
    Gpio_t Miso;
    Gpio_t Sclk;
    Gpio_t Nss;
}Spi_t;

/*!
 * \brief Initializes the SPI object and MCU peripheral
 *
 * \param [IN] obj  SPI object
 * \param [IN] mosi SPI MOSI pin name to be used
 * \param [IN] miso SPI MISO pin name to be used
 * \param [IN] sclk SPI SCLK pin name to be used
 * \param [IN] nss  SPI NSS pin name to be used
 */
void SpiInit( Spi_t *obj, SpiIds_t spiId, PinNames mosi, PinNames miso, PinNames sclk, PinNames nss );

/*!
 * \brief De-initializes the SPI object and MCU peripheral
 *
 * \param [IN] obj SPI object
 */
void SpiDeInit( Spi_t *obj );

// /*!
//  * \brief Configures the SPI peripheral
//  *
//  * \remark Slave mode isn't currently handled
//  *
//  * \param [IN] SpiModes_t   SPI Mode. [0,1,2,3]
//  * \param [IN] SpiFrequencys_t  SPI Frequency. [125,250,500,1M,2M,4M,8M]
//  * \param [IN] SpiBitOrders_t  SPI bit orders. [MSB,LSB]
//  */
// void SpiFormat( SpiModes_t spiMode, SpiFrequencys_t spiFrequency, SpiBitOrders_t spiBitOrder);

/*!
 * \brief Sends outData and receives inData
 *
 * \param [IN] obj     SPI object
 * \param [IN] txData data to be sent
 * \param [IN] tx_len sent data len.
 * \param [IN] rxData   Received data.
 * \param [IN] rx_len   Received data len.
 */
void SpiInOut( Spi_t *obj, uint8_t *txData , uint8_t tx_len, uint8_t *rxData, uint8_t rx_len);

/*!
 * \brief Sends outData and receives inData
 *
 * \param [IN] obj       SPI object
 * \param [IN] writeData Byte to be sent
 * \param [IN] len       sent data len.
 */
void SpiWirte( Spi_t *obj, uint8_t *writeData, uint8_t len );

/*!
 * \brief Sends outData and receives inData
 *
 * \param [IN] obj      SPI object
 * \param [IN] readData data to be received
 * \param [IN] len      Received data len.
 */
void SpiRead( Spi_t *obj, uint8_t *readData, uint8_t len );

#endif // __SPI_H__
