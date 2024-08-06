/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       Hello World application
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "led_matrix.h"

int main(void)
{
    puts("Hello World!");

    printf("You are running RIOT on a(n) %s board.\n", RIOT_BOARD);
    printf("This board features a(n) %s CPU.\n", RIOT_CPU);

    const uint32_t frames_per_level = 2;
    uint32_t frame_target = led_matrix_frame_number() + frames_per_level;

    led_matrix_fb_clear();
    for (;;) {
        const char *text = "IoT";
        const size_t len = 3;

        for (uint8_t b = 1; b < LED_MATRIX_BRIGHTNESS_LEVELS; b++) {
            led_matrix_text(&bitmap_font_matrix_light8, text, len, 1, 1, b);
            frame_target = led_matrix_fb_switch(frame_target) + frames_per_level;
        }

        for (uint8_t b = LED_MATRIX_BRIGHTNESS_MAX; b > 0; b--) {
            led_matrix_text(&bitmap_font_matrix_light8, text, len, 1, 1, b);
            frame_target = led_matrix_fb_switch(frame_target) + frames_per_level;
        }
    }

    return 0;
}
