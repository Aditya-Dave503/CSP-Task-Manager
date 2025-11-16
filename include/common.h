#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#define MAX_PROCESSES 1024
#define MAX_NAME_LEN 256
#define REFRESH_INTERVAL 2

typedef enum {
    PROCESS_RUNNING = 'R',
    PROCESS_SLEEPING = 'S', 
    PROCESS_ZOMBIE = 'Z',
    PROCESS_STOPPED = 'T'
} process_state_t;

typedef struct {
    pid_t pid;
    char name[MAX_NAME_LEN];
    process_state_t state;
    double cpu_usage;
    double memory_usage;
    unsigned long utime;
    unsigned long stime;
    long rss;
} process_info_t;

typedef struct {
    double total_cpu_usage;
    double memory_usage;
    double load_avg[3];
    long uptime;
    long total_memory;
    long free_memory;
} system_stats_t;

typedef enum {
    MENU_LIST_PROCESSES = 1,
    MENU_KILL_PROCESS,
    MENU_SYSTEM_STATS,
    MENU_LIVE_MONITOR,
    MENU_REFRESH,
    MENU_QUIT
} menu_option_t;

void die(const char *message);

#endif
