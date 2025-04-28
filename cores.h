#include <matrizled.c>
#include "pico/stdlib.h"
#include <time.h>
// AQUI ESTA O MAPEAMENTO DE TODOS OS NUMEROS DE 0 A 9
// por padrao foi definido a exibicao dos nuemros na cor VERMELHA

#define intensidade 1

void print()
{
    npWrite();
    // sleep_ms(2000);
    npClear();
}

// COnverter RBG Para Hexa, melhor forma de representação.
uint32_t cor_para_rgb(const char *cor_nome)
{ // Recebe o char cor nome
    if (strcmp(cor_nome, "Preto") == 0)
        return 0x000000; // Apagado
    else if (strcmp(cor_nome, "Marrom") == 0)
        return 0x4B3800;
    else if (strcmp(cor_nome, "Vermelho") == 0)
        return 0xFF0000;
    else if (strcmp(cor_nome, "Laranja") == 0)
        return 0x473800;
    else if (strcmp(cor_nome, "Amarelo") == 0)
        return 0xFFFF00;
    else if (strcmp(cor_nome, "Verde") == 0)
        return 0x00FF00;
    else if (strcmp(cor_nome, "Azul") == 0)
        return 0x0000FF;
    else if (strcmp(cor_nome, "Roxo") == 0)
        return 0xC800C8;
    else if (strcmp(cor_nome, "Cinza") == 0)
        return 0x808080;
    else if (strcmp(cor_nome, "Branco") == 0)
        return 0xFFFFFF;
    else
        return 0x000000; // Se nâo reconhecer nâo pinta nada.
}

void exibir_faixas_por_linha(const char *cor1, const char *cor2, const char *cor_multi)
{              // Recebe as strings char que armazena as cores
    npClear(); // Limpa a matriz

    // Converte cores para RGB
    uint32_t rgb1 = cor_para_rgb(cor1);
    uint32_t rgb2 = cor_para_rgb(cor2);
    uint32_t rgb_multi = cor_para_rgb(cor_multi);

    // Linha 1 (y=0): Faixa 1 Percorrer a linha de 0 4, isto é a primeira linha
    for (int x = 0; x < 5; x++)
    {
        int pos = getIndex(x, 0); // Mapeia (x, y=0) para o índice do LED
        npSetLED(pos, (rgb1 >> 16) & 0xFF, (rgb1 >> 8) & 0xFF, rgb1 & 0xFF);
    }

    // Linha 3 (y=2): Faixa 2 Percorrer a linha de 10 a 14, isto é a terceira linha
    for (int x = 0; x < 5; x++)
    {
        int pos = getIndex(x, 2); // Mapeia (x, y=2) para o índice do LED
        npSetLED(pos, (rgb2 >> 16) & 0xFF, (rgb2 >> 8) & 0xFF, rgb2 & 0xFF);
    }

    // Linha 5 (y=4): Faixa 3 Percorrer a linha de 20 a 24, isto é a quinta linha
    for (int x = 0; x < 5; x++)
    {
        int pos = getIndex(x, 4); // Mapeia (x, y=4) para o índice do LED
        npSetLED(pos, (rgb_multi >> 16) & 0xFF, (rgb_multi >> 8) & 0xFF, rgb_multi & 0xFF);
    }

    npWrite(); // Atualiza a matriz
}