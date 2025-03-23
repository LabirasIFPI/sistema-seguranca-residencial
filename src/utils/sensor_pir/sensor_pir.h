#include "pico/stdlib.h"

// Pino do sensor de proximidade
#define SENSOR_PIN 18

/*
* Função responsável por iniciar o sensor de presença HC-SR501
*/
void sensor_pir_init() {
    // inicializa sensor PIR HC-SR501
    gpio_init(SENSOR_PIN);
    gpio_set_dir(SENSOR_PIN, GPIO_IN);
}

/*
* Função para ativar as interrupções do sensor
*/
void sensor_enable_interrupt() {
    gpio_set_irq_enabled(SENSOR_PIN, GPIO_IRQ_EDGE_RISE, true);
}

/*
* Função para desativar as interrupções do sensor
*/
void sensor_disable_interrupt() {
    gpio_set_irq_enabled(SENSOR_PIN, GPIO_IRQ_EDGE_RISE, false);
}