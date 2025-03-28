#ifndef JOYSTICK_H
#define JOYSTICK_H

// inclusões de bibliotecas
#include <stdio.h>
#include "hardware/adc.h"

// Pinos do Joystick
#define EIXO_Y 26
#define BTN_SW 22

/*
* Função reponsável por inicializar o joystick com o eixo Y e o botão SW
*/
void joystick_init();

/*
* Função reponsável por fazer a leitura do eixo Y
*/
uint16_t joystick_read_current_value();

/*
* Função para ativar as interrupções do botão SW
*/
void joystick_enable_interrupt();

/*
* Função para desativar as interrupções do botão SW
*/
void joystick_disable_interrupt();

#endif