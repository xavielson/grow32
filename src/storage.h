#pragma once
#include <Arduino.h>
#include "relay.h"      // Para acessar RelayConfig
#include "scheduler.h"  // Para acessar ScheduleEvent, schedules, scheduleCounts

// Inicializa SPIFFS
bool storage_init();

bool storage_load(RelayConfig relays[], int numRelays);

bool storage_save_all(RelayConfig relays[], size_t num_relays);
bool storage_load_all(RelayConfig relays[], size_t num_relays);

bool format_storage();
