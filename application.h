#ifndef APPLICATION_H
#define APPLICATION_H
#define SLAVE_DIR "./slave"
#define NUM_WORKERS 10
  void forker2(char * files[]);
  int everyProcessEnded(int childPid[NUM_WORKERS]);



#endif
