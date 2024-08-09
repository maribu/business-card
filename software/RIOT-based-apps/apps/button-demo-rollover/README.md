# Button Rollover Demo

The use of Charlieplexing for has the disadvantage that when more than one
button is pressed at the same time, it may be impossible to reliably determine
which set of buttons is pressed.

E.g. the up button is detected when current can flow from `BUTTON_MATRIX_PIN_0`
to `BUTTON_MATRIX_PIN_2`, the right button is detected when current can flow
from `BUTTON_MATRIX_PIN_2` to `BUTTON_MATRIX_PIN_1`, and the left button is
detected when current can flow from `BUTTON_MATRIX_PIN_0` to
`BUTTON_MATRIX_PIN_1`. If now both the up and the right button are pressed,
current can flow from `BUTTON_MATRIX_PIN_0` to `BUTTON_MATRIX_PIN_2` (via the
closed up button) and from `BUTTON_MATRIX_PIN_2` to `BUTTON_MATRIX_PIN_1` (via
the closed right button). Hence, current can also flow from pin 0 to pin 1 (via
pin 2), regardless of whether the left button is pressed or not.

The board provides the `compensate_rollover()` function that can help here:
Under the assumption that at most two buttons are pressed at the same time,
when registering three pressed buttons the ghost key can be reliably determined.

This app showcases this by displaying the raw button input at the top of
the screen and the compensated one at the bottom. Note that when pressing
three buttons at the same time, this simple heuristic is not sufficient.
