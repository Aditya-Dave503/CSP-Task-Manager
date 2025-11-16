#include "../include/common.h"
#include "../include/process_list.h"
#include "../include/ui_controller.h"
#include "../include/system_stats.h"
#include "../include/utils.h"
#include <stdlib.h>

void handle_list_processes() {
    process_info_t *processes = malloc(MAX_PROCESSES * sizeof(process_info_t));
    if (!processes) {
        die("Memory allocation failed for processes");
    }
    
    int count = get_all_processes(processes);
    if (count < 0) {
        free(processes);
        printf("Error reading processes. Press Enter to continue...");
        getchar();
        return;
    }
    
    display_processes_table(processes, count);
    free(processes);
}

void handle_kill_process() {
    char input[100];
    pid_t pid;
    int signal = 15;
    
    printf("Enter PID to kill: ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error reading input.\n");
        return;
    }
    
    if (sscanf(input, "%d", &pid) != 1) {
        printf("Invalid PID format.\n");
        return;
    }
    
    printf("Send signal (15=SIGTERM, 9=SIGKILL) [15]: ");
    if (fgets(input, sizeof(input), stdin) != NULL && input[0] != '\n') {
        sscanf(input, "%d", &signal);
    }
    
    if (pid > 0) {
        printf("Attempting to kill process %d with signal %d...\n", pid, signal);
        kill_process(pid, signal);
    } else {
        printf("Invalid PID: %d\n", pid);
    }
    
    printf("Press Enter to continue...");
    getchar();
}

void handle_system_stats() {
    system_stats_t stats;
    if (get_system_stats(&stats) == 0) {
        display_system_stats(&stats);
    } else {
        printf("Error reading system statistics. Press Enter to continue...");
        getchar();
    }
}

void handle_live_monitor() {
    process_info_t *processes = malloc(MAX_PROCESSES * sizeof(process_info_t));
    system_stats_t stats;
    
    if (!processes) {
        die("Memory allocation failed for processes");
    }
    
    int count = get_all_processes(processes);
    if (count < 0) {
        free(processes);
        printf("Error reading processes. Press Enter to continue...");
        getchar();
        return;
    }
    
    display_live_monitor(processes, count, &stats);
    free(processes);
}

int main() {
    int running = 1;
    
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
            case MENU_REFRESH:
                break;
            case MENU_QUIT:
                running = 0;
                printf("Goodbye!\n");
                break;
            default:
                printf("Invalid choice! Press Enter to continue...");
                getchar();
                break;
        }
    }
    
    return 0;
}
