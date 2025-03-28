#include "led.h"

void led_init() {
    gpio_init(PIN_BLUE_LED);
    gpio_set_dir(PIN_BLUE_LED, GPIO_OUT);
}

void led_turn_on() {
    gpio_put(PIN_BLUE_LED, 1);
}

void led_turn_off() {
    gpio_put(PIN_BLUE_LED, 0);
}
