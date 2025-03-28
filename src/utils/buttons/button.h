#ifndef BUTTON_H
#define BUTTON_H

#include "pico/stdlib.h"

// pinos dos botões
#define PIN_BTN_B 6            // Pino do botão B
#define PIN_BTN_A 5            // Pino do botão A

/*
* Função responsável por inicializar os botões A e B
*/
void button_init();

/*
* Função para ativar as interrupções dos botões A e B
*/
void button_enable_interrupt();

/*
* Função para desativar as interrupções dos botões A e B
*/
void button_disable_interrupt();

#endif