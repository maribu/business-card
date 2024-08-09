/*
 * Copyright (C) 2024 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_button_matrix  Charlieplexed Button Matrix Driver
 * @ingroup     drivers
 *
 * This module contains the driver to use a charlieplexed Button matrix.
 * It assumes a single fully populated button matrix being present
 * using the same wiring pattern given in the following schematic:
 *
 * @image html ../graphics/schematics.svg "Assumed schematic of the button matrix" width=100%
 *
 * @warning     The driver assumes that all GPIOs driving the matrix are
 *              on the same GPIO port.
 *
 * While the schematic above uses 3 GPIOs to sample 6 buttons the driver is
 * written to work with up to 16 GPIO pins (or to sample up to 240 buttons).
 *
 * The buttons are referred to by x and y coordinates starting from the topmost
 * leftmost button. Unlike the LED Matrix driver, the diagonal gap in the matrix
 * is part of the coordinate, e.g. `(0, 0)` refers to SW1, `(1, 0)` is SW2,
 * `(0, 1)` is SW2 and `(2, 0)` is non-existing.
 *
 * The board configuration needs to provide the following macros:
 *
 * ```C
 * // The GPIO port of all GPIO pins driving the button matrix belong to
 * #define BUTTON_MATRIX_PORT       GPIO_PORT_x
 *
 * // The GPIO pin numbers driving the button matrix
 * #define BUTTON_MATRIX_PIN_0      7
 * #define BUTTON_MATRIX_PIN_1      3
 * #define BUTTON_MATRIX_PIN_2      9
 * #define BUTTON_MATRIX_PIN_3      11
 * ...
 * ```
 *
 * The driver is written under the assumption that only a single thread will
 * sample the buttons. Two threads concurrently using the API will cause
 * races.
 *
 * @warning     When pressing more than one button at once, some additional
 *              buttons may incorrectly be reported due to rollover
 *
 * @{
 *
 * @file
 * @brief       Interface definition of the `button_matrix` module
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#ifndef BUTTON_MATRIX_H
#define BUTTON_MATRIX_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Prepares the button matrix for sampling
 *
 * @retval  0       Success
 * @retval  <0      Configuring the GPIOs failed, error code is passed through
 */
int button_matrix_init(void);

/**
 * @brief   Read out the button state at the given position
 *
 * @retval  `true`      The button is pressed (or rollover)
 * @retval  `false`     The button is **NOT** pressed
 */
bool button_matrix_test(uint8_t x, uint8_t y);

/**
 * @brief   Scan the whole button matrix and write the result
 *
 * @param[out]  dest    The bit vector to write the input to
 */
void button_matrix_scan(uint8_t *dest);

#ifdef __cplusplus
}
#endif

#endif /* BUTTON_MATRIX_H */
/** @} */
