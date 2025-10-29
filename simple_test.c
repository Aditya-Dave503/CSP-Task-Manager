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

int main() {
    printf("Testing simple menu...\n");
    sleep(1);
    display_main_menu();
    
    int choice;
    scanf("%d", &choice);
    printf("You chose: %d\n", choice);
    
    return 0;
}
