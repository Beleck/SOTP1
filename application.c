#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "include/application.h"

int main (int argc, char * argv[]){
  forker(argv);
  return 0;
}

/*
 *  Problem: parent process ends and the remaining processes end up orphans.
 */
void forker(char * files[]){
  int childPid[NUM_WORKERS];
  for (int i =0; i<NUM_WORKERS; i++){
    char * args[] = {(char*)&i, NULL};
    childPid[i]=fork();
    if (childPid[i] == 0){ //parent node
      waitpid(childPid[i], NULL, 0);
    }else{
      execv(SLAVE_DIR, args);
    }
  }
}
