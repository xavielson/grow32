#ifndef RELAY_H
#define RELAY_H

#include <Arduino.h> 

#define NUM_RELAYS 8
#define NUM_PHYSICAL 3

extern const int relayPins[NUM_PHYSICAL];
extern bool relayStates[NUM_RELAYS];

struct RelayConfig {
  String name;
  String type; // "Led", "Rega", "Wavemaker", "Runoff"
};

extern RelayConfig relays[NUM_RELAYS];

void relay_setup();
void relay_set(int idx, bool state);
void relay_toggle(int idx);

#endif
