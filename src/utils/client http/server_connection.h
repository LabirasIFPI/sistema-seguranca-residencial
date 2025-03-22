#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include "lwip/ip_addr.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/api.h"

// configurações do servidor
#define SERVER_IP "10.0.0.109";
#define SERVER_PORT 8080
int server_ip_address[] = {10, 0, 0, 109};

// callback para tratar da resposta do servidor
err_t tcp_client_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err) {
    
    if (err != ERR_OK || p == NULL) {
        if (p != NULL) pbuf_free(p);
        tcp_close(pcb);
        printf("Conexão fechada\n");
        return ERR_OK;
    }
        
    fwrite(p->payload, 1, p->len, stdout);
    printf("\n");

    pbuf_free(p);

    return ERR_OK; 
}

// connecta ao servudor e envia um post de alerta
void send_alert_to_server(const char *server_ip_s, uint16_t server_port, const char *path, char *request_body, const char *type_method) {
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar PCB\n");
        return;
    }

    ip_addr_t server_ip;
    IP4_ADDR(&server_ip, server_ip_address[0], server_ip_address[1], server_ip_address[2], server_ip_address[3]); // ip from server

    if (tcp_connect(pcb, &server_ip, server_port, NULL) != ERR_OK) {
        printf("Erro ao conectar ao servidor\n");
        tcp_abort(pcb);
        return;
    }

    printf("Conectado ao Servidor!\n");

    char request[521];
    snprintf(request, sizeof(request),
    "%s %s HTTP/1.1\r\n"
    "Host: %s\r\n"
    "Content-Type: application/json\r\n"
    "Content-Length: %d\r\n"
    "\r\n"  // <- Linha separadora obrigatória antes do corpo da requisição
    "%s"
    "\r\n", // <- Certifique-se de finalizar corretamente
    type_method, path, server_ip_s, strlen(request_body), request_body);
    
    printf("Request: %s\n", request); // Adiciona log para verificar a solicitação

    if (tcp_write(pcb, request, strlen(request), TCP_WRITE_FLAG_COPY) != ERR_OK) {
        printf("Erro ao enviar dados\n");
        tcp_abort(pcb);
        return;
    }

    if (tcp_output(pcb) != ERR_OK) {
        printf("Erro ao enviar dados (tcp_output)\n");
        tcp_abort(pcb);
        return;
    }

    tcp_recv(pcb, tcp_client_recv);
}

// cria um alerta para poder ser enviado ao servidor
void create_alert(char *status, char *sensor_identifier) {
    printf("\nPresenca detectada\nIniciando comunicacao com API...\n");
    const char *typeMethod = "POST";
    const char *server_ip_s = SERVER_IP;
    const char *path = "/alert";
    char json_request[256];
    snprintf(json_request, sizeof(json_request),
    "{\n"
    "\"status\" : \"%s\",\n"
    "\"sensorIdentifier\" : \"%s\"\n"
    "}", status, sensor_identifier);

    send_alert_to_server(server_ip_s, SERVER_PORT, path, json_request, typeMethod);
}