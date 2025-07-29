#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "relay.h"

/**
 * Máximo de eventos de agendamento por relé.
 */
#define MAX_EVENTS 300

/**
 * Enum para os dias da semana.
 * 0 = Todos, 1 = Domingo, ... 7 = Sábado
 */
enum DayOfWeek {
    ALL_DAYS = 0,
    SUNDAY,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
};

/**
 * Representa um evento de agendamento para um relé.
 */
struct ScheduleEvent {
    int dayOfWeek;  // 0=Todos, 1=Dom ... 7=Sab
    int h_on, m_on, s_on;
    int h_off, m_off, s_off;
};

extern ScheduleEvent schedules[NUM_RELAYS][MAX_EVENTS];
extern int scheduleCounts[NUM_RELAYS];

/**
 * Inicializa o scheduler. Deve ser chamado no setup().
 */
void scheduler_setup();

/**
 * Executa o loop do scheduler. Deve ser chamado no loop().
 */
void scheduler_loop();

/**
 * Processa todos os agendamentos e atualiza os relés conforme necessário.
 */
void processSchedules();

/**
 * Adiciona um evento de agendamento para um relé.
 * Retorna true se o evento foi adicionado com sucesso.
 */
bool addScheduleEvent(int relayIdx, const ScheduleEvent& event);

#endif
