/*
 * Copyright (C) 2024 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_bitmap_fonts
 * @{
 *
 * @file
 * @brief       Implementation of common bitmap font functions
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * @}
 */

#include <assert.h>
#include <stdint.h>

#include "include/bitmap_fonts.h"

bitmap_glyph_t bitmap_font_get(const bitmap_font_t *font, char codepoint)
{
    assert(font != NULL);

    unsigned idx = (uint8_t)codepoint;
    idx -= 0x20;
    if (idx > (0x7e - 0x20)) {
        idx = (uint8_t)'?' - 0x020;
    }

    /* Dark Magic: The access `font->offsets[idx + 1]` may overflow by one
     * element and access the struct member `data_size` instead; which contains
     * exactly the value we are after in the case it overflows :) */
    bitmap_glyph_t result = {
        .data = font->data + font->offsets[idx],
        .width = font->offsets[idx + 1] - font->offsets[idx],
    };

    return result;
}


size_t bitmap_font_render_width(const bitmap_font_t *font, const char *text, size_t text_len)
{
    assert(font != NULL && text != NULL);

    bitmap_glyph_t left = bitmap_font_get(font, *text++);
    text_len--;
    size_t result = left.width;

    while (text_len--) {
        bitmap_glyph_t right = bitmap_font_get(font, *text++);
        result += bitmap_glyph_space_between(&left, &right);
        result += right.width;
        left = right;
    }

    return result;
}

bool bitmap_glyph_space_between(const bitmap_glyph_t *left, const bitmap_glyph_t *right)
{
    assert((left != NULL) && (right != NULL));

    uint16_t x1 = left->width - 1;
    const uint16_t x2 = 0;
    if ((left->data[x1] & 1U) && (right->data[x2] & 1U)) {
        return true;
    }

    for (unsigned y = 1; y < 8; y++) {
        if ((left->data[x1] & (1U << y)) && (right->data[x2] & (1U << y))) {
            return true;
        }

        if ((left->data[x1] & (1U << (y - 1))) && (right->data[x2] & (1U << y))) {
            return true;
        }

        if ((left->data[x1] & (1U << y)) && (right->data[x2] & (1U << (y - 1)))) {
            return true;
        }
    }

    return false;
}
