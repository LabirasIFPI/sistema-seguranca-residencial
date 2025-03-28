#ifndef DISPLAY_H
#define DISPLAY_H

// inclusões de bibliotecas
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "ssd1306.h"
#include "hardware/i2c.h"

// configurações do display
#define SCREEN_WIDTH 128    
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C // Endereço I2C do display
#define I2C_SDA 14          // Pino SDA
#define I2C_SCL 15          // Pino SCL

//Instância do Display
static ssd1306_t display;

/*
* Função reponsável por limpar o conteúdo do display
*/
void display_clear();

/*
* Função reponsável por escrever um texto no display apagando o que tinha antes
*/
void display_write_text(const char *text, uint32_t x_axis, uint32_t y_axis, uint32_t scale, uint32_t await_timer_ms);


/*
* Função reponsável por escrever um texto no display sem apagar o que tinha nele
*/
void display_write_text_no_clear(const char *text, uint32_t x_axis, uint32_t y_axis, uint32_t scale, uint32_t await_timer_ms);

/*
* Função reponsável por exibir o conteúdo no display
*/
void display_show();

/*
* Função reponsável por iniciar o display
*/
void display_init();

#endif