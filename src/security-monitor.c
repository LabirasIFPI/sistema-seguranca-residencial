#include <stdio.h>                                      // Biblioteca padrão do C
#include "pico/stdlib.h"                                // Biblioteca padrão do SDK do Raspberry Pi Pico W
#include "hardware/timer.h"                             // Biblioteca para interrupções de timer
#include "utils/sensor_pir/sensor_pir.h"                // Funções para controle do sensor de presença HC-SR501
#include "utils/wifi/wifi.h"                            // Funções para conexão Wi-Fi
#include "utils/client http/server_connection.h"        // Funções para comunicação com o servidor
#include "utils/display/display.h"                      // Funções para controle do display OLED
#include "utils/led/led.h"                              // Funções para controle do led
#include "utils/buttons/button.h"                       // Funções para controle dos botões
#include "utils/buzzer/buzzer.h"                        // Funções para controle do BUZZER
#include "utils/joystick/joystick.h"                    // Funções para controle do JOYSTICK

// Credenciais de Wi-Fi
#define WIFI_SSID "MARIA JULIA"
#define WIFI_PASS "medjugorje"

// Variáveis globais de controle
bool wifi_is_connected = false;
bool sensor_is_active = false;
bool button_is_active = false;
bool buzzer_is_active = false;
bool edit_delay_mode_is_on = false;
int delay_sensor = 5;               // Tempo de espera antes de reativar o sensor (em segundos)

// Estruturas para controle dos timers
struct repeating_timer timer;
struct repeating_timer timer2;
struct repeating_timer timer3;

/*
* Função para conectar ao Wi-Fi
*/
void connect_to_wifi() {
    display_clear();
    display_write_text("Conectando em:", 23, 20, 1, 0);
    display_write_text_no_clear(WIFI_SSID, 23, 32, 1, 0);
    display_show();

    // caso a conexão se estabeleça com sucesso
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

/*
* Callback para reativar o botão após um tempo (evita bouncing)
*/
bool reenable_button_callback() {
    button_enable_interrupt();
    button_is_active = true;
    return false;
}

/*
* Função responsável por cancelar timers ativos evitando conflitos
*/
void cancel_timers() {
    cancel_repeating_timer(&timer);
    cancel_repeating_timer(&timer2);
    cancel_repeating_timer(&timer3);
}

/*
* Função reponsável por fazer o deboucing nos botões para evitar falsas leituras
*/
void debouncing_buttons() {
    button_disable_interrupt();   // Desativa temporariamente o botão (evita bouncing)
    button_is_active = false;

    // evitando conflitos de timers
    cancel_timers();

    // adiciona timer para rativar os botões em 300ms
    add_repeating_timer_ms(300, reenable_button_callback, NULL, &timer3);
}

/*
* Desenha no display o status do WIFI e SENSOR e exibe os indicativos para ativar/desativar dispositivos
*/
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

/*
* Desenha no dislay o valor do delay do sensor quando alterado
*/
void show_delay_value_on_display() {
    display_clear();

    // preparando mensagem para escrever no display
    const message[30];
    snprintf(message, 30, "delay atual: %d", delay_sensor);
    display_write_text_no_clear(message, 17, 22, 1, 0);

    // informando valor mínimo e máximo
    display_write_text_no_clear("min: 5  max: 15", 16, 54, 1, 0);

    // exibindo as mensagens no display
    display_show();
}

/*
* Callback executado após o tempo de delay_sensor para deslogar dispositivos de alerta e reativar o sensor
*/
bool reset_sensor_callback() {
    led_turn_off();              // Desliga o LED
    buzzer_stop_beep();          // Desliga o beep do buzzer
    sensor_enable_interrupt();   // Reativa o sensor
    sensor_is_active = true;
    update_display_status();     // Coloca no display o a tela padrão de status
    return false;                // Retorna false para não ser chamada novamente   
}

/*
* Callback para exibir contagem regressiva no display após detecção de presença
*/
bool display_presence_detected_callback() {
    display_clear();
    static int current_second = -1;
    
    // garantindo que o cronômetro sempre use o valor atualizado do delay
    static int last_delay_time = 5;
    if (delay_sensor != last_delay_time){
        current_second = delay_sensor;
        last_delay_time = delay_sensor;
    }

    if (current_second == -1) {
        current_second = delay_sensor;
    }

    // escreve mensagem de alerta no display
    display_write_text_no_clear("Presenca Detectada!", 9, 0, 1, 0);
    display_write_text_no_clear("Enviando Alerta", 16, 15, 1, 0);

    // escreve cronômetro regressivo no display
    char timer_msg[20];
    snprintf(timer_msg, sizeof(timer_msg), "Voltando em: %d...", --current_second+1);
    display_write_text_no_clear(timer_msg, 16, 55, 1, 0);
    display_show();

    // se o cronômetro chegou ao final
    if (current_second == 0) {
        current_second = delay_sensor;
        button_enable_interrupt();
        button_is_active = true;
        return false;
    }

    return true;
}

/*
* Callback usado para alterar o tempo de delay do sensor
*/
bool update_delay_callback() {
    
    // obtendo o valor atual do eixo y do joystick
    uint16_t eixo_y_value = joystick_read_current_value();

    // ajustando delay de acordo com o movimento do joystick
    if (eixo_y_value > 2500 && delay_sensor <15) {
        delay_sensor++;
    }
    else if(eixo_y_value < 1500 && delay_sensor >5) {
        delay_sensor--;
    }

    show_delay_value_on_display();
    return true;
}

/*
* Callback para tratar todas as interrupções geradas pelos GPIOs
*/
void handle_gpio_interrupt(uint gpio_pin, uint32_t event) {

    // sensor gerou sinal
    if (gpio_pin == SENSOR_PIN && sensor_is_active) {
        sensor_disable_interrupt();           // Desativa temporariamente o sensor
        sensor_is_active = false;
        button_disable_interrupt();           // Desativa temporariamente o botão
        button_is_active = false;
        led_turn_on();                        // Acende o LED azul    
        if (buzzer_is_active) buzzer_beep();  // Toca beep do buzzer

        if (wifi_is_connected) {
            create_alert("DETECTED", "S01");  // Envia alerta ao servidor
        }
        
        // evitando conflitos de timers
        cancel_timers();

        // ativando interrupções para tela de alerta e para resetar os dispositivos após o delay
        add_repeating_timer_ms(1000, display_presence_detected_callback, NULL, &timer2);
        add_repeating_timer_ms((delay_sensor+1) * 1000, reset_sensor_callback, NULL, &timer);
    }

    // botão B foi pressionado
    if (gpio_pin == PIN_BTN_B && button_is_active) {
        
        // tratando o bouncing
        debouncing_buttons();

        // alterna o estado do sensor PIR HC-SR501
        sensor_is_active ? sensor_disable_interrupt() : sensor_enable_interrupt();
        sensor_is_active = !sensor_is_active;
        update_display_status();
    }

    // botão A foi pressionado
    if (gpio_pin == PIN_BTN_A && button_is_active) {

        // tratando o bouncing
        debouncing_buttons();

        // alterna o estado do buzzer
        buzzer_is_active = !buzzer_is_active;
        update_display_status();
    }

    // botão SW do joystick foi pressionado
    if (gpio_pin == BTN_SW && button_is_active) {
        edit_delay_mode_is_on = !edit_delay_mode_is_on;

        // tratando o bouncing
        debouncing_buttons();

        if (!edit_delay_mode_is_on) {
            update_display_status();
        }

        if (edit_delay_mode_is_on) {
            sensor_disable_interrupt();
            sensor_is_active = false;
            add_repeating_timer_ms(300, update_delay_callback, NULL, &timer);
        }

    }
}

/*
* Função reponsável por inicair os sensores/atuadores e configura as interrupções
*/
void setup() {

    // inicializa o led azul
    led_init();
    // inicializa o buzzer
    buzzer_init();
    // inicializa sensor PIR HC-SR501
    sensor_pir_init();
    // inicializa o botão A e B
    button_init();
    button_is_active = true;
    // inicializando o joystick
    joystick_init();

    // configura interrupções para o sensor e botão
    gpio_set_irq_enabled_with_callback(SENSOR_PIN, GPIO_IRQ_EDGE_RISE, true, &handle_gpio_interrupt);
    gpio_set_irq_enabled(PIN_BTN_B, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(PIN_BTN_A, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(BTN_SW, GPIO_IRQ_EDGE_FALL, true);
}

int main() {
    
    stdio_init_all();           // Inicializa a comunicação serial
    display_init();             // Inicializa o display OLED
    sleep_ms(2000);             // Tempo para display iniciar
    connect_to_wifi();          // Conecta ao Wi-Fi
    setup();                    // Configura sensores e atuadores
    sleep_ms(1000);             // Tempo para os dispositivos se estabilizares
    update_display_status();

    while (true) {
        sleep_ms(10);           // Pequeno delay para evitar sobrecarga do processador
    }

    return 0;
}