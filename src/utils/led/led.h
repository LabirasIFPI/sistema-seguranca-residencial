#ifndef LED_H
#define LED_H

// inclusão de bibliotecas
#include "pico/stdlib.h"

#define PIN_BLUE_LED 12        // Pino do LED azul

/*
* Função para iniciarlzar o led RGB
*/
void led_init();

/*
* Função para ligar o led com a cor Azul
*/
void led_turn_on();

/*
* Função para desligar o led
*/
void led_turn_off();

#endif