#include "../include/process_list.h"
#include <dirent.h>
#include <string.h>

// Global variables to store previous CPU times for calculation
static unsigned long prev_total_time[MAX_PROCESSES] = {0};
static pid_t prev_pids[MAX_PROCESSES] = {0};

static int parse_process_stat(pid_t pid, process_info_t *process) {
    char path[256];
    char line[1024];
    FILE *fp;
    
    snprintf(path, sizeof(path), "/proc/%d/stat", pid);
    fp = fopen(path, "r");
    if (!fp) return -1;
    
    if (fgets(line, sizeof(line), fp)) {
        char *token = strtok(line, " ");
        int field = 1;
        
        while (token != NULL) {
            switch (field) {
                case 1: process->pid = atoi(token); break;
                case 2: 
                    strncpy(process->name, token, MAX_NAME_LEN - 1);
                    if (process->name[0] == '(') {
                        memmove(process->name, process->name + 1, strlen(process->name));
                        if (process->name[strlen(process->name) - 1] == ')') {
                            process->name[strlen(process->name) - 1] = '\0';
                        }
                    }
                    break;
                case 3: process->state = (process_state_t)token[0]; break;
                case 14: process->utime = strtoul(token, NULL, 10); break;
                case 15: process->stime = strtoul(token, NULL, 10); break;
                case 24: process->rss = strtol(token, NULL, 10); break;
            }
            token = strtok(NULL, " ");
            field++;
        }
    }
    
    fclose(fp);
    return 0;
}

static double calculate_memory_usage(process_info_t *process) {
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0) return 0.0;
    
    FILE *meminfo = fopen("/proc/meminfo", "r");
    if (!meminfo) return 0.0;
    
    char line[256];
    long total_memory = 0;
    
    while (fgets(line, sizeof(line), meminfo)) {
        if (strstr(line, "MemTotal:")) {
            sscanf(line, "MemTotal: %ld kB", &total_memory);
            break;
        }
    }
    fclose(meminfo);
    
    if (total_memory <= 0) return 0.0;
    
    double memory_kb = (process->rss * page_size) / 1024.0;
    return (memory_kb / total_memory) * 100.0;
}

// NEW: Calculate CPU usage for all processes
void calculate_cpu_usage(process_info_t *processes, int count, 
                         cpu_stats_t *prev_cpu, cpu_stats_t *curr_cpu) {
    // First, read current system CPU stats
    if (read_system_cpu_stats(curr_cpu) != 0) {
        return; // Failed to read CPU stats
    }
    
    // Calculate total system CPU time between samples
    unsigned long prev_total_cpu = prev_cpu->user + prev_cpu->nice + prev_cpu->system + 
                                  prev_cpu->idle + prev_cpu->iowait + prev_cpu->irq + prev_cpu->softirq;
    unsigned long curr_total_cpu = curr_cpu->user + curr_cpu->nice + curr_cpu->system + 
                                  curr_cpu->idle + curr_cpu->iowait + curr_cpu->irq + curr_cpu->softirq;
    
    unsigned long total_cpu_diff = curr_total_cpu - prev_total_cpu;
    
    if (total_cpu_diff == 0) {
        // No time elapsed, can't calculate CPU usage
        for (int i = 0; i < count; i++) {
            processes[i].cpu_usage = 0.0;
        }
        return;
    }
    
    // Calculate CPU usage for each process
    for (int i = 0; i < count; i++) {
        unsigned long current_process_time = processes[i].utime + processes[i].stime;
        
        // Find previous time for this process
        int found = 0;
        unsigned long previous_process_time = 0;
        
        for (int j = 0; j < MAX_PROCESSES; j++) {
            if (prev_pids[j] == processes[i].pid) {
                previous_process_time = prev_total_time[j];
                found = 1;
                break;
            }
        }
        
        if (found) {
            unsigned long process_time_diff = current_process_time - previous_process_time;
            
            // Calculate CPU usage percentage
            // (process CPU time / total system CPU time) * 100
            processes[i].cpu_usage = ((double)process_time_diff / total_cpu_diff) * 100.0;
            
            // Cap at 100% (can happen due to timing issues)
            if (processes[i].cpu_usage > 100.0) {
                processes[i].cpu_usage = 100.0;
            }
        } else {
            processes[i].cpu_usage = 0.0;
        }
        
        // Store current values for next calculation
        prev_pids[i] = processes[i].pid;
        prev_total_time[i] = current_process_time;
    }
}

int get_all_processes(process_info_t *processes) {
    DIR *dir;
    struct dirent *entry;
    int count = 0;
    
    dir = opendir("/proc");
    if (!dir) {
        perror("opendir");
        return -1;
    }
    
    while ((entry = readdir(dir)) != NULL && count < MAX_PROCESSES) {
        if (entry->d_type == DT_DIR) {
            char *endptr;
            long pid = strtol(entry->d_name, &endptr, 10);
            
            if (endptr != entry->d_name && *endptr == '\0' && pid > 0) {
                if (parse_process_stat(pid, &processes[count]) == 0) {
                    processes[count].memory_usage = calculate_memory_usage(&processes[count]);
                    // CPU usage will be calculated separately using calculate_cpu_usage()
                    processes[count].cpu_usage = 0.0;
                    count++;
                }
            }
        }
    }
    
    closedir(dir);
    return count;
}

int kill_process(pid_t pid, int signal) {
    char path[256];
    
    snprintf(path, sizeof(path), "/proc/%d", pid);
    
    if (access(path, F_OK) != 0) {
        printf("Error: Process with PID %d does not exist.\n", pid);
        return -1;
    }
    
    if (kill(pid, signal) == 0) {
        printf("Successfully sent signal %d to process %d\n", signal, pid);
        return 0;
    } else {
        perror("Error killing process");
        return -1;
    }
}
