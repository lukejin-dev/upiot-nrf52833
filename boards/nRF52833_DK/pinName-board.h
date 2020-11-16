/*!
 * \file      pinName-board.h
 *
 * \brief     Target board GPIO pins definitions
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
#ifndef __PIN_NAME_BOARD_H__
#define __PIN_NAME_BOARD_H__

#ifdef __cplusplus
extern "C"
{
#endif

/*!
 * nRF52833 Pin Names
 */
#define MCU_PINS \
    P0_00 = 0, P0_01, P0_02, P0_03, P0_04, P0_05, P0_06, P0_07, \
        P0_08, P0_09, P0_10, P0_11, P0_12, P0_13, P0_14, P0_15, \
        P0_16, P0_17, P0_18, P0_19, P0_20, P0_21, P0_22, P0_23, \
        P0_24, P0_25, P0_26, P0_27, P0_28, P0_29, P0_30, P0_31, \
        P1_00, P1_01, P1_02, P1_03, P1_04, P1_05, P1_06, P1_07, \
        P1_08, P1_09

#ifdef __cplusplus
}
#endif

#endif // __PIN_NAME_BOARD_H__
