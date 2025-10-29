#include "../include/ipc_shared.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

static int shm_id = -1;
static sem_t *semaphore = NULL;

// Create shared memory segment
int create_shared_memory(void) {
    // Create shared memory segment
    shm_id = shmget(SHM_KEY, sizeof(shared_data_t), IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("shmget");
        return -1;
    }
    
    // Initialize shared memory
    shared_data_t *data = (shared_data_t*)shmat(shm_id, NULL, 0);
    if (data == (void*)-1) {
        perror("shmat");
        return -1;
    }
    
    // Initialize shared data
    memset(data, 0, sizeof(shared_data_t));
    data->daemon_pid = 0;
    data->client_count = 0;
    data->shutdown_requested = 0;
    data->last_update = time(NULL);
    
    shmdt(data);
    return shm_id;
}

// Attach to existing shared memory
int attach_shared_memory(void) {
    shm_id = shmget(SHM_KEY, sizeof(shared_data_t), 0666);
    if (shm_id == -1) {
        return -1;
    }
    return shm_id;
}

void detach_shared_memory(shared_data_t *data) {
    if (data && data != (void*)-1) {
        shmdt(data);
    }
}

void destroy_shared_memory(void) {
    if (shm_id != -1) {
        shmctl(shm_id, IPC_RMID, NULL);
        shm_id = -1;
    }
}

// Semaphore functions
sem_t* create_semaphore(void) {
    // Remove semaphore if it exists
    sem_unlink(SEM_NAME);
    
    // Create new semaphore
    semaphore = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (semaphore == SEM_FAILED) {
        perror("sem_open");
        return NULL;
    }
    return semaphore;
}

sem_t* open_semaphore(void) {
    semaphore = sem_open(SEM_NAME, 0);
    if (semaphore == SEM_FAILED) {
        perror("sem_open");
        return NULL;
    }
    return semaphore;
}

void close_semaphore(sem_t *sem) {
    if (sem) {
        sem_close(sem);
    }
}

void lock_semaphore(sem_t *sem) {
    if (sem) {
        sem_wait(sem);
    }
}

void unlock_semaphore(sem_t *sem) {
    if (sem) {
        sem_post(sem);
    }
}
