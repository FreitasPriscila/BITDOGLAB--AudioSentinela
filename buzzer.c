#ifndef BUZZER_H
#define BUZZER_H
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define BUZZER_PIN 21

// Configuração da frequência do buzzer (em Hz)
#define BUZZER_FREQUENCY 100

// Definição de uma função para inicializar o PWM no pino do buzzer
void pwm_init_buzzer(uint pin) {
    // Configurar o pino como saída de PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o PWM com frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (BUZZER_FREQUENCY * 4096)); // Divisor de clock
    pwm_init(slice_num, &config, true);

    // Iniciar o PWM no nível baixo
    pwm_set_gpio_level(pin, 0);
}

// Definição de uma função para emitir um beep com duração especificada
void alarm_sound(uint pin, uint duration_ms) {
    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);
   for(int i = 0; i < duration_ms/100; i++) {
        pwm_set_gpio_level(pin, 3000);
        sleep_ms(50);
        pwm_set_gpio_level(pin, 1000);
        sleep_ms(50);
    }
    pwm_set_gpio_level(pin, 0);
}
#endif