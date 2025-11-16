#ifndef PROCESS_LIST_H
#define PROCESS_LIST_H

#include "common.h"

int get_all_processes(process_info_t *processes);
int kill_process(pid_t pid, int signal);

#endif
