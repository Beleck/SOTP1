#ifndef APPLICATION_H
#include <semaphore.h>
#define APPLICATION_H
#define SLAVE_DIR "./slave"
#define NUM_WORKERS 2

sem_t *sem;

void forker(char * files[]);



#endif
