#include "wifi.h"

int connect_wifi(const char *ssid, const char *password) {

    // inicializa o chip
    if (cyw43_arch_init()) {
        printf("Wi-fi init failed\n");
        return -1;
    }

    // ativa o módulo de wifi
    cyw43_arch_enable_sta_mode();

    // tentando se conectar
    printf("Connecting Wifi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("Failed to connect.\n");
        return -1;
    }

    // caso a conexão tenha sido realizada com sucesso
    printf("Connected.\n");
    return 0;
}