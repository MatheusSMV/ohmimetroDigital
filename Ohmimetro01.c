#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h" //Controle do display
#include "lib/font.h"    //As fontes do display
#include <string.h>
#include "cores.h" //Biblbioteca para o controle da matriz de LEDs

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define ADC_PIN 28       // GPIO para o voltímetro
#define Botao_A 5        // GPIO para botão A
#define MATRIZ_LED_PIN 7 // Pino GPIO 7 conectado à matriz de LEDs WS2812

int R_conhecido = 10000;   // Resistor de 10k ohm
float R_x = 0.0;           // Resistor desconhecido
float ADC_VREF = 3.31;     // Tensão de referência do ADC
int ADC_RESOLUTION = 4095; // Resolução do ADC (12 bits)

// Valores baseados na tabela E 24 - Resistores de 5%
int resistores[] = {
    510, 560, 620, 680, 750, 820, 910, 1000, 1100, 1200, 1300, 1500, 1600, 1800, 2000,
    2200, 2400, 2700, 3000, 3300, 3600, 3900, 4300, 4700, 5100, 5600, 6200, 6800, 7500,
    8200, 9100, 10000, 11000, 12000, 13000, 15000, 16000, 18000, 20000, 22000, 24000,
    27000, 30000, 33000, 36000, 39000, 43000, 47000, 51000, 56000, 62000, 68000, 75000,
    82000, 91000, 100000};

// Cores padrão para os dígitos e multiplicadores
// 0 - Preto; 1 - Marrom; 2 - Vermelho; 3 - Laranja; 4 - Amarelo; 5 - Verde; 6 - Aazul; 7 - Roxo; 8 - Cinza; 9 - Branco.
const char *codigo_cores[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
const char *cores[] = {"Preto", "Marrom", "Vermelho", "Laranja", "Amarelo", "Verde", "Azul", "Roxo", "Cinza", "Branco"};
// 1000 - 1k, 10000 - 10k, 100000 - 100k
const float multiplicadores_final[] = {1, 10, 100, 1000, 10000, 100000, 1000000};
// 1 - preto, 10 - marrom, 100 - vermelho, 1000 - laranja; 10000 - amarelo; 100000 - verde; 1000000 - azul
const char *codigo_multiplicadores[] = {"1", "10", "100", "1000", "10000", "100000", "1000000"};
const char *cores_multiplicadores[] = {"Preto", "Marrom", "Vermelho", "Laranja", "Amarelo", "Verde", "Azul"};

// Função para encontrar o valor comercial mais próximo
int encontrar_resistor_proximo(float valor, int *resistores, int n) // Recebe a variável resistores
{
    for (int i = 0; i < n; i++) // Percorre os valores da tabela resistores
    {
        float limite_inferior = resistores[i] * 0.95;             // -5% isto é o limite inferior
        float limite_superior = resistores[i] * 1.05;             // +5% isto é o limite superior
        if (valor >= limite_inferior && valor <= limite_superior) // Verifica se esta no limite
            return i;                                             // Retorna o array, isto é os valor encontrado
    }
    return -1; // Valor nâo encontrado
}

// Função para obter código de cores - Recebe os parâmetros da faixa, e a variável valor, da função anterior
void obter_codigo_cores(int valor, char *faixa1, char *faixa2, char *faixa3, char *cor1, char *cor2, char *cor_multi)
{
    int n = valor;         // Valor do resistor.
    int multiplicador = 0; // Variável para o cálculo da potência de 10.

    while (n >= 100)
    {
        n /= 10; // Realiza a divisão por 10, a cada divisâo faz multiplicador ++, parando nos dois digitos.
        multiplicador++;
    }

    int d1 = n / 10; // Obtém o primeiro digito, primeira faixa.
    int d2 = n % 10; // Obtém o segundo digito, segunda faixa.

    // Recebe o array corresponde a faixa 1, Faixa 2 e multiplicador.
    strcpy(faixa1, codigo_cores[d1]);
    strcpy(cor1, cores[d1]);
    strcpy(cor2, cores[d2]);
    strcpy(faixa2, codigo_cores[d2]);
    strcpy(faixa3, codigo_multiplicadores[multiplicador]);
    strcpy(cor_multi, cores_multiplicadores[multiplicador]);
}

// Função para converter nome da cor para RGB

// Código Para reset - Retirar ao Enviar
#include "pico/bootrom.h"
#define botaoB 6
void gpio_irq_handler(uint gpio, uint32_t events)
{
    reset_usb_boot(0, 0);
}

int main()
{
    // Inicializa comunicação serial padrão (UART via USB) para printf
    stdio_init_all();

    // Inicializa a matriz de LEDs WS2812 conectada ao pino definido
    npInit(MATRIZ_LED_PIN);
    // Para ser utilizado o modo BOOTSEL com botão B

    gpio_init(botaoB);
    gpio_set_dir(botaoB, GPIO_IN);
    gpio_pull_up(botaoB);
    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);                    // Set the GPIO pin function to I2C
    gpio_pull_up(I2C_SDA);                                        // Pull up the data line
    gpio_pull_up(I2C_SCL);                                        // Pull up the clock line
    ssd1306_t ssd;                                                // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd);                                         // Configura o display
    ssd1306_send_data(&ssd);                                      // Envia os dados para o display

    // Limpa o display. O display inicia com todos os pixels apagados.
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    adc_init();
    adc_gpio_init(ADC_PIN); // GPIO 28 como entrada analógica

    float tensao;
    char str_x[5]; // Buffer para armazenar a string
    char str_y[5]; // Buffer para armazenar a string

    char faixa1[10], faixa2[10], faixa3[10], cor1[10], cor2[10], cor_multi[10], str_cores[40];

    bool cor = true;
    while (true)
    {
        adc_select_input(2); // Seleciona o ADC para eixo X. O pino 28 como entrada analógica

        // Média das medidas.
        float soma = 0.0f;
        for (int i = 0; i < 500; i++)
        {
            soma += adc_read();
            sleep_ms(1);
        }
        float media = soma / 500.0f;

        // Fórmula simplificada: R_x = R_conhecido * ADC_encontrado /(ADC_RESOLUTION - adc_encontrado)
        R_x = (R_conhecido * media) / (ADC_RESOLUTION - media);

        sprintf(str_x, "%1.0f", media); // Converte o inteiro em string
        sprintf(str_y, "%1.0f", R_x);   // Converte o float em string

        int indice = encontrar_resistor_proximo(R_x, resistores, sizeof(resistores) / sizeof(resistores[0]));
        if (indice != -1)
        {
            int valor_resistor = resistores[indice];

            obter_codigo_cores(valor_resistor, faixa1, faixa2, faixa3, cor1, cor2, cor_multi); // Obtém os arrays.
            exibir_faixas_por_linha(cor1, cor2, cor_multi);                                    // Acende os LEDs conforme as faixas
        }

        // Após obter os valores das faixas

        // Atualiza o conteúdo do display com animações
        ssd1306_fill(&ssd, !cor);                          // Limpa o display
        ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);      // Desenha um retângulo
        ssd1306_draw_string(&ssd, "Faixas:  E24-5", 5, 5); // Desenha uma string
        ssd1306_line(&ssd, 3, 13, 123, 13, cor);           // Desenha uma linha
        ssd1306_draw_string(&ssd, "1: -", 5, 15);          // Desenha uma string
        ssd1306_line(&ssd, 3, 23, 123, 23, cor);           // Desenha uma linha
        ssd1306_draw_string(&ssd, "2: -", 5, 25);          // Desenha uma string
        ssd1306_line(&ssd, 3, 33, 123, 33, cor);           // Desenha uma linha
        ssd1306_draw_string(&ssd, "3:   -", 5, 35);        // Desenha uma string
        ssd1306_draw_string(&ssd, faixa1, 20, 15);         // Faixa 1 (linha 1)
        ssd1306_draw_string(&ssd, cor1, 40, 15);           // Faixa 1 (linha 1)
        ssd1306_draw_string(&ssd, faixa2, 20, 25);         // Faixa 2 (linha 2)
        ssd1306_draw_string(&ssd, cor2, 40, 25);           // Faixa 1 (linha 1)
        ssd1306_draw_string(&ssd, faixa3, 20, 35);         // Faixa 3 (linha 3)
        ssd1306_draw_string(&ssd, cor_multi, 55, 35);      // Faixa 1 (linha 1)
        ssd1306_line(&ssd, 3, 43, 123, 43, cor);           // Desenha uma linha
        ssd1306_draw_string(&ssd, "ADC", 13, 45);          // Desenha uma string
        ssd1306_draw_string(&ssd, "Resisten.", 50, 45);    // Desenha uma string
        ssd1306_line(&ssd, 44, 48, 44, 60, cor);           // Desenha uma linha vertical
        ssd1306_draw_string(&ssd, str_x, 8, 54);           // Desenha uma string
        ssd1306_draw_string(&ssd, str_y, 59, 54);          // Desenha uma string
        ssd1306_send_data(&ssd);                           // Atualiza o display
        sleep_ms(700);
    }
}
