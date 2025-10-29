#include "../include/common.h"
#include "../include/process_list.h"
#include <stdio.h>
#include <string.h>

// NEW: Read system-wide CPU statistics from /proc/stat
int read_system_cpu_stats(cpu_stats_t *stats) {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) return -1;

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "cpu ", 4) == 0) {
            sscanf(line + 5, "%lu %lu %lu %lu %lu %lu %lu",
                   &stats->user, &stats->nice, &stats->system, 
                   &stats->idle, &stats->iowait, &stats->irq, 
                   &stats->softirq);
            fclose(fp);
            return 0;
        }
    }
    
    fclose(fp);
    return -1;
}

// NEW: Calculate system-wide CPU usage percentage
double calculate_system_cpu_usage(cpu_stats_t *prev, cpu_stats_t *curr) {
    // Calculate total CPU time between two samples
    unsigned long prev_total = prev->user + prev->nice + prev->system + 
                              prev->idle + prev->iowait + prev->irq + prev->softirq;
    unsigned long curr_total = curr->user + curr->nice + curr->system + 
                              curr->idle + curr->iowait + curr->irq + curr->softirq;
    
    // Calculate idle time difference
    unsigned long prev_idle = prev->idle + prev->iowait;
    unsigned long curr_idle = curr->idle + curr->iowait;
    
    // Calculate differences
    unsigned long total_diff = curr_total - prev_total;
    unsigned long idle_diff = curr_idle - prev_idle;
    
    if (total_diff == 0) return 0.0;
    
    // CPU usage = (total - idle) / total
    return ((double)(total_diff - idle_diff) / total_diff) * 100.0;
}

// NEW: Get comprehensive system statistics
int get_system_stats(system_stats_t *stats) {
    FILE *fp;
    char line[256];
    
    // Get CPU statistics (we'll calculate usage separately)
    stats->cpu_usage = 0.0;  // Will be calculated in main loop
    
    // Get memory usage from /proc/meminfo
    fp = fopen("/proc/meminfo", "r");
    if (fp) {
        long mem_total = 0, mem_free = 0, buffers = 0, cached = 0;
        
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, "MemTotal:")) sscanf(line, "MemTotal: %ld kB", &mem_total);
            if (strstr(line, "MemFree:")) sscanf(line, "MemFree: %ld kB", &mem_free);
            if (strstr(line, "Buffers:")) sscanf(line, "Buffers: %ld kB", &buffers);
            if (strstr(line, "Cached:")) sscanf(line, "Cached: %ld kB", &cached);
        }
        fclose(fp);
        
        if (mem_total > 0) {
            long used_memory = mem_total - mem_free - buffers - cached;
            stats->memory_usage = ((double)used_memory / mem_total) * 100.0;
        }
    }
    
    // Get load average from /proc/loadavg
    fp = fopen("/proc/loadavg", "r");
    if (fp) {
        fgets(line, sizeof(line), fp);
        sscanf(line, "%lf %lf %lf", &stats->load_avg[0], &stats->load_avg[1], &stats->load_avg[2]);
        fclose(fp);
    }
    
    // Get uptime from /proc/uptime
    fp = fopen("/proc/uptime", "r");
    if (fp) {
        double uptime_seconds;
        fscanf(fp, "%lf", &uptime_seconds);
        stats->uptime = (long)uptime_seconds;
        fclose(fp);
    }
    
    return 0;
}
