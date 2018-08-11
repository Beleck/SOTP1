#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "include/application.h"

int main (int argc, char * argv[]){
  forker(argv);
  return 0;
}

void forker(char *files[]){
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
