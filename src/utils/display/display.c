#include "display.h"

void display_clear() {
    ssd1306_clear(&display);
}

void display_write_text(const char *text, uint32_t x_axis, uint32_t y_axis, uint32_t scale, uint32_t await_timer_ms) {
    display_clear();
    ssd1306_draw_string(&display, x_axis, y_axis, scale, text);
    ssd1306_show(&display);
    sleep_ms(await_timer_ms);
}


void display_write_text_no_clear(const char *text, uint32_t x_axis, uint32_t y_axis, uint32_t scale, uint32_t await_timer_ms) {
    ssd1306_draw_string(&display, x_axis, y_axis, scale, text);
    sleep_ms(await_timer_ms);
}

void display_show() {
    ssd1306_show(&display);
}

void display_init() {
    // Inicializa I2C no canal 1
    i2c_init(i2c1, 400 * 1000); // 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Inicializa o display
    if (!ssd1306_init(&display, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_ADDRESS, i2c1)) { 
        printf("Falha ao inicializar o display SSD1306\n");
        return;
    }

    printf("Display SSD1306 inicializado com sucesso!\n");
}