# Sistema de Medição de Resistência Elétrica Com Indicação Visual Via Display OLED e Matriz de LEDs WS2812

Este projeto utiliza um microcontrolador Raspberry Pi Pico para medir a resistência de um resistor desconhecido. A resistência é calculada utilizando a conversão analógica-digital (ADC) e exibida em um display OLED SSD1306, enquanto as faixas de cores correspondentes ao resistor são indicadas em uma matriz de LEDs WS2812.

## Recursos do Projeto

- **Cálculo da Resistência**: Determina o valor de um resistor desconhecido usando a fórmula baseada no divisor de tensão.
- **Tabela de Resistores Comerciais**: Integra a tabela E24 de resistores com tolerância de 5%.
- **Display OLED SSD1306**: Mostra o valor calculado do resistor e as faixas de cores.
- **Matriz de LEDs**: Representa visualmente as faixas de cores do resistor na matriz WS2812.
- **Função Reset**: Usa um botão adicional para reiniciar o microcontrolador no modo BOOTSEL.
  
## Componentes Necessários

- Raspberry Pi Pico
- Display OLED SSD1306
- Matriz de LEDs WS2812
- Resistor conhecido de 10kΩ
- Botões (para funcionalidades adicionais)
- Jumpers e protoboard

## Configuração de Hardware

### Conexões GPIO:

| Componente            | Pino do Pico | Função                |
|-----------------------|--------------|-----------------------|
| ADC (Resistor)        | GPIO 28      | Entrada Analógica     |
| Botão A               | GPIO 5       | Botão de Ação         |
| Matriz de LEDs WS2812 | GPIO 7       | Controle de LEDs      |
| I2C SDA (OLED)        | GPIO 14      | Linha de Dados I2C    |
| I2C SCL (OLED)        | GPIO 15      | Linha de Relógio I2C  |
| Botão B               | GPIO 6       | Reinicialização (BOOT)|

## Funcionamento

1. **Medição de Resistência**:
   - O ADC captura a tensão no divisor resistivo.
   - A resistência é calculada com base na tensão medida e no resistor conhecido (10kΩ).
   
2. **Identificação de Valores Comerciais**:
   - A resistência calculada é comparada com a tabela E24 para encontrar o valor comercial mais próximo.

3. **Exibição no Display**:
   - O display OLED exibe:
     - O valor do ADC.
     - O valor do resistor calculado.
     - As faixas de cores representando o resistor.

4. **Indicação Visual**:
   - A matriz de LEDs acende com as cores correspondentes às faixas do resistor.

5. **Função de Reset**:
   - Um botão adicional (GPIO 6) permite reiniciar o Pico no modo BOOTSEL.

## Dependências

Este projeto utiliza as seguintes bibliotecas:

- [Pico SDK](https://github.com/raspberrypi/pico-sdk) (para inicialização e configuração do microcontrolador).
- `ssd1306.h` e `font.h` (para controle do display OLED).
- `cores.h` (para controle da matriz de LEDs WS2812).

## Como Usar

1. Clone este repositório e configure o ambiente de desenvolvimento do Pico SDK.
2. Compile e carregue o código no Raspberry Pi Pico.
3. Conecte os componentes conforme a seção de **Configuração de Hardware**.
4. Execute o projeto e veja as leituras no display OLED e as faixas no LED.

## Melhorias Futuras

- Adicionar suporte para mais tolerâncias de resistores (tabelas E96, por exemplo).
- Interface gráfica para configuração dinâmica do resistor conhecido.
- Suporte para múltiplas medições simultâneas. 

---

**Nota**: Certifique-se de verificar as conexões físicas antes de ligar o dispositivo para evitar danos ao hardware.
