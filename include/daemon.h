#ifndef DAEMON_H
#define DAEMON_H

#include "common.h"
#include "ipc_shared.h"

// Daemon functions
int daemonize(void);
void run_daemon(void);
void stop_daemon(void);

// Signal handlers
void setup_signal_handlers(void);
void handle_signal(int sig);

// Configuration
int load_configuration(void);
void reload_configuration(int sig);

#endif
