/*
 * Copyright (C) 2024 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_button_matrix
 * @{
 *
 * @file
 * @brief       Hardware parameters of the button matrix
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#ifndef BUTTON_MATRIX_PARAMS_H
#define BUTTON_MATRIX_PARAMS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BUTTON_MATRIX_PORT
#  error "BUTTON_MATRIX_PORT not defined"
#endif

/**
 * @brief   Pin numbers of the button matrix
 */
static const uint8_t button_matrix_pins[] = {
    BUTTON_MATRIX_PIN_0,
    BUTTON_MATRIX_PIN_1,
#ifdef BUTTON_MATRIX_PIN_2
    BUTTON_MATRIX_PIN_2,
#endif
#ifdef BUTTON_MATRIX_PIN_3
    BUTTON_MATRIX_PIN_3,
#endif
#ifdef BUTTON_MATRIX_PIN_4
    BUTTON_MATRIX_PIN_4,
#endif
#ifdef BUTTON_MATRIX_PIN_5
    BUTTON_MATRIX_PIN_5,
#endif
#ifdef BUTTON_MATRIX_PIN_6
    BUTTON_MATRIX_PIN_6,
#endif
#ifdef BUTTON_MATRIX_PIN_7
    BUTTON_MATRIX_PIN_7,
#endif
#ifdef BUTTON_MATRIX_PIN_8
    BUTTON_MATRIX_PIN_8,
#endif
#ifdef BUTTON_MATRIX_PIN_9
    BUTTON_MATRIX_PIN_9,
#endif
#ifdef BUTTON_MATRIX_PIN_10
    BUTTON_MATRIX_PIN_10,
#endif
#ifdef BUTTON_MATRIX_PIN_11
    BUTTON_MATRIX_PIN_11,
#endif
#ifdef BUTTON_MATRIX_PIN_12
    BUTTON_MATRIX_PIN_12,
#endif
#ifdef BUTTON_MATRIX_PIN_13
    BUTTON_MATRIX_PIN_13,
#endif
#ifdef BUTTON_MATRIX_PIN_14
    BUTTON_MATRIX_PIN_14,
#endif
#ifdef BUTTON_MATRIX_PIN_15
    BUTTON_MATRIX_PIN_15,
#endif
};

#define BUTTON_MATRIX_PIN_NUMOF     ARRAY_SIZE(button_matrix_pins)  /**< Number of BUTTON matrix pins */
#define BUTTON_MATRIX_WIDTH         BUTTON_MATRIX_PIN_NUMOF         /**< Width of the BUTTON matrix */
#define BUTTON_MATRIX_HEIGHT        BUTTON_MATRIX_PIN_NUMOF         /**< Height of the BUTTON matrix */

/**
 * @brief   Number of BUTTONs in the matrix
 */
#define BUTTON_MATRIX_BUTTON_NUMOF  (BUTTON_MATRIX_WIDTH * (BUTTON_MATRIX_HEIGHT - 1))

#ifdef __cplusplus
}
#endif

#endif /* BUTTON_MATRIX_PARAMS_H */
/** @} */
