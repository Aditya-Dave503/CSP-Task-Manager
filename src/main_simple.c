#include "../include/common.h"
#include "../include/process_list.h"
#include "../include/ui_controller.h"
#include "../include/utils.h"
#include <stdlib.h>
#include <unistd.h>

// Simplified versions of daemon functions that just return dummy values
int is_daemon_running(void) {
    return 0; // Always return "not running" for now
}

void stop_daemon(void) {
    printf("Daemon stop functionality would be here\\n");
}

void handle_daemon_control() {
    printf("\\n=== Daemon Control ===\\n");
    printf("1. Start Daemon\\n");
    printf("2. Stop Daemon\\n");
    printf("3. Check Status\\n");
    printf("4. Back to Main Menu\\n");
    printf("Choice: ");
    
    int choice = get_user_choice();
    
    switch (choice) {
        case 1:
            printf("Starting daemon... (not implemented yet)\\n");
            break;
        case 2:
            stop_daemon();
            break;
        case 3:
            if (is_daemon_running()) {
                printf("Daemon is running\\n");
            } else {
                printf("Daemon is not running\\n");
            }
            break;
        case 4:
            return;
        default:
            printf("Invalid choice\\n");
    }
    
    printf("Press Enter to continue...");
    getchar();
}

// Keep the existing handle functions but simplify them
void handle_list_processes() {
    process_info_t *processes = malloc(MAX_PROCESSES * sizeof(process_info_t));
    if (!processes) {
        die("Memory allocation failed");
    }
    
    int count = get_all_processes(processes);
    if (count > 0) {
        display_processes_table(processes, count);
    } else {
        printf("No processes found. Press Enter to continue...");
        getchar();
    }
    
    free(processes);
}

void handle_system_stats() {
    system_stats_t stats;
    // Simple dummy stats for now
    stats.cpu_usage = 25.5;
    stats.memory_usage = 47.8;
    stats.load_avg[0] = 0.85;
    stats.load_avg[1] = 0.45;
    stats.load_avg[2] = 0.25;
    stats.uptime = 123456;
    
    display_system_stats(&stats);
}

void handle_live_monitor() {
    printf("Live monitor would start here\\n");
    printf("Press Enter to continue...");
    getchar();
}

void handle_kill_process() {
    printf("Kill process functionality would be here\\n");
    printf("Press Enter to continue...");
    getchar();
}

int main() {
    int running = 1;
    
    printf("=== System Monitor (Phase 4 - Simplified) ===\\n");
    printf("Daemon: Not running (using direct mode)\\n");
    printf("\\n");
    
    while (running) {
        display_main_menu();
        int choice = get_user_choice();
        
        switch (choice) {
            case MENU_LIST_PROCESSES:
                handle_list_processes();
                break;
            case MENU_KILL_PROCESS:
                handle_kill_process();
                break;
            case MENU_SYSTEM_STATS:
                handle_system_stats();
                break;
            case MENU_LIVE_MONITOR:
                handle_live_monitor();
                break;
            case MENU_DAEMON_CONTROL:
                handle_daemon_control();
                break;
            case MENU_QUIT:
                running = 0;
                printf("Goodbye!\\n");
                break;
            default:
                printf("Invalid choice! Press Enter to continue...");
                getchar();
                break;
        }
    }
    
    return 0;
}
