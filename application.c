#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <semaphore.h>

#define SLAVE_DIR "./slave"
#define NUM_WORKERS 1


int main (int argc, char * argv[]){

    int fds[NUM_WORKERS][2];

    int num_files = argc - 1; 
    int child_pid[NUM_WORKERS];
    int num_init = num_files/(NUM_WORKERS*4) + 1;
    for (int i = 0; i < NUM_WORKERS; i++) {
        pipe(fds[i]);
        child_pid[i] = fork();
        if (!child_pid[i]) {
            close(fds[i][1]);
            dup2(fds[i][0], 0);

            execl(SLAVE_DIR, "slave", NULL);
        } else {
            for (int j = 0; j < num_init; j++) {
                dprintf(fds[i][1], "%s\n", argv[i*num_init + j + 1]);
                num_files--;
            }
        }
    }
    return 0;
}

