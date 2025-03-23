# Sistema Inteligente de Monitoramento de Movimento

## Visão Geral
Este é um sistema de detecção de movimento baseado na placa **BitDogLab** (RP2040 com Wi-Fi). Ele monitora áreas específicas e aciona **alarmes locais**, além de enviar **alertas remotos** via **bot do Telegram**.

## Funcionalidades
- **Detecção de movimento**: Utiliza o sensor **PIR HC-SR501**.
- **Alarmes locais**: Indica detecção de movimento com **LED RGB e buzzer**.
- **Notificação remota**: Envio de alertas via **bot do Telegram**.
- **Interface local**: Controle por **joystick** e **botões físicos**.
- **Exibição no display**: Informações sobre status do sistema no **OLED 128x64**.

## Hardware Utilizado
- **Placa BitDogLab** (RP2040 com Wi-Fi)
- **Sensor PIR HC-SR501** (Detecção de movimento)
- **Joystick** (Configuração do delay e interação com o sistema)
- **2 Botões** (Ativar/Desativar sensor e buzzer)
- **LED RGB** (Indica status do sistema)
- **Buzzer** (Alarme sonoro)
- **Display OLED 128x64** (Exibição de informações)

## Software Utilizado
- **Firmware (RP2040)**: Desenvolvido em **C** com **Pico SDK**
- **Backend**: API em **Java (Spring Boot)** para gerenciar e enviar alertas
- **Banco de Dados**: Postgresql (armazenamento de eventos de movimento)
- **Comunicação**: Protocolo HTTP e API REST

## Como Funciona
1. O **sensor PIR HC-SR501** detecta movimento.
2. O **RP2040 processa os dados** e exibe no **display OLED**.
3. Se um movimento for detectado:
   - O **LED RGB** muda de cor para indicar alerta.
   - O **buzzer** emite um som de alerta (se ativado).
   - Um **alerta é enviado** via bot do **Telegram**.
4. O usuário pode ajustar o tempo de delay e ativar/desativar os alarmes usando o **joystick e os botões físicos**.

## Instalação e Configuração
### Firmware (RP2040)
1. Clone este repositório:
   ```sh
   git clone https://github.com/victordev018/sistema-de-seguranca
   cd sistema-de-seguranca/
   ```
2. Compile com o **Pico SDK**:
   ```sh
   mkdir build && cd build
   cmake ..
   make
   ```
3. Envie o firmware para o **RP2040**:
   ```sh
   cp security-monitor.uf2 /media/pi/RPI-RP2
   ```

### Backend (API em Java)
Para mais informações sobre a API, consulte-a em https://github.com/victordev018/security-system-api

## Uso
- **Monitoramento local:** O status do sensor e da conexão Wi-Fi é exibido no display OLED.
- **Configuração:** Use o **joystick** para ajustar o tempo de delay entre leituras.
- **Ativação/Desativação:** Os **botões físicos** permitem ligar ou desligar o sensor e o alarme sonoro.
- **Alertas:** Se um movimento for detectado, um **alarme local** é ativado e uma **notificação remota** é enviada.

## Autor
João Victor
