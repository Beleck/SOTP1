#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>
#include "include/application.h"

int main (int argc, char * argv[]){
    (void) argc;
    (void) argv;

    sem_init(sem, 0, 0);
    forker(argv);
    return 0;
}

void forker(char *files[]){
    (void) files;

    int childPid[NUM_WORKERS];
    for (int i = 0; i < NUM_WORKERS; i++) {
        childPid[i] = fork();
        if (childPid[i]){ //parent node
            waitpid(childPid[i], NULL, 0);
        } else {
            execl(SLAVE_DIR, (char *)&i, NULL);
        }
    }
}
