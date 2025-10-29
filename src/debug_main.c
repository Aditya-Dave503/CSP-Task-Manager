#include "../include/common.h"
#include "../include/ui_controller.h"
#include <stdio.h>

int main() {
    printf("Testing menu display...\n");
    display_main_menu();
    
    printf("\nEnum values:\n");
    printf("MENU_DAEMON_CONTROL = %d\n", MENU_DAEMON_CONTROL);
    printf("MENU_QUIT = %d\n", MENU_QUIT);
    
    return 0;
}
