# Grow32

Programa que usa um ESP32 para controlar os dispositivos de uma estufa, como Led, Rega, Wavemaker etc..
O programa permite que o usuario agende horarios para ligar e desligar os dispositos, usando reles ligados ao ESP32.

## Instalação

1. Abra o sketch na Arduino IDE.
2. Conecte sua placa ESP32 ao computador.
3. Selecione a placa e porta correta.
4. Clique em **Upload**.

## Arquivos

- `grow32.ino`: Arquivo principal do projeto
- `ntpclock.h` / `ntpclock.cpp`: Código do relogio usando modo NTP
- `relay.h` / `relay.cpp`: Código que controla os reles
- `scheduler.h` / `scheduler.cpp`: Contem a logica de agendamento
- `webinterface.h` / `webinterface.cpp`: Codigo de setup e loop da interface
- `web_html.h` / `web_html.cpp`: HTML, JS, CSS
- `web_routes.h` / `web_routes.cpp`: Web routes

## Dependências

Nenhum por enquanto

## Licença

Sem licença
