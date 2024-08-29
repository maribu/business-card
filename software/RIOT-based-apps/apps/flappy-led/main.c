#include <assert.h>
#include <stdbool.h>

#include "button_matrix.h"
#include "led_matrix.h"
#include "led_matrix_games.h"

int main(void)
{
    int retval;

    retval = led_matrix_init();
    assert(retval == 0);

    retval = button_matrix_init();
    assert(retval == 0);
    (void)retval;

    while (1) {
        led_matrix_games_flappy_led();
    }

    return 0;
}
