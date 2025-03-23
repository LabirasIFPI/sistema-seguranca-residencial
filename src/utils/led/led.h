#include "pico/stdlib.h"

#define PIN_BLUE_LED 12        // Pino do LED azul

/*
* função para iniciarlzar o led RGB
*/
void led_init() {
    gpio_init(PIN_BLUE_LED);
    gpio_set_dir(PIN_BLUE_LED, GPIO_OUT);
}

/*
* função para ligar o led com a cor Azul
*/
void led_turn_on() {
    gpio_put(PIN_BLUE_LED, 1);
}

/*
* função para desligar o led
*/
void led_turn_off() {
    gpio_put(PIN_BLUE_LED, 0);
}