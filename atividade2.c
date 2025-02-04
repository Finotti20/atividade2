#include <stdio.h> // Biblioteca padrão do C.
#include "pico/stdlib.h" // Biblioteca padrão do Raspberry Pi Pico para controle de GPIO, temporização e comunicação serial.
#include "pico/time.h"   // Biblioteca para gerenciamento de temporizadores e alarmes.

const uint LED_PIN_GREEN = 11;    // Define o pino GPIO 11 para controlar o LED verde.
const uint LED_PIN_BLUE = 12;     // Define o pino GPIO 12 para controlar o LED azul.
const uint LED_PIN_RED = 13;      // Define o pino GPIO 13 para controlar o LED vermelho.
const uint BUTTON_PIN = 5;        // Define o pino GPIO 5 para ler o estado do botão.

bool led_active = false;    // Indica se o LED está atualmente aceso (para evitar múltiplas ativações).

// Função de callback para desligar o LED vermelho.
int64_t turn_off_red_callback(alarm_id_t id, void *user_data) {
    gpio_put(LED_PIN_RED, false); // Desliga o LED vermelho.
    led_active = false; // Indica que o ciclo terminou.
    return 0;
}

// Função de callback para desligar o LED azul e ligar o vermelho.
int64_t turn_off_blue_callback(alarm_id_t id, void *user_data) {
    gpio_put(LED_PIN_BLUE, false); // Desliga o LED azul.
    gpio_put(LED_PIN_RED, true);   // Liga o LED vermelho.
    
    // Agenda o LED vermelho para apagar após mais 3 segundos.
    add_alarm_in_ms(3000, turn_off_red_callback, NULL, false);
    return 0;
}

// Função de callback para desligar o LED verde.
int64_t turn_off_green_callback(alarm_id_t id, void *user_data) {
    gpio_put(LED_PIN_GREEN, false); // Desliga o LED verde.
    
    // Mantém o LED azul aceso e agenda o desligamento após mais 3 segundos.
    add_alarm_in_ms(3000, turn_off_blue_callback, NULL, false);
    return 0;
}

int main() {
    stdio_init_all(); // Inicializa a comunicação serial.

    // Configura os pinos dos LEDs como saída.
    gpio_init(LED_PIN_GREEN);
    gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);
    gpio_put(LED_PIN_GREEN, false);

    gpio_init(LED_PIN_BLUE);
    gpio_set_dir(LED_PIN_BLUE, GPIO_OUT);
    gpio_put(LED_PIN_BLUE, false);

    gpio_init(LED_PIN_RED);
    gpio_set_dir(LED_PIN_RED, GPIO_OUT);
    gpio_put(LED_PIN_RED, false);

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    while (true) {
        if (gpio_get(BUTTON_PIN) == 0 && !led_active) {
            sleep_ms(50); // Debounce
            if (gpio_get(BUTTON_PIN) == 0) {
                gpio_put(LED_PIN_GREEN, true); // Acende o LED verde.
                gpio_put(LED_PIN_BLUE, true);  // Acende o LED azul simultaneamente.
                gpio_put(LED_PIN_RED, true);   // Acende o LED vermelho simultaneamente.
                led_active = true; // Marca que o ciclo iniciou.
                
                // Agenda o LED verde para apagar após 3 segundos.
                add_alarm_in_ms(3000, turn_off_green_callback, NULL, false);
            }
        }
        sleep_ms(10); // Pequena pausa para otimizar uso da CPU.
    }
    return 0;
}
