#include <stdio.h>
#include "neopixel.c"
#include "microphone_dma.c"
#include "buzzer.c"
#include "display_oled.c"
#include "buttonA.c"
#include "pwm.c"

#define BUTTON_B 6

void display_initial_message(uint8_t* ssd, struct render_area* frame_area) {
    char *text[] = {"   som suspeito  ", " detectado"};
    int y = 30;
    for (uint i = 0; i < 2; i++) {
        ssd1306_draw_string(ssd, 5, y, text[i]);
        y += 8;
    }
    ssd1306_draw_line(ssd, 0, 17, 128, 17, true);
    render_on_display(ssd, frame_area);
    
}

int main() {
  stdio_init_all();   // Inicializa os tipos stdio padrão presentes ligados ao binário
  
    gpio_init(BUTTON_A);
    gpio_init(BUTTON_B);
    //definir como entrada digital, então microcontrolador irá ler o estado lógico
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    //gpio_pull_up=ativa o resistor interno do microcontrolador para o pino 5, conecta o pino à uma alta tensão (High), quando pressiona
    //do ele é conectado ao pino GND que tira toda a tensão do pino(LOW)
    gpio_pull_up(BUTTON_A);
    gpio_pull_up(BUTTON_B);

  // Configuração dos pinos PWM
    gpio_set_function(13, GPIO_FUNC_PWM); // Vermelho
    gpio_set_function(11, GPIO_FUNC_PWM); // Verde
    gpio_set_function(12, GPIO_FUNC_PWM); // Azul

    // Obter slices
    uint slice_red = pwm_gpio_to_slice_num(13);
    uint slice_green = pwm_gpio_to_slice_num(11);
    uint slice_blue = pwm_gpio_to_slice_num(12);

// Obter canais PWM (A ou B) automaticamente
//pode nao funcionar usar "PWM_CHAN_A" pois a pwm possui dois canais(A e B) e os pinos podem ter canais diferentes
    uint channel_red = pwm_gpio_to_channel(13);
    uint channel_green = pwm_gpio_to_channel(11);
    uint channel_blue = pwm_gpio_to_channel(12);

    // Configuração de TOP e divisor (frequência de 1kHz)
    pwm_set_wrap(slice_red, 255);
    pwm_set_wrap(slice_green, 255);
    pwm_set_wrap(slice_blue, 255);
  //divisor de clock de um slice
    pwm_set_clkdiv(slice_red, 64);
    pwm_set_clkdiv(slice_green, 64);
    pwm_set_clkdiv(slice_blue, 64);

    // Habilitar PWM
    pwm_set_enabled(slice_red, true);
    pwm_set_enabled(slice_green, true);
    pwm_set_enabled(slice_blue, true);

  // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_fuxnction(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();

    // Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    // zera o display inteiro
    uint8_t ssd[ssd1306_buffer_length];
    memset(ssd, 0, ssd1306_buffer_length);

    
    
        

  //parte dos leds
  npInit(LED_PIN, LED_COUNT);
  npClear();
  npWrite();

  // Preparação do ADC.
  adc_gpio_init(MIC_PIN);
  adc_init();
  adc_select_input(MIC_CHANNEL);

  adc_fifo_setup(
    true, // Habilitar FIFO
    true, // Habilitar request de dados do DMA
    1, // Threshold para ativar request DMA é 1 leitura do ADC
    false, // Não usar bit de erro
    false // Não fazer downscale das amostras para 8-bits, manter 12-bits.
  );

  adc_set_clkdiv(ADC_CLOCK_DIV);


  // Tomando posse de canal do DMA.
  dma_channel = dma_claim_unused_channel(true);

  // Configurações do DMA.
  dma_cfg = dma_channel_get_default_config(dma_channel);

  channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_16); // Tamanho da transferência é 16-bits (usamos uint16_t para armazenar valores do ADC)
  channel_config_set_read_increment(&dma_cfg, false); // Desabilita incremento do ponteiro de leitura (lemos de um único registrador)
  channel_config_set_write_increment(&dma_cfg, true); // Habilita incremento do ponteiro de escrita (escrevemos em um array/buffer)
  
  channel_config_set_dreq(&dma_cfg, DREQ_ADC); // Usamos a requisição de dados do ADC



    // Inicializar o PWM no pino do buzzer
    pwm_init_buzzer(BUZZER_PIN);
    restart:
    char *text[] = {
            "PROJETO!   ",
            "  Embarcatech   "};

        int y = 0;
        for (uint i = 0; i < count_of(text); i++)
        {
            ssd1306_draw_string(ssd, 5, y, text[i]);
            y += 8;
        }
        render_on_display(ssd, &frame_area);

          ssd1306_draw_line(ssd, 0, 17, 128, 17, true);
          render_on_display(ssd, &frame_area);

         



  while (true) {
    
    if (!gpio_get(BUTTON_A)){
      for (int i = 0; i <= 255; i++) {
            set_pwm_duty(slice_red, channel_red, i + 128);   //o i sozinho Aumenta o vermelho
              set_pwm_duty(slice_green, channel_green, 255 - i); //255-i  Diminui o verde
              set_pwm_duty(slice_blue, channel_blue, i /2  ); //a intensidade i/2 pode fazer a cor ser menos forte q as demais
              sleep_ms(10); // Aguarda para ver o efeito
          }} else if (gpio_get(BUTTON_A)){
            set_pwm_duty(slice_red, channel_red, 0);   //o i sozinho Aumenta o vermelho
              set_pwm_duty(slice_green, channel_green, 0); //255-i  Diminui o verde
              set_pwm_duty(slice_blue, channel_blue, 0  );
          }
          
        // 1. Sample microphone
        sample_mic();
        float avg = mic_power();
        avg = 2.f * abs(ADC_ADJUST(avg));

        // 2. Check sound threshold and update displays
        if (avg > 2.5f || !gpio_get(BUTTON_B)) {
          display_initial_message(ssd, &frame_area);
          show_alert_sequence();
          // Clear display
        memset(ssd, 0, ssd1306_buffer_length);
        
        // Return to "Projeto Embarcatech" message
        char *text[] = {
            "PROJETO!   ",
            "  Embarcatech   "
        };
        
        int y = 0;
        for (uint i = 0; i < count_of(text); i++) {
            ssd1306_draw_string(ssd, 5, y, text[i]);
            y += 8;
        }
        ssd1306_draw_line(ssd, 0, 17, 128, 17, true);
        render_on_display(ssd, &frame_area);
                  
    }
    }
}
