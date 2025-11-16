#ifndef SYSTEM_STATS_H
#define SYSTEM_STATS_H

#include "common.h"

int get_system_stats(system_stats_t *stats);
double calculate_cpu_usage(void);
int get_memory_usage(system_stats_t *stats);
int get_load_average(system_stats_t *stats);
int get_uptime(system_stats_t *stats);

#endif
