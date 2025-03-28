#ifndef WIFI_H
#define WIFI_H

// inclusão de bibliotecas
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

/*
* Função responsável por inicializar o módulo de wifi e fazer a conexão
*/
int connect_wifi(const char *ssid, const char *password);

/*
* Função reponsável por retornar o IP da placa
*/
uint8_t * get_ip_address();

#endif