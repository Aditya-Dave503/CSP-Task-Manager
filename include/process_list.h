#ifndef PROCESS_LIST_H
#define PROCESS_LIST_H

#include "common.h"

int get_all_processes(process_info_t *processes);
static int parse_process_stat(pid_t pid, process_info_t *process);
static double calculate_memory_usage(process_info_t *process);
int kill_process(pid_t pid, int signal);

// NEW: CPU calculation functions
void calculate_cpu_usage(process_info_t *processes, int count, 
                         cpu_stats_t *prev_cpu, cpu_stats_t *curr_cpu);
int read_system_cpu_stats(cpu_stats_t *stats);
double calculate_system_cpu_usage(cpu_stats_t *prev, cpu_stats_t *curr);

// NEW: System statistics functions
int get_system_stats(system_stats_t *stats);

#endif
