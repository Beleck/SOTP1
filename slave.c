#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#include <errno.h>

void md5_file(char *file); 

int main(int argc, char * argv[]){
    sem_t *sem = sem_open("/SEM", O_RDWR);

    for (int i = 1; i < argc; i++) {
        printf("File : %s\n", argv[i]);
        md5_file(argv[i]);
    }

    if (sem_post(sem) == -1) {
        fprintf(stderr, "Error on sem\n");
    }
    sem_close(sem);
    return 0;
}

void md5_file(char *file) {
    int pid = fork();
    if (pid) {
        waitpid(pid, NULL, 0);
    } else {
        execl("/usr/bin/md5sum", "md5sum", file, NULL);
    }
}
