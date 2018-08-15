#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <semaphore.h>
#include "appShm.c"

#define SLAVE_DIR "./slave"
#define NUM_WORKERS 2
#define BASE_SIZE 1000

static int viewerSignalReceived = 0;
static int viewerShmFd;
static char *viewmmap = NULL;
static sem_t *viewSem;

void sig_handle() {
//    printf("Signal Received\n");
    return;
}

void sig_handle_viewer(){
    viewerSignalReceived = 1;

    viewerShmFd = newshm(APP_SHM, O_RDWR | O_CREAT, S_IRWXU);
    posix_fallocate(viewerShmFd, 0, BASE_SIZE);
    viewmmap = newshmmap(BASE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
    viewSem = newsem(APP_SEM, O_CREAT, S_IRWXU, 0);
}

int main (int argc, char * argv[]){
    if (argc == 1) {
        fprintf(stderr, "Need at least one file\n");
        exit(EXIT_FAILURE);
    }

// Signal initialisation
    signal(SIGUSR1, sig_handle);
    signal(SIGUSR2, sig_handle_viewer);

// Pipe initialisation
    int fd[2];
    pipe(fd);

// Variable initialisation
    int num_files = argc - 1;
    int child_pid[NUM_WORKERS];
    int num_init = num_files/(NUM_WORKERS*4) + 1;

// Childs creation and sending them first files
    for (int i = 0; i < NUM_WORKERS; i++) {
        child_pid[i] = fork();
        if (!child_pid[i]) {
            close(fd[1]);
            dup2(fd[0], 0);

            execl(SLAVE_DIR, "slave", NULL);
        } else {
            for (int j = 0; j < num_init; j++) {
                dprintf(fd[1], "%s\n", argv[i*num_init + j + 1]);
                num_files--;
            }
        }
    }

    while (num_files > 0) {
        pause();
        dprintf(fd[1], "%s\n", argv[argc - num_files]);
        num_files--;
    }

// Ending of the entire program
    close(fd[0]);
    close(fd[1]);
    for (int i = 0; i < NUM_WORKERS; i++) {
        waitpid(child_pid[i], NULL, 0);
    }

    return 0;
}
