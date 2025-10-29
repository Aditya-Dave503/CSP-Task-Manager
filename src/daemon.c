#include "../include/daemon.h"
#include "../include/process_list.h"
#include "../include/system_stats.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

static int running = 1;
static shared_data_t *shared_data = NULL;
static sem_t *semaphore = NULL;

// Daemonize the process
int daemonize(void) {
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("fork");
        return -1;
    }
    
    if (pid > 0) {
        // Parent process exits
        exit(0);
    }
    
    // Child process continues
    umask(0);
    
    // Create new session
    if (setsid() < 0) {
        perror("setsid");
        return -1;
    }
    
    // Change working directory to root
    if (chdir("/") < 0) {
        perror("chdir");
        return -1;
    }
    
    // Close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    
    // Reopen stdin, stdout, stderr to /dev/null
    open("/dev/null", O_RDONLY); // stdin
    open("/dev/null", O_WRONLY); // stdout  
    open("/dev/null", O_WRONLY); // stderr
    
    return 0;
}

// Signal handler function
void handle_signal(int sig) {
    switch(sig) {
        case SIGTERM:
        case SIGINT:
            printf("Daemon received shutdown signal\\n");
            running = 0;
            break;
        case SIGHUP:
            printf("Daemon received reload signal\\n");
            reload_configuration(sig);
            break;
    }
}

// Setup signal handlers
void setup_signal_handlers(void) {
    struct sigaction sa;
    
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGHUP, &sa, NULL);
}

// Load configuration (placeholder for now)
int load_configuration(void) {
    printf("Loading configuration...\\n");
    // Will be implemented in Phase 5
    return 0;
}

// Reload configuration
void reload_configuration(int sig) {
    printf("Reloading configuration...\\n");
    load_configuration();
}

// Main daemon loop
void run_daemon(void) {
    printf("Starting monitor daemon (PID: %d)\\n", getpid());
    
    // Setup IPC
    if (create_shared_memory() == -1) {
        die("Failed to create shared memory");
    }
    
    semaphore = create_semaphore();
    if (!semaphore) {
        die("Failed to create semaphore");
    }
    
    // Attach to shared memory
    int shm_id = attach_shared_memory();
    shared_data = (shared_data_t*)shmat(shm_id, NULL, 0);
    if (shared_data == (void*)-1) {
        die("Failed to attach shared memory");
    }
    
    // Initialize shared data
    lock_semaphore(semaphore);
    shared_data->daemon_pid = getpid();
    shared_data->shutdown_requested = 0;
    unlock_semaphore(semaphore);
    
    // Setup signal handlers
    setup_signal_handlers();
    
    // Load configuration
    load_configuration();
    
    // CPU stats for calculation
    cpu_stats_t prev_cpu, curr_cpu;
    read_system_cpu_stats(&prev_cpu);
    
    printf("Daemon started successfully\\n");
    
    // Main daemon loop
    while (running) {
        lock_semaphore(semaphore);
        
        // Check if shutdown was requested
        if (shared_data->shutdown_requested) {
            running = 0;
            unlock_semaphore(semaphore);
            break;
        }
        
        // Update system statistics
        get_system_stats(&shared_data->system_stats);
        
        // Calculate system CPU usage
        read_system_cpu_stats(&curr_cpu);
        shared_data->system_stats.cpu_usage = calculate_system_cpu_usage(&prev_cpu, &curr_cpu);
        prev_cpu = curr_cpu;
        
        // Update process list
        shared_data->process_count = get_all_processes(shared_data->processes);
        if (shared_data->process_count > 0) {
            calculate_cpu_usage(shared_data->processes, shared_data->process_count, 
                              &prev_cpu, &curr_cpu);
        }
        
        // Update timestamp
        shared_data->last_update = time(NULL);
        
        unlock_semaphore(semaphore);
        
        // Sleep until next update
        sleep(REFRESH_INTERVAL);
    }
    
    // Cleanup
    printf("Shutting down daemon...\\n");
    lock_semaphore(semaphore);
    shared_data->daemon_pid = 0;
    unlock_semaphore(semaphore);
    
    detach_shared_memory(shared_data);
    close_semaphore(semaphore);
    destroy_shared_memory();
    
    printf("Daemon stopped\\n");
}

// Stop daemon
void stop_daemon(void) {
    int shm_id = attach_shared_memory();
    if (shm_id == -1) {
        printf("No daemon running\\n");
        return;
    }
    
    shared_data_t *data = (shared_data_t*)shmat(shm_id, NULL, 0);
    if (data == (void*)-1) {
        printf("Failed to attach to shared memory\\n");
        return;
    }
    
    sem_t *sem = open_semaphore();
    if (sem) {
        lock_semaphore(sem);
        
        if (data->daemon_pid > 0) {
            printf("Stopping daemon (PID: %d)\\n", data->daemon_pid);
            kill(data->daemon_pid, SIGTERM);
            data->shutdown_requested = 1;
        } else {
            printf("No daemon running\\n");
        }
        
        unlock_semaphore(sem);
        close_semaphore(sem);
    }
    
    shmdt(data);
}
