#include "relay.h"
#include <Arduino.h>
#include "scheduler.h"

/**
 * Pinos dos relés físicos (altere conforme seu hardware).
 */
const int relayPins[NUM_PHYSICAL] = {16, 17, 18, 19, 21, 22, 23, 25};

/**
 * Estados iniciais (todos desligados).
 */
bool relayStates[NUM_RELAYS] = {false};
bool relayManual[NUM_RELAYS] = {false};
bool relayHasSchedule[NUM_RELAYS] = {false};

/**
 * Configuração inicial dos relés.
 * Ajuste nome/tipo conforme necessidade.
 */
RelayConfig relays[NUM_RELAYS] = {
    {"", "", -1, 0},
    {"", "", -1, 0},
    {"", "", -1, 0},
    {"", "", -1, 0},
    {"", "", -1, 0},
    {"", "", -1, 0},
    {"", "", -1, 0},
    {"", "", -1, 0},
};

/**
 * Checa se índice é válido para um relé lógico.
 */
static bool isValidRelayIndex(int idx) {
    return (idx >= 0 && idx < NUM_RELAYS);
}

/**
 * Inicializa os pinos dos relés, todos desligados (HIGH = desligado, LOW = ligado).
 */
void relay_setup() {
    for (int i = 0; i < NUM_PHYSICAL; i++) {
        pinMode(relayPins[i], OUTPUT);
        digitalWrite(relayPins[i], HIGH); // HIGH = desligado (relé normalmente fechado)
    }
}

/**
 * Define o estado do relé. Estado físico: LOW = ligado, HIGH = desligado.
 */
void relay_set(int idx, bool state) {
    if (!isValidRelayIndex(idx)) return;
    relayStates[idx] = state;
    if (idx < NUM_PHYSICAL) {
        digitalWrite(relayPins[idx], state ? LOW : HIGH); // LOW aciona o relé
    }
}

/**
 * Alterna o estado do relé e ativa modo manual.
 */
void relay_toggle(int idx) {
    if (!isValidRelayIndex(idx)) return;
    relayStates[idx] = !relayStates[idx];
    relayManual[idx] = true; // Ativa modo manual ao toggle
    relay_set(idx, relayStates[idx]);
}

/**
 * Atualiza o valor de relayHasSchedue
 */
void updateRelayHasSchedule() {
    for (int i = 0; i < NUM_RELAYS; i++) {
        relayHasSchedule[i] = (scheduleCounts[i] > 0);
    }
}
