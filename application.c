#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "include/application.h"

int main (int argc, char * argv[]){
  forker2(argv);
  return 0;
}

/*
 *  Problem: Program ends but keeps running, its strange, try it and you'll understand
 */
void forker2(char * files[]){
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
