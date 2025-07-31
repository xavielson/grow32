#pragma once
#include <Arduino.h>
#include "relay.h"      // Para acessar RelayConfig
#include "scheduler.h"  // Para acessar ScheduleEvent, schedules, scheduleCounts

// Inicializa SPIFFS
bool storage_init();

// Salva e carrega apenas as configurações dos relés
bool storage_save_relays(RelayConfig relays[], size_t num_relays);
bool storage_load_relays(RelayConfig relays[], size_t num_relays);

// Salva e carrega os agendamentos de todos os relés
bool storage_save_schedules();
bool storage_load_schedules();

bool storage_save_all(RelayConfig relays[], size_t num_relays);
bool storage_load_all(RelayConfig relays[], size_t num_relays);
