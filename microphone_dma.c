#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/dma.h"
#include "neopixel.c"
#include"buzzer.c"

// Pino e canal do microfone no ADC.
#define MIC_CHANNEL 2
#define MIC_PIN (26 + MIC_CHANNEL)

// Parâmetros e macros do ADC.
#define ADC_CLOCK_DIV 96.f
#define SAMPLES 200 // Número de amostras que serão feitas do ADC.
#define ADC_ADJUST(x) (x * 3.3f / (1 << 12u) - 1.65f) // Ajuste do valor do ADC para Volts.
// Pino e número de LEDs da matriz de LEDs.
#define LED_PIN 7
#define LED_COUNT 25

#define abs(x) ((x < 0) ? (-x) : (x))





// Canal e configurações do DMA
uint dma_channel;
dma_channel_config dma_cfg;


// Buffer de amostras do ADC.
uint16_t adc_buffer[SAMPLES];


void sample_mic();
float mic_power();


// bool limit_volts(){
//       // Realiza uma amostragem do microfone.
//     sample_mic();

//     // Pega a potência média da amostragem do microfone.
//     float avg = mic_power();
//     avg = 2.f * abs(ADC_ADJUST(avg)); // Ajusta para intervalo de 0 a 3.3V. (apenas magnitude, sem sinal)
//     if (avg > 2.5f) {
//               return true;
//           }else{
//             // Se o volume ultrapassar o limite, interrompe a animação e exibe o alerta
//               return false; 
//           }
// }


void exibirMatriz(int matriz[5][5][3], int tempo_ms) {
    
    for (int linha = 0; linha < 5; linha++) {
        for (int coluna = 0; coluna < 5; coluna++) {
            int posicao = getIndex(linha, coluna);
            npSetLED(posicao, matriz[linha][coluna][0], matriz[linha][coluna][1], matriz[linha][coluna][2]);
        }
    }
    npWrite();
    alarm_sound(BUZZER_PIN, 3000); 
    sleep_ms(100);
   
    
    
    
}

// Exibir sequência de alerta
void show_alert_sequence() {   
    int matriz_losangulo[5][5][3] = { 
      {{0, 0, 0}, {0, 0, 0}, {1, 0, 0}, {0, 0, 0}, {0, 0, 0}},    
      {{0, 0, 0},  {1, 0, 0}, {0, 0, 0},  {1, 0, 0}, {0, 0, 0}},   
      {{1, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},  {1, 0, 0}},  
      {{0, 0, 0}, {1, 0, 0}, {0, 0, 0},  {1, 0, 0}, {0, 0, 0}},   
      {{0, 0, 0}, {0, 0, 0},  {1, 0, 0}, {0, 0, 0}, {0, 0, 0}} };
    int matriz_x[5][5][3] = {
      {{1, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {1, 0, 0}},    
      {{0, 0, 0},  {1, 0, 0}, {0, 0, 0},  {1, 0, 0}, {0, 0, 0}},   
      {{0, 0, 0}, {0, 0, 0}, {1, 0, 0}, {0, 0, 0},  {0, 0, 0}},  
      {{0, 0, 0}, {1, 0, 0}, {0, 0, 0},  {1, 0, 0}, {0, 0, 0}},   
      {{1, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}, {1, 0, 0}} };
    int matriz_exclama[5][5][3] = { 
      {{0, 0, 0}, {0, 0, 0}, {1, 0, 0}, {0, 0, 0}, {0, 0, 0}},    
      {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},    
      {{0, 0, 0}, {0, 0, 0}, {1, 0, 0}, {0, 0, 0}, {0, 0, 0}},        
      {{0, 0, 0}, {0, 0, 0}, {1, 0, 0}, {0, 0, 0}, {0, 0, 0}},
      {{0, 0, 0}, {0, 0, 0}, {1, 0, 0}, {0, 0, 0}, {0, 0, 0}} };

    exibirMatriz(matriz_losangulo, 1000);
     // Limpa a matriz de LEDs.
    npClear();
    exibirMatriz(matriz_x, 1000);
     // Limpa a matriz de LEDs.
    npClear();
    exibirMatriz(matriz_exclama, 1000);
     // Limpa a matriz de LEDs.
    npClear();
    npWrite();
}



// Exibir padrão de segurança (sem alerta)
// void show_no_alert() {
//   alert_detected= false;
//     int matriz_pisca_blue[5][5][3] = { 
//       {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}},    
//       {{0, 0, 0},  {0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}},   
//       {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},  
//       {{0, 0, 0}, {0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}},   
//       {{0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}, {0, 0, 0}} };
//     int matriz_pisca_green[5][5][3] = { 
//       {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},    
//       {{0, 1, 0}, {0, 1, 0}, {0, 1, 0},  {0, 1, 0}, {0, 1, 0}},   
//       {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},  {0, 0, 0}},  
//       {{0, 0, 0}, {0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}},   
//       {{0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}, {0, 0, 0}} };
//     int matriz_pisca[5][5][3] = { 
//       {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}},    
//       {{0, 1, 0},  {0, 1, 0}, {0, 1, 0},  {0, 1, 0}, {0, 1, 0}},   
//       {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},  
//       {{0, 0, 0}, {0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}},   
//       {{0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}, {0, 0, 0}} };
//     int matriz_pisca_blue_3 [5][5][3] = {
//       {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},    
//       {{0, 0, 0}, {0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}},   
//       {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}},  
//       {{0, 0, 0}, {0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}},   
//       {{0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}

//     };
//     int matriz_pisca_2 [5][5][3] = {
//       {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}},    
//       {{0, 1, 0}, {0, 1, 0}, {0, 1, 0},  {0, 1, 0}, {0, 1, 0}},   
//       {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}},  
//       {{0, 0, 0}, {0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}},   
//       {{0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}

//     };
//     int matriz_pisca_green_4 [5][5][3] = {
//       {{0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},    
//       {{0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},   
//       {{0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},  
//       {{0, 1, 0}, {0, 1, 0}, {0, 1, 0},  {0, 1, 0}, {0, 1, 0}},   
//       {{0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}

//     };
//     int matriz_pisca_3 [5][5][3] = {
//       {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}},    
//       {{0, 1, 0}, {0, 1, 0}, {0, 1, 0},  {0, 1, 0}, {0, 1, 0}},   
//       {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}},  
//       {{0, 1, 0}, {0, 1, 0}, {0, 1, 0},  {0, 1, 0}, {0, 1, 0}},   
//       {{0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}

//     };
//     int matriz_pisca_blue_5 [5][5][3] = {
//       {{0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},    
//       {{0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},   
//       {{0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},  
//       {{0, 0, 0}, {0, 0, 0},  {0, 0, 0}, {0, 0, 0}, {0, 0, 0}},   
//       {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}}

//     };
//     int matriz_pisca_4[5][5][3] = {
//       {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}},    
//       {{0, 1, 0}, {0, 1, 0}, {0, 1, 0},  {0, 1, 0}, {0, 1, 0}},   
//       {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}},  
//       {{0, 1, 0}, {0, 1, 0}, {0, 1, 0},  {0, 1, 0}, {0, 1, 0}}, 
//       {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}}

//     };
//     int matriz_check[5][5][3] = {
//       {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 1, 0}},    
//       {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 1, 0}, {0, 0, 1}},   
//       {{0, 1, 0}, {0, 0, 1}, {0, 1, 0}, {0, 0, 1}, {0, 0, 1}},  
//       {{0, 0, 1}, {0, 1, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}}, 
//       {{0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}}

//     };

//       exibirMatriz(matriz_pisca_blue, 1000);
//     //      // Realiza uma amostragem do microfone.
//     // sample_mic();

//     // // Pega a potência média da amostragem do microfone.
//     // float avg = mic_power();
//     // avg = 2.f * abs(ADC_ADJUST(avg)); // Ajusta para intervalo de 0 a 3.3V. (apenas magnitude, sem sinal)
//     //   if( avg> 2.5f){
//     //     alert_detected=true;
//     //     show_alert_sequence();
//     //     break;
//     //   }
      
//       exibirMatriz(matriz_pisca_green, 1000);

//       exibirMatriz(matriz_pisca, 1000);
     
//       exibirMatriz(matriz_pisca_blue_3, 1000);
       
//       exibirMatriz(matriz_pisca_2, 1000);
      
//       exibirMatriz(matriz_pisca_green_4, 1000);
    
//       exibirMatriz(matriz_pisca_3, 1000);
     
//       exibirMatriz(matriz_pisca_blue_5, 1000);
     
//       exibirMatriz(matriz_pisca_4, 1000);
        
//       exibirMatriz(matriz_check, 1000);

// }



/**
 * Realiza as leituras do ADC e armazena os valores no buffer.
 */
void sample_mic() {
  adc_fifo_drain(); // Limpa o FIFO do ADC.
  adc_run(false); // Desliga o ADC (se estiver ligado) para configurar o DMA.

  dma_channel_configure(dma_channel, &dma_cfg,
    adc_buffer, // Escreve no buffer.
    &(adc_hw->fifo), // Lê do ADC.
    SAMPLES, // Faz SAMPLES amostras.
    true // Liga o DMA.
  );

  // Liga o ADC e espera acabar a leitura.
  adc_run(true);
  dma_channel_wait_for_finish_blocking(dma_channel);
  
  // Acabou a leitura, desliga o ADC de novo.
  adc_run(false);
}


/**
 * Calcula a potência média das leituras do ADC. (Valor RMS)
 */
float mic_power() {
  float avg = 0.f;

  for (uint i = 0; i < SAMPLES; ++i)
    avg += adc_buffer[i] * adc_buffer[i];
  
  avg /= SAMPLES;
  return sqrt(avg);
}

