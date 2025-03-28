#include "joystick.h"

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

void joystick_enable_interrupt() {
    gpio_set_irq_enabled(BTN_SW, GPIO_IRQ_EDGE_FALL, true);
}

void joystick_disable_interrupt() {
    gpio_set_irq_enabled(BTN_SW, GPIO_IRQ_EDGE_FALL, false);
}