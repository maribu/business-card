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

#include "irq.h"
#include "led_matrix.h"
#include "stdio_base.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static void _init(void)
{
    int retval = led_matrix_init();
    assert(retval == 0);
    (void)retval;
}

static ssize_t _write(const void *buffer, size_t len)
{
    if (irq_is_in()) {
        return 0;
    }

    led_matrix_print(buffer, len, 3);
    return len;
}

STDIO_PROVIDER(STDIO_UART, _init, NULL, _write)
