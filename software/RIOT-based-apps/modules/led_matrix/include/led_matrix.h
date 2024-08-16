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
 *
 * The LEDs are referred to by `x` and `y` coordinates in this module, with
 * `(0, 0)` being the topmost-leftmost LED. Incrementing `x` by one will go one
 * LED to the right, incrementing `y` by one will go one LED down. E.g. `(0,
 * 0)` would refer to D7 in the schematic, `(2, 1)` to D26, and `(7, 2)` to
 * D72. Or in other words: The diagonal gap in the schematic is silently
 * stepped over and the coordinates match exactly the physical layout on the
 * PCB, which has no diagonal gap.
 *
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

#include "bitmap_fonts.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The number of bits used to encode a pixel in the LED matrix
 */
#define LED_MATRIX_BRIGHTNESS_BITS      4U

/**
 * @brief   The number different brightness levels supported
 *
 * The human eye perceives brightness in a non-linear fashion, but this
 * implementation will increase the time a given pixel is active per
 * frame linearly with the brightness given.
 */
#define LED_MATRIX_BRIGHTNESS_LEVELS    (1U << LED_MATRIX_BRIGHTNESS_BITS)

/**
 * @brief   The highest brightness value supported
 */
#define LED_MATRIX_BRIGHTNESS_MAX       (LED_MATRIX_BRIGHTNESS_LEVELS - 1U)


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
 * @brief   Switch the active buffer with the scratch buffer at
 *          just before drawing the given frame number
 *
 * @param[in]   at_frame_number     The number of the frame to switch at
 *
 * This function will block the caller until the frame buffer has been
 * switched. The switch is done in the drawing ISR right between
 * `at_frame_number - 1` and `at_frame_number`, if that point in
 * time is still in the future. Otherwise the framebuffer is switched
 * at the next frame
 *
 * @return  The frame that the frame buffer was switched at
 *
 * @warning This function is not thread-safe. The caller must ensure
 *          that no other thread is concurrently accessing the
 *          LED matrix's frame buffers.
 */
uint32_t led_matrix_fb_switch(uint32_t at_frame_number);

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
 * @brief   Render the given glyph into the scratch frame buffer
 * @param[in]   glyph   The glyph to place
 * @param[in]   x       X coordinate to place the topmost corner of the glyph
 * @param[in]   y       Y coordinate to place the leftmost corner of the glyph
 * @param[in]   brightness  The brightness of the glyph
 */
void led_matrix_glyph(const bitmap_glyph_t *glyph, int x, int y, uint8_t brightness);

/**
 * @brief   Render the given text into the frame buffer
 *
 * @param[in]   font        The bitmap font to use
 * @param[in]   text        The text to render
 * @param[in]   len         Length of @p text in bytes
 * @param[in]   xoffset     Move the text right (positive `xoffset`) or left
 *                          (negative `xoffset`) (in pixels)
 * @param[in]   yoffset     Move the text down (positive `yoffset`) or up
 *                          (negative `yoffset`) (in pixels)
 * @param[in]   brightness  The brightness of the text
 *
 * Control is returned to the calling thread once the last letter of the
 * message has scrolled out of the screen.
 *
 * @warning This function is not thread-safe. The caller must ensure
 *          that no other thread is concurrently accessing the
 *          LED matrix's frame buffers.
 */
void led_matrix_text(const bitmap_font_t *font, const char *text, size_t len,
                     int xoffset, int yoffset, uint8_t brightness);

/**
 * @brief   Show an animation that scrolls the given text rendered with the
 *          given font through the LED matrix
 *
 * @param[in]   font        The bitmap font to use
 * @param[in]   text        The text to render
 * @param[in]   len         Length of @p text in bytes
 * @param[in]   brightness  The brightness of the text
 *
 * @warning This function is not thread-safe. The caller must ensure
 *          that no other thread is concurrently accessing the
 *          LED matrix's frame buffers.
 *
 * This function returns once the full text has scrolled through the
 * LED matrix and the screen is blank again.
 */
void led_matrix_text_scroll(const bitmap_font_t *font, const char *text, size_t len,
                            uint8_t brightness);

/**
 * @brief   Similar to @ref led_matrix_text_scroll but loops through the
 *          text until at least one of the given set of buttons is pressed
 *
 * @param[in]   font        The bitmap font to use
 * @param[in]   text        The text to render
 * @param[in]   len         Length of @p text in bytes
 * @param[in]   btn_filter  Bitmask specifying the buttons used to exit the message
 * @param[out]  btn_target  Bitmask of the buttons actually pressed
 * @param[in]   btn_len     Size of @p btn_filter and @p btn_target
 * @param[in]   brightness  The brightness of the text
 *
 * @pre     @p btn_len equals `(BUTTON_MATRIX_BUTTON_NUMOF + 7) / 8)` (or is exactly
 *          the size needed to hold all buttons present in the board)
 *
 * @warning This function is only provided if module `button_matrix` is also used.
 */
void led_matrix_text_scroll_until_button(const bitmap_font_t *font,
                                         const char *text, size_t len,
                                         const uint8_t *btn_filter,
                                         uint8_t *btn_target,
                                         size_t btn_len,
                                         uint8_t brightness);
#ifdef __cplusplus
}
#endif

#endif /* LED_MATRIX_H */
/** @} */
