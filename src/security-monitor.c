#include <stdio.h>                                      // Biblioteca padrão do C
#include "pico/stdlib.h"                                // Biblioteca padrão do SDK do Raspberry Pi Pico W
#include "hardware/timer.h"                             // Biblioteca para interrupções de timer
#include "utils/wifi/wifi.h"                            // Funções para conexão Wi-Fi
#include "utils/client http/server_connection.h"        // Funções para comunicação com o servidor
#include "utils/display/display.h"                      // Funções para controle do display OLED
#include "utils/buzzer/buzzer.h"                        // Funções para a manipulaçõa do BUZZER

// Credenciais de Wi-Fi
#define WIFI_SSID "MARIA JULIA"
#define WIFI_PASS "medjugorje"

// Pinos GPIO
#define SENSOR_PIN 18          // Pino do sensor de proximidade
#define PIN_BLUE_LED 12        // Pino do LED azul
#define PIN_BTN_B 6            // Pino do botão B
#define PIN_BTN_A 5            // Pino do botão A

// Variáveis globais de controle
bool wifi_is_connected = false;
bool sensor_is_active = false;
bool button_is_active = false;
bool buzzer_is_active = false;
const int delay_sensor = 5;     // Tempo de espera antes de reativar o sensor (em segundos)

// Estruturas para controle dos timers
struct repeating_timer timer;
struct repeating_timer timer2;
struct repeating_timer timer3;

// Função para conectar ao Wi-Fi
void connect_to_wifi() {
    display_clear();
    display_write_text("Conectando em:", 23, 20, 1, 0);
    display_write_text_no_clear(WIFI_SSID, 23, 32, 1, 0);
    display_show();

    if (connect_wifi(WIFI_SSID, WIFI_PASS) == 0) {
        printf("Wi-Fi conectado\n");
        wifi_is_connected = true;
        return;
    }

    // Exibe mensagem de erro no display
    display_clear();
    display_write_text("Falha de conexao!", 0, 0, 1, 1500);
    display_show();
}

// Desativa a interrupção do sensor de proximidade
void disable_sensor_interrupt() {
    gpio_set_irq_enabled(SENSOR_PIN, GPIO_IRQ_EDGE_RISE, false);
    sensor_is_active = false;
}

// Ativa a interrupção do sensor de proximidade
void enable_sensor_interrupt() {
    gpio_set_irq_enabled(SENSOR_PIN, GPIO_IRQ_EDGE_RISE, true);
    sensor_is_active = true;
}

// Ativa a interrupção do botão B
void enable_button_interrupt() {
    gpio_set_irq_enabled(PIN_BTN_B, GPIO_IRQ_EDGE_FALL, true);
    button_is_active = true;
}

// Desativa a interrupção os botões
void disable_button_interrupt() {
    gpio_set_irq_enabled(PIN_BTN_B, GPIO_IRQ_EDGE_FALL, false);
    gpio_set_irq_enabled(PIN_BTN_B, GPIO_IRQ_EDGE_FALL, false);
    button_is_active = false;
}

// Callback para reativar o botão após um tempo (evita bouncing)
bool reenable_button_callback() {
    enable_button_interrupt();
    return false;
}

// Atualiza o status no display OLED
void update_display_status() {

    // limpa todo o conteúdo atual no display
    display_clear();

    // status do wifi
    const char *connected_msg = wifi_is_connected ? "Wi-Fi: CONECTADO" : "Wi-Fi: DESCONECTADO";
    display_write_text_no_clear(connected_msg, 0, 0, 1, 0);

    // status do sensor
    const char *sensor_msg = sensor_is_active ? "Sensor: LIGADO" : "Sensor: DESLIGADO";
    display_write_text_no_clear(sensor_msg, 0, 12, 1, 0);

    // indicação de alteração do modo de operação
    const char *button_a_msg =  buzzer_is_active ? "A: desativar buzzer" : "A: ativar buzzer";
    display_write_text_no_clear(button_a_msg, 0, 44, 1, 0);

    // indicativo de alteração de status do sensor
    const char *button_b_msg = sensor_is_active ? "B: desativar sensor" : "B: ativar sensor";
    display_write_text_no_clear(button_b_msg, 0, 54, 1, 0);

    // desenha todo o conteúdo no display
    display_show();
}

// Callback executado após o tempo de delay_sensor para reativar o sensor
bool reset_sensor_callback() {
    gpio_put(PIN_BLUE_LED, 0);   // Desliga o LED
    buzzer_stop_beep();          // Desliga o beep do buzzer
    enable_sensor_interrupt();   // Reativa o sensor
    update_display_status();
    return false;
}

// Callback para exibir contagem regressiva no display após detecção de presença
bool display_presence_detected_callback() {
    display_clear();
    static int current_second = delay_sensor;

    // escreve mensagem de alerta no display
    display_write_text_no_clear("Presença Detectada!", 9, 0, 1, 0);
    display_write_text_no_clear("Enviando Alerta", 16, 15, 1, 0);

    // escreve cronômetro regressivo no display
    char timer_msg[20];
    snprintf(timer_msg, sizeof(timer_msg), "Voltando em: %d...", --current_second);
    display_write_text_no_clear(timer_msg, 16, 55, 1, 0);
    display_show();

    // se o cronômetro chegou ao final
    if (current_second == 1) {
        current_second = delay_sensor;
        enable_button_interrupt();
        return false;
    }

    return true;
}

// Callback para tratar interrupções dos GPIOs
void handle_gpio_interrupt(uint gpio_pin, uint32_t event) {

    // sensor gerou sinal
    if (gpio_pin == SENSOR_PIN && sensor_is_active) {
        gpio_put(PIN_BLUE_LED, 1);          // Acende o LED azul    
        if (buzzer_is_active) buzzer_beep();// Toca beep do buzzer
        disable_sensor_interrupt();         // Desativa temporariamente o sensor
        disable_button_interrupt();         // Desativa temporariamente o botão

        if (wifi_is_connected) {
            create_alert("DETECTED", "S01");    // Envia alerta ao servidor
        }
        
        // evitando conflitos de timers
        cancel_repeating_timer(&timer);
        cancel_repeating_timer(&timer2);
        cancel_repeating_timer(&timer3);

        add_repeating_timer_ms(1000, display_presence_detected_callback, NULL, &timer2);
        add_repeating_timer_ms(delay_sensor * 1000, reset_sensor_callback, NULL, &timer);
    }

    // botão B foi pressionado
    if (gpio_pin == PIN_BTN_B && button_is_active) {
        disable_button_interrupt();   // Desativa temporariamente o botão (evita bouncing)

        // evitando conflitos de timers
        cancel_repeating_timer(&timer);
        cancel_repeating_timer(&timer2);
        cancel_repeating_timer(&timer3);

        // adiciona timer para rativar os botões em 300ms
        add_repeating_timer_ms(300, reenable_button_callback, NULL, &timer3);

        // alterna o estado do sensor
        sensor_is_active ? disable_sensor_interrupt() : enable_sensor_interrupt();
        update_display_status();
    }

    // botão A foi pressionado
    if (gpio_pin == PIN_BTN_A && button_is_active) {
        disable_button_interrupt();   // Desativa temporariamente o botão (evita boucing)

        // evitando conflitos de timers
        cancel_repeating_timer(&timer);
        cancel_repeating_timer(&timer2);
        cancel_repeating_timer(&timer3);

        // adiciona timer para reativar os botões em 300ms
        add_repeating_timer_ms(300, reenable_button_callback, NULL, &timer3);

        // alterna o estado do buzzer
        buzzer_is_active = !buzzer_is_active;
        update_display_status();
    }
}

// Configuração inicial dos sensores e atuadores
void setup() {

    // inicializa o led azul
    gpio_init(PIN_BLUE_LED);
    gpio_set_dir(PIN_BLUE_LED, GPIO_OUT);

    // inicializa o buzzer
    buzzer_init();

    // inicializa sensor PIR HC-SR501
    gpio_init(SENSOR_PIN);
    gpio_set_dir(SENSOR_PIN, GPIO_IN);
    // sensor_is_active = true;

    // inicializa o botão B
    gpio_init(PIN_BTN_B);
    gpio_set_dir(PIN_BTN_B, GPIO_IN);
    gpio_pull_up(PIN_BTN_B); // Configura pull-up interno
    button_is_active = true;

    // inicializa o botão A
    gpio_init(PIN_BTN_A);
    gpio_set_dir(PIN_BTN_A, GPIO_IN);
    gpio_pull_up(PIN_BTN_A); // Configura pull-up interno

    // configura interrupções para o sensor e botão
    gpio_set_irq_enabled_with_callback(SENSOR_PIN, GPIO_IRQ_EDGE_RISE, true, &handle_gpio_interrupt);
    gpio_set_irq_enabled(PIN_BTN_B, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(PIN_BTN_A, GPIO_IRQ_EDGE_FALL, true);
}

int main() {
    
    stdio_init_all();           // Inicializa a comunicação serial
    display_init();             // Inicializa o display OLED
    sleep_ms(2000);             // Aguarda um tempo antes de iniciar
    connect_to_wifi();          // Conecta ao Wi-Fi
    setup();                    // Configura sensores e atuadores
    sleep_ms(1000);
    update_display_status();

    while (true) {
        sleep_ms(10);           // Pequeno delay para evitar sobrecarga do processador
    }

    return 0;
}