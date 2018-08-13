#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include "include/slave.h"

int main(int argc, char * argv[]){
    size_t size;
    char *line = NULL;
    int num_char = getline(&line, &size, stdin);
    while (num_char != -1) {
        line[num_char - 1] = '\0';
        md5_file(line);
        kill(getppid(), SIGUSR1);
        num_char = getline(&line, &size, stdin);
    }
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
