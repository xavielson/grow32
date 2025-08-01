#ifndef NTPCLOCK_H
#define NTPCLOCK_H

#include <time.h>
#include <RTClib.h>


extern RTC_DS3231 rtc;

void ntp_or_rtc_setup();

/**
 * Inicializa e sincroniza o relógio NTP (bloqueante até sincronizar).
 * Deve ser chamada no setup().
 */
void ntp_setup();

/**
 * Retorna o horário local atual (epoch time).
 * 
 * @return time_t: segundos desde 01/01/1970, já ajustado para o fuso horário configurado.
 */
time_t ntp_now();

#endif
