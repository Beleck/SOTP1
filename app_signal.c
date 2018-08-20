#define _POSIX_C_SOURCE 200809L
#include <fcntl.h>
#include <sys/mman.h>
#include "app_signal.h"

int viewer_signal_received = 0;
int slave_sig_received = 0;
char *view_mmap = NULL;

void sig_slave_handler(int signum) {
    (void) signum;

	slave_sig_received++;
}

void sig_handle_viewer() {
    viewer_signal_received = 1;

    viewer_shm_fd = newshm(APP_SHM, O_RDWR | O_CREAT, S_IRWXU);
    posix_fallocate(viewer_shm_fd, 0, BASE_SIZE);
    view_mmap = newshmmap(BASE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, viewer_shm_fd, 0);
    view_sem = newsem(APP_SEM, O_CREAT, S_IRWXU, 0);
}

