/*
 * Copyright (C) 2024 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_bitmap_fonts    A library for bitmap fonts
 * @ingroup     sys
 *
 * @{
 *
 * @file
 * @brief       Interface definition of the bitmap font module
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#ifndef BITMAP_FONT_H
#define BITMAP_FONT_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Data structure holding a bitmap font of 8 pixel height and variable
 *          width
 */
typedef struct {
    const uint8_t *data;    /**< Bitmap data column-wise */
    uint16_t offsets[94];   /**< Offset of the glyphs in data */
    /**
     * @brief   Length of the data array
     *
     * This intentionally is laid out in memory that an access of offsets out
     * of bounds by 1 will access the data size. For every glyph index
     * `i`, this way `.offsets[i + 1] - .offsets[i]` will yield the glyph
     * width.
     */
    uint16_t data_size;
} bitmap_font_t;

/**
 * @brief   Data structure holding a bitmap font glyph
 */
typedef struct {
    const uint8_t *data;    /**< Bitmap data column-wise */
    uint8_t width;          /**< Width of the glyph in columns */
} bitmap_glyph_t;

/**
 * @name    Extra glyphs for emojis, icons, etc.
 * @{
 */
extern const bitmap_glyph_t bitmap_glyph_arrow_up;      /**< An arrow facing upwards */
extern const bitmap_glyph_t bitmap_glyph_arrow_down;    /**< An arrow facing downwards */
extern const bitmap_glyph_t bitmap_glyph_arrow_left;    /**< An arrow facing left */
extern const bitmap_glyph_t bitmap_glyph_arrow_right;   /**< An arrow facing right */
extern const bitmap_glyph_t bitmap_glyph_heart;         /**< A heart emoji */
extern const bitmap_glyph_t bitmap_glyph_thumb_up;      /**< A thump up emoji */
extern const bitmap_glyph_t bitmap_glyph_thumb_down;    /**< A thump down emoji */

/** @} */

/**
 * @brief   The MatrixLight8 bitmap font from https://github.com/trip5/Matrix-Fonts
 */
extern const bitmap_font_t bitmap_font_matrix_light8;

/**
 * @brief   Check if the pixel at the give x,y coordinates is set in the given glyph.
 *
 * @param[in]   glyph   Glyph to check
 * @param[in]   x       X coordinate (0 is leftmost)
 * @param[in]   y       Y coordinate (0 is topmost)
 *
 * @retval  true        The pixel at the give coordinates in @p glyph is set.
 * @retval  false       The pixel is *NOT* set.
 */
static inline bool bitmap_glyph_at(const bitmap_glyph_t *glyph, uint8_t x, uint8_t y)
{
    assert((glyph != NULL) && (y < 8) && (x < glyph->width));

    return (glyph->data[x] & (1U << y));
}

/**
 * @brief   Get the glyph of the given character
 *
 * @param[in]   font        The bitmap font to get the glyph from
 * @param[in]   codepoint   The character to get the glyph of
 *
 * @return  The requested glyph
 * @retval  `?`             if `(codepoints < 0x20) || (codepoint > 0x7e)`
 */
bitmap_glyph_t bitmap_font_get(const bitmap_font_t *font, char codepoint);

/**
 * @brief   Get the width (in pixels) of the given text rendered in the given
 *          font.
 * @param[in]   font                The bitmap font that will be used for rendering
 * @param[in]   text                The text to render
 * @param[in]   text_len            Size of @p text in bytes
 */
size_t bitmap_font_render_width(const bitmap_font_t *font, const char *text, size_t text_len);

/**
 * @brief   Should a pixel of space be added between the to glyphs?
 *
 * @param[in]   left    Glyph rendered left
 * @param[in]   right   Glyph rendered right
 *
 * @retval  true    If a space is needed between @p left and @p right
 * @retval  false   No space should be added (e.g. between "o" and "T")
 */
bool bitmap_glyph_space_between(const bitmap_glyph_t *left, const bitmap_glyph_t *right);

#ifdef __cplusplus
}
#endif

#endif /* BITMAP_FONT_H */
/** @} */
