#ifndef APPLICATION_H
#include <semaphore.h>
#define APPLICATION_H
#define SLAVE_DIR "./slave"
#define NUM_WORKERS 2
#define BASE_SIZE 1000

sem_t *sem;

void forker(char * files[]);



#endif
