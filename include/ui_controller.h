#ifndef UI_CONTROLLER_H
#define UI_CONTROLLER_H

#include "common.h"

void display_processes_table(process_info_t *processes, int count);
void display_main_menu(void);
int get_user_choice(void);
void clear_screen(void);

// NEW: System statistics display
void display_system_stats(system_stats_t *stats);
void display_live_monitor(process_info_t *processes, int count, system_stats_t *stats);

#endif
