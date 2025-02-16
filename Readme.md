Raspberry-Pi-Pico-W---BitDogLab
Projetos realizados durante o curso de Sistemas Embarcados - Projeto final baseado na ideia de totens de segurança

-> Um sistema de segurança inteligente que monitora sons ambientes usando um microfone e fornece alertas visuais através de matrizes de LED e um display OLED. O projeto é baseado no microcontrolador Raspberry Pi Pico.

🚀 Funcionalidades

Monitoramento em Tempo Real: Análise contínua do som ambiente utilizando ADC com DMA para amostragem eficiente.

Matriz de LED RGB 5x5: Exibição de padrões de alerta visuais dinâmicos.

Display OLED: Exibição de mensagens de status e avisos.

Indicadores RGB com PWM: Iluminação colorida para diferentes níveis de alerta.

Botões de Controle Manual: Interface para interação direta.

Buzzer para Alertas Sonoros: Sinalização de eventos suspeitos.

Padrões de Alerta Visuais: Exibição de formas como diamante, "X" e ponto de exclamação para alertas diferenciados.

🔧 Implementação Técnica

DMA para Amostragem de Som: Eficiência na captura de dados do microfone.

PWM para Controle de LEDs RGB: Mistura de cores precisa para sinalização.

Comunicação via I2C: Integração com o display OLED SSD1306.

Controle de LED WS2812B via PIO: Gerenciamento otimizado dos LEDs endereçáveis.

Detecção de Som Configurável: Ajuste de limiares para acionamento de alertas.

Arquitetura Modular: Código estruturado com módulos separados para:

Controle da matriz de LED

Amostragem de som

Gerenciamento do display

Entrada por botão

Controle do buzzer

🛠 Componentes Utilizados

Raspberry Pi Pico

Módulo de Microfone

Matriz de LEDs WS2812B 5x5

Display OLED SSD1306

Botão de Pressão

Buzzer

LED RGB

📌 Como Funciona?

O sistema monitora continuamente o nível sonoro ambiente. Quando ruídos suspeitos são detectados acima de um limite predefinido, ele ativa alertas visuais e sonoros, tornando-se uma solução eficaz para monitoramento de segurança.