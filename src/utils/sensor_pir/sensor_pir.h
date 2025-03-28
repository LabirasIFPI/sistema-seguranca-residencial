#ifndef SENSOR_PIR_H
#define SENSOR_PIR_H

#include "pico/stdlib.h"

// Pino do sensor de proximidade
#define SENSOR_PIN 18

/*
* Função responsável por iniciar o sensor de presença HC-SR501
*/
void sensor_pir_init();

/*
* Função para ativar as interrupções do sensor
*/
void sensor_enable_interrupt();

/*
* Função para desativar as interrupções do sensor
*/
void sensor_disable_interrupt();

#endif