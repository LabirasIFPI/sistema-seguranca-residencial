#include "sensor_pir.h"

void sensor_pir_init() {
    // inicializa sensor PIR HC-SR501
    gpio_init(SENSOR_PIN);
    gpio_set_dir(SENSOR_PIN, GPIO_IN);
}

void sensor_enable_interrupt() {
    gpio_set_irq_enabled(SENSOR_PIN, GPIO_IRQ_EDGE_RISE, true);
}

void sensor_disable_interrupt() {
    gpio_set_irq_enabled(SENSOR_PIN, GPIO_IRQ_EDGE_RISE, false);
}