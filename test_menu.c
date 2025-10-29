#include <stdio.h>

typedef enum {
    MENU_LIST_PROCESSES = 1,
    MENU_KILL_PROCESS,
    MENU_SYSTEM_STATS,
    MENU_LIVE_MONITOR,
    MENU_DAEMON_CONTROL,
    MENU_QUIT
} menu_option_t;

void display_main_menu() {
    printf("=== System Monitor (Phase 4 - IPC) ===\n");
    printf("1. List All Processes\n");
    printf("2. Kill Process\n");
    printf("3. System Statistics\n");
    printf("4. Live Monitor\n");
    printf("5. Daemon Control\n");
    printf("6. Quit\n");
    printf("======================\n");
    printf("Choice: ");
}

int main() {
    display_main_menu();
    return 0;
}
