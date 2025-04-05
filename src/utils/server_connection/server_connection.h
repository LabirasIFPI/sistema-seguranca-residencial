#ifndef SERVER_CONNECTION_H
#define SERVER_CONNECTION_H

// inclusões de bibliotecas
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include "lwip/ip_addr.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/api.h"

// definições de PROXY
#define PROXY_PORT 57140
#define SERVER_IP "35.212.33.83"

/*
* Função callback para tratar da resposta do servidor
*/
err_t tcp_client_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);

/*
* Função que connecta ao servudor e envia um post de alerta
*/
void send_alert_to_server(const char *path, char *request_body, const char *type_method);

/*
* Função que cria um alerta para poder ser enviado ao servidor
*/
void create_alert(char *status, char *sensor_identifier);

#endif