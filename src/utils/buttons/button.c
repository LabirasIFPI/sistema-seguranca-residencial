#include "button.h"

void button_init() {
    // inicializa o botão A
    gpio_init(PIN_BTN_A);
    gpio_set_dir(PIN_BTN_A, GPIO_IN);
    gpio_pull_up(PIN_BTN_A); // Configura pull-up interno

    // inicializa o botão B
    gpio_init(PIN_BTN_B);
    gpio_set_dir(PIN_BTN_B, GPIO_IN);
    gpio_pull_up(PIN_BTN_B); // Configura pull-up interno
}


void button_enable_interrupt() {
    gpio_set_irq_enabled(PIN_BTN_B, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(PIN_BTN_A, GPIO_IRQ_EDGE_FALL, true);
}


void button_disable_interrupt() {
    gpio_set_irq_enabled(PIN_BTN_B, GPIO_IRQ_EDGE_FALL, false);
    gpio_set_irq_enabled(PIN_BTN_A, GPIO_IRQ_EDGE_FALL, false);
}