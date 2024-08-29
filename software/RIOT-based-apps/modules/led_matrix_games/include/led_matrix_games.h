/*
 * Copyright (C) 2024 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_led_matrix_games    Games using the LED matrix and the button matrix module
 * @ingroup     sys
 *
 * This module contains games that are displayed on a (charlieplexed) LED
 * matrix using the `led_matrix` module and rely on input via (charlieplexed)
 * button matrix using the `button_matrix` module.
 *
 * @{
 *
 * @file
 * @brief       Interface definition of the `led_matrix` module
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 */

#ifndef LED_MATRIX_GAMES_H
#define LED_MATRIX_GAMES_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Types needed for the flappy LED game
 * @{
 */

/**
 * @brief   An obstacle to fly through
 */
struct flappy_led_obstacle {
    uint8_t gap_top;
    uint8_t gap_size;
    uint8_t distance;
};

/**
 * @brief   Game data for the flappy LED game
 */
struct flappy_led_game_data {
    uint32_t seed;                              /**< Current PRNG seed */
    uint32_t score;                             /**< Current score */
    int y_offset;                               /**< Used to track the current altitude of the flappy LED */
    struct flappy_led_obstacle obstacles[8];    /**< Obstacles currently tracked as ring buffer */
    uint8_t obstacles_numof;                    /**< Current number of obstacles */
    uint8_t obstacles_pos;                      /**< position of the first obstacle in the ring buffer */
    uint8_t new_obstacle_distance;              /**< Distance to spawn a new obstacle with */
    uint8_t first_obstacle_x;                   /**< X coordinate of the first obstacle */
    uint8_t obstacle_min_gap;                   /**< Minimum size of the gap in an obstacle */
};
/** @} */

/**
 * @brief   Shared game data
 *
 * Since at most one game can be running at a time anyway, we can share RAM tracking
 * the game state between all games by using a `union`.
 */
typedef union {
    struct flappy_led_game_data flappy_led; /**< Data for the flappy LED game */
} led_matrix_games_data_t;

/**
 * @brief   The static allocation of the game data shared between games
 */
extern led_matrix_games_data_t led_matrix_games_data;

/**
 * @brief   Get the next PRNG sequence value
 * @param[in]   x       The previous value
 * @return      The next PRNG sequence value
 *
 * @details Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs"
 */
static inline uint32_t xorshift32(uint32_t x)
{
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return x;
}

/**
 * @brief   Run the flappy LED game until one game is over
 *
 * @pre     @ref led_matrix_init and @ref button_matrix_init has been called
 *
 * @details Call this in a loop to restart the game after the player
 *          crashed into an obstacle and the score was shown.
 */
void led_matrix_games_flappy_led(void);

/**
 * @brief   Run the "LEDmon says" game until one game is over
 *
 * @pre     @ref led_matrix_init and @ref button_matrix_init has been called
 *
 * @details Call this in a loop to restart the game after the player
 *          failed to repeat the button sequence shown correctly.
 */
void led_matrix_games_ledmon_says(void);

#ifdef __cplusplus
}
#endif

#endif /* LED_MATRIX_GAMES_H */
/** @} */
