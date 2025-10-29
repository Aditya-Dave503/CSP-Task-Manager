#include <stdio.h>
#include <stdlib.h>

void display_main_menu() {
    system("clear");
    printf("=== DEBUG TEST ===\n");
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
    printf("Testing basic menu display...\n");
    display_main_menu();
    return 0;
}
