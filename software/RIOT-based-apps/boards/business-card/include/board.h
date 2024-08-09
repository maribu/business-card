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
#define BUTTON_MATRIX_PORT              GPIO_PORT_B     /**< The GPIO port the button matrix connected to */

#define BUTTON_MATRIX_PIN_0             7               /**< GPIO pin number of button matrix pin 0 */
#define BUTTON_MATRIX_PIN_1             4               /**< GPIO pin number of button matrix pin 1 */
#define BUTTON_MATRIX_PIN_2             0               /**< GPIO pin number of button matrix pin 2 */

/* for button_matrix_test() */
#define BUTTON_MATRIX_COORD_UP          0,0     /**< Button matrix coordinates of the up button */
#define BUTTON_MATRIX_COORD_DOWN        0,1     /**< Button matrix coordinates of the up button */
#define BUTTON_MATRIX_COORD_LEFT        1,0     /**< Button matrix coordinates of the left button */
#define BUTTON_MATRIX_COORD_RIGHT       1,2     /**< Button matrix coordinates of the right button */
#define BUTTON_MATRIX_COORD_A           2,1     /**< Button matrix coordinates of the "A" button */
#define BUTTON_MATRIX_COORD_B           2,2     /**< Button matrix coordinates of the "B" button */

/* for button_matrix_scan() */
#define BUTTON_UP       0x01    /**< Bitmask to check for the up button */
#define BUTTON_LEFT     0x02    /**< Bitmask to check for the left button */
#define BUTTON_DOWN     0x04    /**< Bitmask to check for the down button */
#define BUTTON_A        0x08    /**< Bitmask to check for the "A" button */
#define BUTTON_RIGHT    0x10    /**< Bitmask to check for the right button */
#define BUTTON_B        0x20    /**< Bitmask to check for the "B" button */

/**
 * @brief   Heuristic to filter out fake inputs due to rollover
 *
 * The currently implemented heuristic assumes that at most two buttons are
 * pressed at the same time. E.g. pressing both up and right button will
 * result in a ghost reading for button left. So when up, right, and left
 * are enabled, it will assume that in fact only up and right are pressed
 * and left is a ghost input, which is trimmed.
 *
 * As long as indeed only two buttons are pressed at the same time, this
 * simple heuristic is sufficient. More sophisticated heuristics are
 * certainly possible that e.g. track the state over time. But
 * 2-key-rollover seems to be good enough for most games.
 *
 * @param[in]   scan    The output of button_matrix_scan()
 * @returl  The bitmask without rollover artifacts
 */
uint8_t compensate_rollover(uint8_t scan);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
