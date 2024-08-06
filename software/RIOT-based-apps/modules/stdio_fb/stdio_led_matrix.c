/*
 * Copyright (C) 2024 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       STDIO over LED Matrix implementation
 *
 * This file implements stdio output of the LED Matrix
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * @}
 */

#include "bitmap_fonts.h"
#include "irq.h"
#include "led_matrix.h"
#include "stdio_base.h"

#ifndef STDIO_LED_MATRIX_FONT
#  define STDIO_LED_MATRIX_FONT &bitmap_font_matrix_light8
#endif

#ifndef STDIO_LED_MATRIX_BRIGHTNESS
#  define STDIO_LED_MATRIX_BRIGHTNESS   LED_MATRIX_BRIGHTNESS_MAX
#endif

static void _init(void)
{
    int retval = led_matrix_init();
    assert(retval == 0);
    (void)retval;
}

static ssize_t _write(const void *_buffer, size_t len)
{
    const char *buffer = _buffer;
    size_t retval = len;

    if (irq_is_in()) {
        return 0;
    }

    for (size_t nlpos = 0; nlpos < len; nlpos++) {
        if (buffer[nlpos] == '\n') {
            led_matrix_text_scroll(STDIO_LED_MATRIX_FONT, buffer, nlpos,
                                   STDIO_LED_MATRIX_BRIGHTNESS);

            len -= nlpos + 1;
            buffer += nlpos + 1;
            nlpos = 0;
        }
    }

    if (len) {
        led_matrix_text_scroll(STDIO_LED_MATRIX_FONT, buffer, len,
                               STDIO_LED_MATRIX_BRIGHTNESS);
    }

    return retval;
}

STDIO_PROVIDER(STDIO_UART, _init, NULL, _write)
