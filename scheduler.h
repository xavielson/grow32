#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "relay.h"

#define MAX_EVENTS 300

struct ScheduleEvent {
  int dayOfWeek; // 0=Todos, 1=Dom ... 7=Sab
  int h_on, m_on, s_on;
  int h_off, m_off, s_off;
};

extern ScheduleEvent schedules[NUM_RELAYS][MAX_EVENTS];
extern int scheduleCounts[NUM_RELAYS];

void scheduler_setup();
void scheduler_loop();
void processSchedules();

#endif
