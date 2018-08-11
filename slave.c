#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "include/application.h"
#include "include/slave.h"

int main(int argc, char * argv[]){
    md5_file("test");
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
