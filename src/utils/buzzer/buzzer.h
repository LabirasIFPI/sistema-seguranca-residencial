#ifndef BUZZER_H
#define BUZZER_H

// inclusões de bibliotecas
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

// definições de pinagem e frequência do BUZZER
#define BUZZER_PIN 21
#define BUZZER_FREQUENCY 100


/*
* Função para inicializar do PWM no pino do BUZZER
*/
void buzzer_init();

/*
* Função que dispara um beep no BUZZER
*/
void buzzer_beep();

/*
* Função que desliga o beep no BUZZER
*/
void buzzer_stop_beep();

#endif