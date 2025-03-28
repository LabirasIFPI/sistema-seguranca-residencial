#include "buzzer.h"

void buzzer_init() {
    
    // configurar o pino como saida pwm
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);

    // obtendo o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);

    // configurar o pwm com a frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096));   // divisor de clock
    pwm_init(slice_num, &config, true);

    // iniciar o pwm no nível baixo
    pwm_set_gpio_level(BUZZER_PIN, 0);
}

void buzzer_beep() {

    // obtendo o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);

    // configurar o duty cycle para 50% (ativo)
    pwm_set_gpio_level(BUZZER_PIN, 4090);
}

void buzzer_stop_beep() {
    // desativar o sinal PWM (duty cycle 0)
    pwm_set_gpio_level(BUZZER_PIN, 0);
}