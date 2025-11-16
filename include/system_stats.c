#include "../include/system_stats.h"
#include <stdio.h>
#include <string.h>

// Global variables for CPU calculation
static unsigned long last_total_time = 0;
static unsigned long last_idle_time = 0;

double calculate_cpu_usage(void) {
    FILE *file = fopen("/proc/stat", "r");
    if (!file) return 0.0;

    char line[256];
    unsigned long user, nice, system, idle, iowait, irq, softirq;
    
    if (fgets(line, sizeof(line), file)) {
        sscanf(line, "cpu %lu %lu %lu %lu %lu %lu %lu",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq);
    }
    fclose(file);

    unsigned long total_time = user + nice + system + idle + iowait + irq + softirq;
    unsigned long idle_time = idle + iowait;

    if (last_total_time == 0) {
        // First call - initialize
        last_total_time = total_time;
        last_idle_time = idle_time;
        return 0.0;
    }

    unsigned long total_delta = total_time - last_total_time;
    unsigned long idle_delta = idle_time - last_idle_time;

    last_total_time = total_time;
    last_idle_time = idle_time;

    if (total_delta == 0) return 0.0;

    double usage = 100.0 * (1.0 - ((double)idle_delta / total_delta));
    return usage;
}

int get_memory_usage(system_stats_t *stats) {
    FILE *file = fopen("/proc/meminfo", "r");
    if (!file) return -1;

    char line[256];
    long mem_total = 0, mem_free = 0, mem_available = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "MemTotal:")) {
            sscanf(line, "MemTotal: %ld kB", &mem_total);
        } else if (strstr(line, "MemFree:")) {
            sscanf(line, "MemFree: %ld kB", &mem_free);
        } else if (strstr(line, "MemAvailable:")) {
            sscanf(line, "MemAvailable: %ld kB", &mem_available);
        }
    }
    fclose(file);

    if (mem_total > 0) {
        stats->total_memory = mem_total;
        stats->free_memory = mem_available;
        stats->memory_usage = 100.0 * (1.0 - ((double)mem_available / mem_total));
    }

    return 0;
}

int get_load_average(system_stats_t *stats) {
    FILE *file = fopen("/proc/loadavg", "r");
    if (!file) return -1;

    fscanf(file, "%lf %lf %lf", 
           &stats->load_avg[0], 
           &stats->load_avg[1], 
           &stats->load_avg[2]);
    fclose(file);
    return 0;
}

int get_uptime(system_stats_t *stats) {
    FILE *file = fopen("/proc/uptime", "r");
    if (!file) return -1;

    double uptime_seconds;
    fscanf(file, "%lf", &uptime_seconds);
    fclose(file);

    stats->uptime = (long)uptime_seconds;
    return 0;
}

int get_system_stats(system_stats_t *stats) {
    stats->total_cpu_usage = calculate_cpu_usage();
    get_memory_usage(stats);
    get_load_average(stats);
    get_uptime(stats);
    return 0;
}

void calculate_process_cpu_usage(process_info_t *processes, int count) {
    static int first_run = 1;
    static process_info_t last_processes[MAX_PROCESSES];
    static int last_count = 0;

    if (first_run) {
        // Initialize on first run
        for (int i = 0; i < count; i++) {
            last_processes[i] = processes[i];
        }
        last_count = count;
        first_run = 0;
        return;
    }

    // Calculate CPU usage for each process
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < last_count; j++) {
            if (processes[i].pid == last_processes[j].pid) {
                unsigned long utime_delta = processes[i].utime - last_processes[j].utime;
                unsigned long stime_delta = processes[i].stime - last_processes[j].stime;
                unsigned long total_delta = utime_delta + stime_delta;

                // Assume 100ms between samples (should be configurable)
                double time_interval = 0.1; // seconds
                double cpu_usage = (total_delta / time_interval) / 100.0; // Convert to percentage

                if (cpu_usage < 0) cpu_usage = 0;
                if (cpu_usage > 100) cpu_usage = 100;

                processes[i].cpu_usage = cpu_usage;
                break;
            }
        }
    }

    // Update last processes
    for (int i = 0; i < count; i++) {
        last_processes[i] = processes[i];
    }
    last_count = count;
}
