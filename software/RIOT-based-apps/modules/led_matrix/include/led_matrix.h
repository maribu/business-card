/*
 * Copyright (C) 2024 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_led_matrix  Charlieplexed LED Matrix Driver
 * @ingroup     drivers
 *
 * This module contains the driver to use a charlieplexed LED matrix as
 * display. It assumes a single fully populated LED matrix being present
 * using the same wiring pattern given in the following schematic:
 *
 * @image html ../graphics/schematics.svg "Assumed schematic of the LED matrix" width=100%
 *
 * @warning     The driver assumes that all GPIOs driving the matrix are
 *              on the same GPIO port.
 *
 * While the schematic above using 10 GPIOs to drive 90 LEDs, the code is
 * written to work with up to 16 GPIO pins (or to drive up to 240 LEDs).
 * The board configuration needs to provide the following macros:
 *
 * ```C
 * // The timer used for drawing to the matrix
 * #define LED_MATRIX_TIMER         TIMER_DEV(x)
 *
 * // The GPIO port of all GPIO pins driving the LED matrix belong to
 * #define LED_MATRIX_PORT          GPIO_PORT_x
 *
 * // The GPIO pin numbers driving the LED matrix
 * #define LED_MATRIX_PIN_0         7
 * #define LED_MATRIX_PIN_1         3
 * #define LED_MATRIX_PIN_2         9
 * #define LED_MATRIX_PIN_3         11
 * ...
 * ```
 *
 * The driver is written under the assumption that only a single thread will
 * perform the rendering and therefore is not thread safe.
 *
 * The LED matrix is updated using a periodic timer IRQ. Double buffering is
 * used to allow rendering to a scratch framebuffer without visual glitches.
 *
 * @{
 *
 * @file
 * @brief       Interface definition of the `led_matrix` module
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#ifndef LED_MATRIX_H
#define LED_MATRIX_H

#include <stdint.h>

#include "periph/gpio_ll.h"
#include "periph/timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set the brightness of the given LED matrix in the scratch
 *          frame buffer
 *
 * @param   x           X-coordinate (starting from leftmost pixel at 0)
 * @param   y           Y-coordinate (starting from topmost pixel at 0)
 * @param   brightness  How bright the pixel should be (0 is off, 3 is
 *                      maximum brightness)
 *
 * @note    Calling this function with out of range values for
 *          @p x and @p y is safe and will leave the scratch frame
 *          buffer unmodified.
 *
 * @warning This function is not thread-safe. The caller must ensure
 *          that no other thread is concurrently accessing the
 *          LED matrix's frame buffers.
 */
void led_matrix_fb_set(int x, int y, uint8_t brightness);

/**
 * @brief   Clear all pixels in the scratch buffer
 *
 * @warning This function is not thread-safe. The caller must ensure
 *          that no other thread is concurrently accessing the
 *          LED matrix's frame buffers.
 */
void led_matrix_fb_clear(void);

/**
 * @brief   Switch the active buffer with the scratch buffer
 *
 * This will result in the scratch buffer becoming active and rendered, and the
 * previously shown buffer becoming the new scratch buffer. The switch is
 * thread safe.
 */
void led_matrix_fb_switch(void);

/**
 * @brief   Prepares the LED matrix and configures and enabled the
 *          periodic timer ISR to draw the matrix
 *
 * @retval  0       Success
 * @retval  <0      Configuring the GPIOs or the timer failed, error code
 *                  is passed through
 */
int led_matrix_init(void);

/**
 * @brief   Delay execution of the calling thread until (at least) the
 *          frame number given has been fully rendered.
 *
 * @param   frame_number    The target frame number to wait for
 * @pre     @p frame_number is no more than `UINT16_MAX` frames in
 *          the future. Otherwise the code will assume the frame counter
 *          has overflown and the target has already been shown.
 *
 * The timer ISR will increment a frame counter whenever the
 * last pixel of the active frame buffer has been shown. This function
 * will delay execution of the calling thread until the value has reached
 * (or exceeded), it will return.
 *
 * This function is thread-safe.
 */
void led_matrix_wait_for_frame(uint32_t frame_number);

/**
 * @brief   Get the current value of the frame counter
 *
 * @return  The frame number currently being shown
 *
 * This function is thread-safe.
 */
uint32_t led_matrix_frame_number(void);

/**
 * @brief   Scroll the given text through the LED matrix
 *
 * @param   text        The text to render
 * @param   brightness  The brightness of the text
 *
 * Control is returned to the calling thread once the last letter of the
 * message has scrolled out of the screen.
 *
 * @warning This function is not thread-safe. The caller must ensure
 *          that no other thread is concurrently accessing the
 *          LED matrix's frame buffers.
 */
void led_matrix_print(const char *text, size_t len, uint8_t brightness);
#ifdef __cplusplus
}
#endif

#endif /* LED_MATRIX_H */
/** @} */
