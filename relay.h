#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h> 

#define NUM_RELAYS 8
#define NUM_PHYSICAL 8


extern const int relayPins[NUM_PHYSICAL];
extern bool relayStates[NUM_RELAYS];
extern bool relayHasSchedule[NUM_RELAYS];
// === ADIÇÃO: flag de modo manual para cada relé ===
extern bool relayManual[NUM_RELAYS];

struct RelayConfig {
  String name;
  String type; // "Led", "Rega", "Wavemaker", "Runoff"
  int wavemaker_mode = -1; // -1 = não é wavemaker, 0 em diante = modos possíveis
};


extern RelayConfig relays[NUM_RELAYS];

void relay_setup();
void relay_set(int idx, bool state);
void relay_toggle(int idx);

#endif
