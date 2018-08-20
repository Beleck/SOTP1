#define _POSIX_C_SOURCE 200809L
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

int main(int argc, char * argv[]) {
    (void) argc;
    (void) argv;

    for (int i = 1; i < argc; i++) {
        md5_file(argv[i]);
    }

    int ppid = getppid();
    size_t size;
    char *line = NULL;
    kill(ppid, SIGUSR1);
    int num_char = getline(&line, &size, stdin);
    while (num_char != -1) {
        line[num_char - 1] = '\0';
        md5_file(line);
        kill(ppid, SIGUSR1);
        num_char = getline(&line, &size, stdin);
    }
    free(line);
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
