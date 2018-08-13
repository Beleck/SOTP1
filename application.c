#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <semaphore.h>

#define SLAVE_DIR "./slave"
#define NUM_WORKERS 2 


int main (int argc, char * argv[]){

    int fd[2];
    pipe(fd);

    int num_files = argc - 1; 
    int child_pid[NUM_WORKERS];
    int num_init = num_files/(NUM_WORKERS*4) + 1;
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
    
    close(fd[0]);
    close(fd[1]);
    for (int i = 0; i < NUM_WORKERS; i++) {
        waitpid(child_pid[i], NULL, 0);
    }

    return 0;
}

