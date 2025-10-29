#ifndef IPC_SHARED_H
#define IPC_SHARED_H

#include "common.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#define SHM_KEY 0x1234
#define SEM_NAME "/task_manager_sem"
#define MAX_CLIENTS 10

// Structure for shared memory segment
typedef struct {
    system_stats_t system_stats;
    process_info_t processes[MAX_PROCESSES];
    int process_count;
    int daemon_pid;
    int client_count;
    time_t last_update;
    int shutdown_requested;
} shared_data_t;

// IPC functions
int create_shared_memory(void);
int attach_shared_memory(void);
void detach_shared_memory(shared_data_t *data);
void destroy_shared_memory(void);

// Semaphore functions
sem_t* create_semaphore(void);
sem_t* open_semaphore(void);
void close_semaphore(sem_t *sem);
void lock_semaphore(sem_t *sem);
void unlock_semaphore(sem_t *sem);

#endif
