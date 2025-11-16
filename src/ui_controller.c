#include "../include/ui_controller.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/select.h>
#include <unistd.h>

void clear_screen() {
    printf("\033[H\033[J");
}

void display_main_menu() {
    clear_screen();
    printf("=== System Monitor (Phase 3) ===\n");
    printf("1. List All Processes\n");
    printf("2. Kill Process\n");
    printf("3. System Statistics\n");
    printf("4. Live Monitor\n");
    printf("5. Refresh\n");
    printf("6. Quit\n");
    printf("=================================\n");
    printf("Choice: ");
}

void display_system_stats(system_stats_t *stats) {
    clear_screen();
    printf("=== System Statistics ===\n\n");
    
    long days = stats->uptime / (24 * 3600);
    long hours = (stats->uptime % (24 * 3600)) / 3600;
    long minutes = (stats->uptime % 3600) / 60;
    long seconds = stats->uptime % 60;
    
    printf("Uptime:          %ld days, %02ld:%02ld:%02ld\n", days, hours, minutes, seconds);
    printf("CPU Usage:       %.2f%%\n", stats->total_cpu_usage);
    printf("Memory Usage:    %.2f%%\n", stats->memory_usage);
    printf("Total Memory:    %ld MB\n", stats->total_memory / 1024);
    printf("Free Memory:     %ld MB\n", stats->free_memory / 1024);
    printf("Load Average:    %.2f, %.2f, %.2f\n", 
           stats->load_avg[0], stats->load_avg[1], stats->load_avg[2]);
    
    printf("\nPress Enter to continue...");
    getchar();
}

void display_live_monitor(process_info_t *processes, int count, system_stats_t *stats) {
    int running = 1;
    time_t last_update = 0;
    
    printf("Starting Live Monitor... (Press 'q' to quit)\n");
    sleep(1);
    
    while (running) {
        time_t current_time = time(NULL);
        
        if (current_time - last_update >= REFRESH_INTERVAL) {
            clear_screen();
            printf("=== Live System Monitor === (Press 'q' to quit)\n\n");
            
            get_system_stats(stats);
            
            printf("System Overview:\n");
            printf("  CPU: %.2f%% | Memory: %.2f%% | Load: %.2f, %.2f, %.2f\n\n",
                   stats->total_cpu_usage, stats->memory_usage,
                   stats->load_avg[0], stats->load_avg[1], stats->load_avg[2]);
            
            printf("Top Processes:\n");
            printf("%-8s %-20s %-6s %-8s %-8s\n", 
                   "PID", "NAME", "STATE", "CPU%", "MEM%");
            printf("-------- -------------------- ------ -------- --------\n");
            
            int display_count = (count < 10) ? count : 10;
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
            
            printf("\nRefreshing every %d seconds...", REFRESH_INTERVAL);
            fflush(stdout);
            
            last_update = current_time;
        }
        
        struct timeval tv = {0, 100000};
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        
        if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
            char ch = getchar();
            if (ch == 'q' || ch == 'Q') {
                running = 0;
            }
        }
        
        usleep(100000);
    }
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

void display_processes_table(process_info_t *processes, int count) {
    printf("\n");
    printf("%-8s %-20s %-6s %-8s %-8s\n", 
           "PID", "NAME", "STATE", "CPU%", "MEM%");
    printf("-------- -------------------- ------ -------- --------\n");
    
    int display_count = (count < 50) ? count : 50;
    
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
