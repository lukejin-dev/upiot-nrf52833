#ifndef __BOARD_CONFIG_H__
#define __BOARD_CONFIG_H__

#include "pinName-board.h"

// SX126X PIN
#define SX126X_RESET            P0_20
#define SX126X_MOSI             P0_11
#define SX126X_SCLK             P1_09
#define SX126X_NSS              P0_17
#define SX126X_DIO1             P0_02

#define SX126X_BUSY             P0_29
#define SX126X_MISO             P0_30
#define SI7021_SDA              P0_31

#define BOARD_LED_1             P0_13
#define BOARD_LED_2             P0_14
#define BOARD_LED_3             P0_16

#define SI7021_SCL              P0_04

#define LED_LORA_TX             0

#define BOARD_LED_LIST { BOARD_LED_1, BOARD_LED_2, BOARD_LED_3 }

#endif // __BOARD_CONFIG_H__