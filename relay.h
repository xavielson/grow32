#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h>
//#include "scheduler.h"

/**
 * Número total de relés lógicos no sistema.
 */
#define NUM_RELAYS 8

/**
 * Número de relés físicos conectados ao ESP32.
 */
#define NUM_PHYSICAL 8

/**
 * Pinos de controle dos relés físicos (ordem).
 */
extern const int relayPins[NUM_PHYSICAL];

/**
 * Estado atual de cada relé (true=ligado, false=desligado).
 */
extern bool relayStates[NUM_RELAYS];

/**
 * Flag: cada relé está com agendamento ativo?
 */
extern bool relayHasSchedule[NUM_RELAYS];

/**
 * Flag: modo manual ativado para cada relé.
 */
extern bool relayManual[NUM_RELAYS];

/**
 * Configuração de cada relé.
 * - name: nome identificador.
 * - type: "Led", "Rega", "Wavemaker", "Runoff"
 * - wavemaker_mode: -1 = não é wavemaker, 0+ = modos válidos
 */
struct RelayConfig {
    String name;
    String type;
    int wavemaker_mode = -1;
    //ScheduleEvent schedules[MAX_SCHEDULES];
    int schedule_count;
};

extern RelayConfig relays[NUM_RELAYS];

/**
 * Inicializa os pinos dos relés e define todos como desligados.
 */
void relay_setup();

/**
 * Define o estado do relé (ligado/desligado).
 * @param idx: índice do relé
 * @param state: true=ligado, false=desligado
 */
void relay_set(int idx, bool state);

/**
 * Alterna o estado do relé (toggle) e ativa modo manual.
 * @param idx: índice do relé
 */
void relay_toggle(int idx);

/**
 * Atualiza o valor de relayHasSchedule
 */
void updateRelayHasSchedule();

#endif
