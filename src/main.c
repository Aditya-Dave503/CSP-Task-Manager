#include "../include/common.h"
#include "../include/process_list.h"
#include "../include/ui_controller.h"
#include "../include/utils.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

// Simple daemon simulation
static pid_t daemon_pid = 0;

int is_daemon_running(void) {
    return (daemon_pid != 0);
}

void stop_daemon(void) {
    if (is_daemon_running()) {
        printf("Stopping daemon (PID: %d)...\n", daemon_pid);
        daemon_pid = 0;
        printf("Daemon stopped.\n");
    } else {
        printf("No daemon is running.\n");
    }
}

void start_daemon(void) {
    if (is_daemon_running()) {
        printf("Daemon is already running\n");
        return;
    }
    
    printf("Starting daemon...\n");
    daemon_pid = 12345; // Simulated PID
    printf("Daemon started (simulated)\n");
}

void handle_daemon_control() {
    printf("\n=== Daemon Control ===\n");
    printf("1. Start Daemon\n");
    printf("2. Stop Daemon\n");
    printf("3. Check Status\n");
    printf("4. Back to Main Menu\n");
    printf("Choice: ");
    
    int choice = get_user_choice();
    
    switch (choice) {
        case 1:
            start_daemon();
            break;
        case 2:
            stop_daemon();
            break;
        case 3:
            if (is_daemon_running()) {
                printf("Daemon is running\n");
            } else {
                printf("Daemon is not running\n");
            }
            break;
        case 4:
            return;
        default:
            printf("Invalid choice\n");
    }
    
    printf("Press Enter to continue...");
    getchar();
}

void handle_list_processes() {
    printf("DEBUG: Entered handle_list_processes\n");
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

void handle_system_stats() {
    printf("DEBUG: Entered handle_system_stats\n");
    system_stats_t stats = {0};
    
    // Get real system statistics
    FILE *fp;
    char line[256];
    
    // Get memory usage
    fp = fopen("/proc/meminfo", "r");
    if (fp) {
        long mem_total = 0, mem_free = 0;
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, "MemTotal:")) sscanf(line, "MemTotal: %ld kB", &mem_total);
            if (strstr(line, "MemFree:")) sscanf(line, "MemFree: %ld kB", &mem_free);
        }
        fclose(fp);
        
        if (mem_total > 0) {
            stats.memory_usage = ((double)(mem_total - mem_free) / mem_total) * 100.0;
        }
    }
    
    // Get load average
    fp = fopen("/proc/loadavg", "r");
    if (fp) {
        fgets(line, sizeof(line), fp);
        sscanf(line, "%lf %lf %lf", &stats.load_avg[0], &stats.load_avg[1], &stats.load_avg[2]);
        fclose(fp);
    }
    
    // Get uptime
    fp = fopen("/proc/uptime", "r");
    if (fp) {
        double uptime_seconds;
        fscanf(fp, "%lf", &uptime_seconds);
        stats.uptime = (long)uptime_seconds;
        fclose(fp);
    }
    
    display_system_stats(&stats);
}

void handle_live_monitor() {
    printf("DEBUG: Entered handle_live_monitor\n");
    printf("Live monitor would start here\n");
    printf("Press Enter to continue...");
    getchar();
}

void handle_kill_process() {
    printf("DEBUG: Entered handle_kill_process\n");
    printf("Kill process functionality would be here\n");
    printf("Press Enter to continue...");
    getchar();
}

int main() {
    printf("DEBUG: Program starting\n");
    
    int running = 1;
    
    printf("=== System Monitor (Phase 4 - Working) ===\n");
    printf("Daemon: Not running\n");
    printf("\n");
    
    while (running) {
        printf("DEBUG: About to display main menu\n");
        display_main_menu();
        printf("DEBUG: Menu displayed, getting user choice\n");
        
        int choice = get_user_choice();
        printf("DEBUG: User choice = %d\n", choice);
        
        switch (choice) {
            case 1: // LIST_PROCESSES
                printf("DEBUG: Calling handle_list_processes\n");
                handle_list_processes();
                break;
            case 2: // KILL_PROCESS
                printf("DEBUG: Calling handle_kill_process\n");
                handle_kill_process();
                break;
            case 3: // SYSTEM_STATS
                printf("DEBUG: Calling handle_system_stats\n");
                handle_system_stats();
                break;
            case 4: // LIVE_MONITOR
                printf("DEBUG: Calling handle_live_monitor\n");
                handle_live_monitor();
                break;
            case 5: // DAEMON_CONTROL
                printf("DEBUG: Calling handle_daemon_control\n");
                handle_daemon_control();
                break;
            case 6: // QUIT
                printf("DEBUG: Quitting\n");
                running = 0;
                printf("Goodbye!\n");
                break;
            default:
                printf("DEBUG: Invalid choice\n");
                printf("Invalid choice! Press Enter to continue...");
                getchar();
                break;
        }
        
        printf("DEBUG: End of loop iteration\n");
    }
    
    printf("DEBUG: Program exiting\n");
    return 0;
}
