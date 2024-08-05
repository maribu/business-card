/*
 * Copyright (C) 2024 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_business_card
 * @{
 *
 * @file
 * @brief       Board specific definitions for Marian's Business Card
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Charlieplexed LED Matrix GPIOs
 * @{
 */
#define LED_MATRIX_TIMER        TIMER_DEV(0)    /**< Timer to use for updating the LED matrix */
#define LED_MATRIX_PORT         GPIO_PORT_A     /**< The GPIO port the LED matrix connected to */

#define LED_MATRIX_PIN_0        0               /**< GPIO pin number connected to LED matrix 0 */
#define LED_MATRIX_PIN_1        1               /**< GPIO pin number connected to LED matrix 1 */
#define LED_MATRIX_PIN_2        2               /**< GPIO pin number connected to LED matrix 2 */
#define LED_MATRIX_PIN_3        3               /**< GPIO pin number connected to LED matrix 3 */
#define LED_MATRIX_PIN_4        4               /**< GPIO pin number connected to LED matrix 4 */
#define LED_MATRIX_PIN_5        5               /**< GPIO pin number connected to LED matrix 5 */
#define LED_MATRIX_PIN_6        6               /**< GPIO pin number connected to LED matrix 6 */
#define LED_MATRIX_PIN_7        7               /**< GPIO pin number connected to LED matrix 7 */
#define LED_MATRIX_PIN_8        11              /**< GPIO pin number connected to LED matrix 8 */
#define LED_MATRIX_PIN_9        12              /**< GPIO pin number connected to LED matrix 9 */
/** @} */

/**
 * @name Charlieplexed Button Matrix GPIOs
 * @{
 */
#define BUTTON_MATRIX_PORT      GPIO_PORT_B     /**< The GPIO port the button matrix connected to */

#define BUTTON_MATRIX_PIN_0     0               /**< GPIO pin number of button matrix pin 0 */
#define BUTTON_MATRIX_PIN_1     3               /**< GPIO pin number of button matrix pin 1 */
#define BUTTON_MATRIX_PIN_2     7               /**< GPIO pin number of button matrix pin 2 */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
