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

#include <stdint.h>

#include "container.h"
#include "include/bitmap_fonts.h"

static const uint8_t _arrow_up[] = {
    0x08, 0x04, 0x02, 0x7f, 0x02, 0x04, 0x08
};
const bitmap_glyph_t bitmap_glyph_arrow_up = {
    .data = _arrow_up,
    .width = ARRAY_SIZE(_arrow_up),
};

static const uint8_t _arrow_down[] = {
    0x08, 0x10, 0x20, 0x7f, 0x20, 0x10, 0x08
};
const bitmap_glyph_t bitmap_glyph_arrow_down = {
    .data = _arrow_down,
    .width = ARRAY_SIZE(_arrow_down),
};

static const uint8_t _arrow_left[] = {
    0x08, 0x1c, 0x2a, 0x49, 0x08, 0x08, 0x08, 0x08
};
const bitmap_glyph_t bitmap_glyph_arrow_left = {
    .data = _arrow_left,
    .width = ARRAY_SIZE(_arrow_left),
};

static const uint8_t _arrow_right[] = {
    0x08, 0x08, 0x08, 0x08, 0x49, 0x2a, 0x1c, 0x08
};
const bitmap_glyph_t bitmap_glyph_arrow_right = {
    .data = _arrow_right,
    .width = ARRAY_SIZE(_arrow_right),
};

static const uint8_t _heart[] = {
    0x0c, 0x1e, 0x3e, 0x7c, 0x3e, 0x1e, 0x0c
};
const bitmap_glyph_t bitmap_glyph_heart = {
    .data = _heart,
    .width = ARRAY_SIZE(_heart),
};

static const uint8_t _thumb_up[] = {
    0x3c, 0x24, 0x24, 0x7c, 0x42, 0x41, 0x46, 0x38
};
const bitmap_glyph_t bitmap_glyph_thumb_up = {
    .data = _thumb_up,
    .width = ARRAY_SIZE(_thumb_up),
};

static const uint8_t _thumb_down[] = {
    0x1e, 0x12, 0x12, 0x1f, 0x21, 0x41, 0x31, 0x0e
};
const bitmap_glyph_t bitmap_glyph_thumb_down = {
    .data = _thumb_down,
    .width = ARRAY_SIZE(_thumb_down),
};
