#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// Definição dos pinos dos LEDs e do botão
#define LED_BLUE 11
#define LED_RED 12
#define LED_GREEN 13
#define BUTTON_PIN 5

// Variável de estado para controlar a sequência dos LEDs
volatile int led_state = 0;
volatile bool sequence_running = false; // Impede novas ativações durante a sequência

// Função de callback para desligar os LEDs em sequência
int64_t turn_off_callback(alarm_id_t id, void *user_data) {
    switch (led_state) {
        case 1:
            gpio_put(LED_BLUE, 0);
            led_state++;
            add_alarm_in_ms(3000, turn_off_callback, NULL, false);
            break;
        case 2:
            gpio_put(LED_RED, 0);
            led_state++;
            add_alarm_in_ms(3000, turn_off_callback, NULL, false);
            break;
        case 3:
            gpio_put(LED_GREEN, 0);
            sequence_running = false; // Libera o botão para nova ativação
            break;
    }
    return 0;
}

// Callback do botão
void button_callback(uint gpio, uint32_t events) {
    if (!sequence_running) { // Só inicia se nenhuma sequência estiver rodando
        sequence_running = true;
        led_state = 1;
        gpio_put(LED_BLUE, 1);
        gpio_put(LED_RED, 1);
        gpio_put(LED_GREEN, 1);
        add_alarm_in_ms(3000, turn_off_callback, NULL, false);
    }
}

int main() {
    stdio_init_all();
    
    // Configuração dos LEDs
    gpio_init(LED_BLUE);
    gpio_init(LED_RED);
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    
    // Configuração do botão
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    
    while (true) {
        printf("Sistema aguardando acionamento do botão...\n");
        sleep_ms(1000); // Impressão a cada segundo
    }
}