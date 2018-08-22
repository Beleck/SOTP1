#ifndef APP_SIGNAL_H
#define APP_SIGNAL_H
#include <semaphore.h>
#include <stddef.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "app_shm.h"

#define NEW_SIGNAL(signal, name, handler, flags) \
    do {                                         \
        struct sigaction name;                   \
        name.sa_handler = handler;               \
        sigemptyset(&name.sa_mask);              \
        name.sa_flags = flags;                   \
        sigaction(signal, &name, NULL);          \
    } while (0)

int viewer_signal_received;
int viewer_shm_fd;
char *view_mmap;
sem_t *view_sem;
int slave_sig_received;

void sig_slave_handler(int signum);
void sig_handle_viewer();
#endif
