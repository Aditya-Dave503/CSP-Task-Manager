#include "../include/ui_controller.h"
#include <stdio.h>
#include <stdlib.h>

void clear_screen() {
    system("clear");
}

void display_main_menu() {
    clear_screen();
    printf("=== System Monitor (Phase 4 - IPC) ===\n");
    printf("1. List All Processes\n");
    printf("2. Kill Process\n");
    printf("3. System Statistics\n");
    printf("4. Live Monitor\n");
    printf("5. Daemon Control\n");
    printf("6. Quit\n");
    printf("======================\n");
    printf("Choice: ");
    fflush(stdout);
}

int get_user_choice() {
    int choice;
    char input[100];
    
    if (fgets(input, sizeof(input), stdin) != NULL) {
        if (sscanf(input, "%d", &choice) == 1) {
            return choice;
        }
    }
    return -1;
}

void format_uptime(long seconds, char *buffer, size_t size) {
    long days = seconds / (24 * 3600);
    long hours = (seconds % (24 * 3600)) / 3600;
    long minutes = (seconds % 3600) / 60;
    long secs = seconds % 60;
    
    if (days > 0) {
        snprintf(buffer, size, "%ld days, %02ld:%02ld:%02ld", days, hours, minutes, secs);
    } else {
        snprintf(buffer, size, "%02ld:%02ld:%02ld", hours, minutes, secs);
    }
}

void display_system_stats(system_stats_t *stats) {
    clear_screen();
    printf("=== System Statistics ===\n\n");
    
    printf("CPU Usage:    %6.2f%%\n", stats->cpu_usage);
    printf("Memory Usage: %6.2f%%\n", stats->memory_usage);
    printf("Load Average: %5.2f, %5.2f, %5.2f\n", 
           stats->load_avg[0], stats->load_avg[1], stats->load_avg[2]);
    
    char uptime_str[64];
    format_uptime(stats->uptime, uptime_str, sizeof(uptime_str));
    printf("Uptime:       %s\n", uptime_str);
    
    printf("\nPress Enter to continue...");
    getchar();
}

void display_live_monitor(process_info_t *processes, int count, system_stats_t *stats) {
    clear_screen();
    
    printf("=== Live System Monitor ===\n");
    printf("CPU: %5.2f%% | Memory: %5.2f%% | Processes: %d\n\n",
           stats->cpu_usage, stats->memory_usage, count);
    
    printf("Live monitor display would be here\n");
    printf("\nPress 'q' to quit\n");
}

void display_processes_table(process_info_t *processes, int count) {
    printf("\n");
    printf("%-8s %-20s %-6s %-8s %-8s\n", 
           "PID", "NAME", "STATE", "CPU%", "MEM%");
    printf("-------- -------------------- ------ -------- --------\n");
    
    int display_count = (count < 20) ? count : 20;
    
    for (int i = 0; i < display_count; i++) {
        char state_char;
        switch (processes[i].state) {
            case PROCESS_RUNNING: state_char = 'R'; break;
            case PROCESS_SLEEPING: state_char = 'S'; break;
            case PROCESS_ZOMBIE: state_char = 'Z'; break;
            case PROCESS_STOPPED: state_char = 'T'; break;
            default: state_char = '?';
        }
        
        printf("%-8d %-20.20s %-6c %-8.2f %-8.2f\n",
               processes[i].pid,
               processes[i].name,
               state_char,
               processes[i].cpu_usage,
               processes[i].memory_usage);
    }
    printf("\nShowing %d of %d processes\n", display_count, count);
    printf("Press Enter to continue...");
    getchar();
}
