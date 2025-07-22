#ifndef NTPCLOCK_H
#define NTPCLOCK_H

#include <time.h>   // <--- Esta linha é obrigatória para definir time_t

void ntp_setup();
time_t ntp_now();

#endif
