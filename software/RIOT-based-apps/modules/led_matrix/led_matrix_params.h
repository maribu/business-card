/*
 * Copyright (C) 2024 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_led_matrix
 * @{
 *
 * @file
 * @brief       Hardware parameters of the LED matrix
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#ifndef LED_MATRIX_PARAMS_H
#define LED_MATRIX_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef LED_MATRIX_TIMER
#  error "LED_MATRIX_TIMER not defined"
#endif

#ifndef LED_MATRIX_PORT
#  error "LED_MATRIX_PORT not defined"
#endif

/**
 * @brief   Pin numbers of the LEDs matrix
 */
static const uint8_t led_matrix_pins[] = {
    LED_MATRIX_PIN_0,
    LED_MATRIX_PIN_1,
#ifdef LED_MATRIX_PIN_2
    LED_MATRIX_PIN_2,
#endif
#ifdef LED_MATRIX_PIN_3
    LED_MATRIX_PIN_3,
#endif
#ifdef LED_MATRIX_PIN_4
    LED_MATRIX_PIN_4,
#endif
#ifdef LED_MATRIX_PIN_5
    LED_MATRIX_PIN_5,
#endif
#ifdef LED_MATRIX_PIN_6
    LED_MATRIX_PIN_6,
#endif
#ifdef LED_MATRIX_PIN_7
    LED_MATRIX_PIN_7,
#endif
#ifdef LED_MATRIX_PIN_8
    LED_MATRIX_PIN_8,
#endif
#ifdef LED_MATRIX_PIN_9
    LED_MATRIX_PIN_9,
#endif
#ifdef LED_MATRIX_PIN_10
    LED_MATRIX_PIN_10,
#endif
#ifdef LED_MATRIX_PIN_11
    LED_MATRIX_PIN_11,
#endif
#ifdef LED_MATRIX_PIN_12
    LED_MATRIX_PIN_12,
#endif
#ifdef LED_MATRIX_PIN_13
    LED_MATRIX_PIN_13,
#endif
#ifdef LED_MATRIX_PIN_14
    LED_MATRIX_PIN_14,
#endif
#ifdef LED_MATRIX_PIN_15
    LED_MATRIX_PIN_15,
#endif
};

#define LED_MATRIX_PIN_NUMOF    ARRAY_SIZE(led_matrix_pins) /**< Number of LED matrix pins */
#define LED_MATRIX_WIDTH        LED_MATRIX_PIN_NUMOF        /**< Width of the LED matrix */
#define LED_MATRIX_HEIGHT       (LED_MATRIX_PIN_NUMOF - 1)  /**< Height of the LED matrix */

/**
 * @brief   Number of LEDs in the matrix
 */
#define LED_MATRIX_LED_NUMOF    (LED_MATRIX_WIDTH * LED_MATRIX_HEIGHT)

#ifdef __cplusplus
}
#endif

#endif /* LED_MATRIX_PARAMS_H */
/** @} */
