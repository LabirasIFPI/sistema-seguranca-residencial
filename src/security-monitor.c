#include <stdio.h>                     // Biblioteca padrão do C
#include "pico/stdlib.h"               // Biblioteca padrão do SDK do Raspberry Pi Pico W
#include "hardware/timer.h"            // Biblioteca para interrupções de timer
#include "sensor_pir.h"                // Funções para controle do sensor de presença HC-SR501
#include "wifi.h"                      // Funções para conexão Wi-Fi
#include "server_connection.h"         // Funções para comunicação com o servidor
#include "display.h"                   // Funções para controle do display OLED
#include "led.h"                       // Funções para controle do led
#include "button.h"                    // Funções para controle dos botões
#include "buzzer.h"                    // Funções para controle do BUZZER
#include "joystick.h"                  // Funções para controle do JOYSTICK
#include <parson.h>                    // Biblioteca para trabalhar com JSON
#include "lwip/tcp.h"                  // Biblioteca para trabalhar com o TCP

// Credenciais de Wi-Fi
#define WIFI_SSID "jotadev"
#define WIFI_PASS "12345678"

// Definição do servidor local
#define PORT 8050

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
* Desenha no display o status do WIFI e SENSOR e exibe os indicativos para ativar/desativar dispositivos
*/
void update_display_status() {

    // limpa todo o conteúdo atual no display
    display_clear();

    // status do wifi
    const char *connected_msg = wifi_is_connected ? "Wi-Fi: CONECTADO" : "Wi-Fi: DESCONECTADO";
    display_write_text_no_clear(connected_msg, 0, 0, 1, 0);

    // status do sensor
    const char *sensor_msg = sensor_is_active ? "Sensor:ON" : "Sensor:OFF";
    display_write_text_no_clear(sensor_msg, 0, 12, 1, 0);

    // staus do buzzer
    const char *buzzer_msg = buzzer_is_active ? "Buzzer:ON" : "Buzzer:OFF";
    display_write_text_no_clear(buzzer_msg, 68, 12, 1, 0);

    // atual delay
    char delay_msg[20];
    snprintf(delay_msg, 30, "Delay: %d", delay_sensor);
    display_write_text_no_clear(delay_msg, 0, 24, 1, 0);

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
* Função que processa o json que o servidor local recebe e atualiza o estado dos dispositios
*/
void process_json(const char *json_str) {
    // Parse o JSON recebido como string
    JSON_Value *root_value = json_parse_string(json_str);
    if (root_value == NULL) {
        printf("Erro ao parsear JSON\n");
        return;
    }

    // Obtendo o objeto JSON
    JSON_Object *root_object = json_value_get_object(root_value);

    // Obtendo os valores dos campos do JSON
    int sensor = (int)json_object_get_number(root_object, "sensor");
    int buzzer = (int)json_object_get_number(root_object, "buzzer");
    int delay = (int)json_object_get_number(root_object, "delay");

    // Processamento dos valores para ativar/desativar sensor/buzzer e atualizar o delay de leitura
    sensor_is_active = sensor;
    buzzer_is_active = buzzer;
    delay_sensor = delay;

    // ativando ou desativando as interrupções do sensor de acordo com o novo estado
    sensor_is_active? sensor_enable_interrupt() : sensor_disable_interrupt();

    // atualizando o status dos dipositivos no display
    update_display_status();

    // Libera a memória
    json_value_free(root_value);
}

/*
* Função callback que receberá os dados quando a conexão for aceita
*/
err_t recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    pbuf_copy_partial(p, buffer, p->len, 0);
    printf("Recebido JSON: %s\n", buffer);

    process_json(buffer); // Chama a função para processar o JSON

    pbuf_free(p);
    return ERR_OK;
}

/*
* Função callback executada para aceitar conexões
*/
err_t tcp_accept_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    // Definir a função de recebimento para esta nova conexão
    tcp_recv(newpcb, recv_callback);
    return ERR_OK;
}

/*
* Função reposável por iniciar o servidor local
*/
void start_tcp_server() {
    struct tcp_pcb *pcb;
    err_t err;

    // Criar um novo PCB (control block) para o servidor TCP
    pcb = tcp_new();
    if (pcb == NULL) {
        printf("Erro ao criar o PCB TCP.\n");
        return;
    }

    // Vincular o servidor ao endereço e porta desejada
    ip_addr_t ipaddr;
    IP4_ADDR(&ipaddr, 0, 0, 0, 0);  // Ou use IP_ADDR_ANY para todas as interfaces
    err = tcp_bind(pcb, &ipaddr, PORT);
    if (err != ERR_OK) {
        printf("Erro ao vincular ao endereço e porta.\n");
        return;
    }

    // Colocar o servidor para ouvir conexões
    pcb = tcp_listen(pcb);
    if (pcb == NULL) {
        printf("Erro ao colocar o servidor em escuta.\n");
        return;
    }

    // Configurar a função de aceitação das conexões
    tcp_accept(pcb, tcp_accept_callback);
    printf("Servidor TCP iniciado na porta %d.\n", PORT);
    uint8_t *ip = get_ip_address();
    printf("Endereço IP: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
}

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
    joystick_enable_interrupt();
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
    button_disable_interrupt();   // Desativa temporariamente os botões A e B (evita bouncing)
    joystick_disable_interrupt(); // Desativa temporariamnete o botão SW do joystick
    button_is_active = false;

    // evitando conflitos de timers
    cancel_timers();

    // adiciona timer para rativar os botões em 300ms
    add_repeating_timer_ms(300, reenable_button_callback, NULL, &timer3);
}

/*
* Desenha no dislay o valor do delay do sensor quando alterado
*/
void show_delay_value_on_display() {
    display_clear();

    // preparando mensagem para escrever no display
    char message[30];
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
        current_second = delay_sensor;      // reseta o cronômetro para o valor de delay
        button_enable_interrupt();          // ativa as interrupções para botões
        joystick_enable_interrupt();        // ativa as interrupções para o joystick
        button_is_active = true;            // atualiza o valor da variável de controle
        return false;                       // retorna falso para a função não ser chamada novamente
    }

    return true;
}

/*
* Callback usado para alterar o tempo de delay do sensor
*/
bool update_delay_callback() {

    // caso esteja no modo de edição do delay
    if (edit_delay_mode_is_on) {
        sensor_is_active = false;
        sensor_disable_interrupt();
        joystick_enable_interrupt();
    }
    
    // caso não esteja em modo de edição do delay
    if (!edit_delay_mode_is_on) {
        reenable_button_callback();
        update_display_status();
        return false;
    }
    
    // obtendo o valor atual do eixo y do joystick
    uint16_t eixo_y_value = joystick_read_current_value();

    // ajustando delay de acordo com o movimento do joystick
    if (eixo_y_value > 2500 && delay_sensor <15) {
        delay_sensor++;
    }
    else if(eixo_y_value < 1500 && delay_sensor >5) {
        delay_sensor--;
    }

    // atualiza o display com o valor atual do delay
    show_delay_value_on_display();
    return true;
}

/*
 * Trata interrupção do sensor PIR
 */
void handle_sensor_interrupt() {
    if (!sensor_is_active) return;       // Ignora se o sensor não estiver ativo

    sensor_disable_interrupt();          // Desativa temporariamente o sensor
    sensor_is_active = false;
    button_disable_interrupt();          // Desativa temporariamente os botões A e B
    joystick_disable_interrupt();        // Desativa temporariamente o botão do joystick
    button_is_active = false;
    
    led_turn_on();                       // Acende o LED azul
    if (buzzer_is_active) buzzer_beep(); // Toca beep do buzzer, caso esteja ativo

    if (wifi_is_connected) {
        create_alert("DETECTED", "S01"); // Envia alerta ao servidor, caso esteja conectado a rede
    }

    // Evitando conflitos de timers
    cancel_timers();

    // Ativando interrupções para tela de alerta e para resetar os dispositivos após o delay finalizar
    add_repeating_timer_ms(1000, display_presence_detected_callback, NULL, &timer2);
    add_repeating_timer_ms((delay_sensor + 1) * 1000, reset_sensor_callback, NULL, &timer);
}

/*
 * Trata interrupção do botão B
 */
void handle_button_b_interrupt() {
    if (!button_is_active) return; // Ignora se os botões estiverem desativados

    debouncing_buttons(); // Tratando bouncing

    // Alterna o estado do sensor PIR HC-SR501
    sensor_is_active ? sensor_disable_interrupt() : sensor_enable_interrupt();
    sensor_is_active = !sensor_is_active;

    update_display_status(); // Atualiza a tela com o novo estado
}

/*
 * Trata interrupção do botão A
 */
void handle_button_a_interrupt() {
    if (!button_is_active) return; // Ignora se os botões estiverem desativados

    debouncing_buttons(); // Tratando bouncing

    // Alterna o estado do buzzer
    buzzer_is_active = !buzzer_is_active;

    update_display_status(); // Atualiza a tela com o novo estado
}

/*
 * Trata interrupção do botão SW do joystick
 */
void handle_joystick_button_interrupt() {
    // Alternando o estado a cada vez que pressiona o botão do joystick
    edit_delay_mode_is_on = !edit_delay_mode_is_on;

    debouncing_buttons(); // Tratando bouncing

    // Iniciando timer para, a cada 300ms, permitir uma leitura do valor do eixo Y do joystick
    // e atualizar esse valor no display
    cancel_timers();
    add_repeating_timer_ms(300, update_delay_callback, NULL, &timer);
}

/*
* Callback para tratar todas as interrupções geradas pelos GPIOs
*/
void handle_gpio_interrupt(uint gpio_pin, uint32_t event) {

    // sensor detectou presença
    if (gpio_pin == SENSOR_PIN) {
        handle_sensor_interrupt();
        return;
    }

    // botão B foi pressionado
    if (gpio_pin == PIN_BTN_B) {
        handle_button_b_interrupt();
        return;
    }

    // botão A foi pressionado
    if (gpio_pin == PIN_BTN_A) {
        handle_button_a_interrupt();
        return;
    }

    // botão SW do joystick foi pressionado
    if (gpio_pin == BTN_SW) {
        handle_joystick_button_interrupt();
        return;
    }
}

/*
* Função reponsável por inicair os sensores/atuadores e configura as interrupções
*/
void setup() {

    // inicializa sensor PIR HC-SR501
    sensor_pir_init();
    // Inicializa o display OLED 
    display_init();
    // inicializa o led azul
    led_init();
    // inicializa o buzzer
    buzzer_init();
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
    setup();                    // Configura sensores e atuadores
    sleep_ms(2000);             // Tempo para os dispositivos se estabilizarem
    connect_to_wifi();          // Conecta ao Wi-Fi
    start_tcp_server();         // Inicia servidor local
    update_display_status();    // Atualiza display com o estado dos dispositivos

    while (true) {
        sleep_ms(10);           // Pequeno delay para evitar sobrecarga do processador
    }

    return 0;
}