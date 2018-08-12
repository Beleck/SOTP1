#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <semaphore.h>

#define SLAVE_DIR "./slave"
#define NUM_WORKERS 1


int main (int argc, char * argv[]){
    sem_unlink("/SEM");
//    return 0;

    sem_t *sem = sem_open("/SEM", O_CREAT, 0600, 0);

    int num_files = argc - 1; 
    int child_pid[NUM_WORKERS];
    int num_init = num_files/(NUM_WORKERS*4) + 1;
    for (int i = 0; i < NUM_WORKERS; i++) {
        child_pid[i] = fork();
        if (!child_pid[i]){
            char *temp[num_init + 2];
            temp[0] = "slave";
            for (int j = 1; j < num_init + 1; j++) {
                temp[j] = argv[i*num_init + j];
            }
            temp[num_init + 1] = NULL;
            execv(SLAVE_DIR, temp);
        }
    }

    num_files -= NUM_WORKERS * num_init;

    //while (num_files > 0) {
    //    sem_wait(sem);
    //}

    for (int i = 0; i < NUM_WORKERS; i++) {
        waitpid(child_pid[i], 0, 0);
    }
    sem_close(sem);
    sem_unlink("/SEM");
    return 0;
}

