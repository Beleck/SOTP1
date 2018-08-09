#include <stdio.h>
#include <unistd.h>
#include "include/application.h"


int main(int argc, char * argv[]){
  printf("On children %d, PID= %d \n", *argv[0], getpid());
  switch (*argv[0]){
    case 0: sleep(2); break;
    case 1: sleep(1); break;
    case 2: sleep(5); break;
    case 3: sleep(7); break;
    case 4: sleep(10); break;
    case 5: sleep(1); break;
    case 6: sleep(4); break;
    case 7: sleep(7); break;
    case 8: sleep(11); break;
    case 9: sleep(2); break;
  }
  printf("Ending children %d \n", *argv[0]);
}
