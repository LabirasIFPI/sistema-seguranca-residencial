#include <stdio.h>
#include "hardware/adc.h"

// Pinos do Joystick
#define EIXO_Y 26
#define BTN_SW 22

void joystick_init() {
    // inicializa o botão SW do joystick
    gpio_init(BTN_SW);
    gpio_set_dir(BTN_SW, GPIO_IN);
    gpio_pull_up(BTN_SW); // Configura pull-up interno

    // configurando o adc e inicializa o eixo Y
    adc_init();
    adc_gpio_init(EIXO_Y);
}

uint16_t joystick_read_current_value() {
    adc_select_input(0);
    return adc_read();
}