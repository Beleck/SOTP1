#ifndef APP_SHM_H
#define APP_SHM_H
#define APP_SHM "/appShm"
#define APP_SEM "/appSem"
#define BASE_SIZE 10000
#include <semaphore.h>

// Defines wrapper functions for error checks
// During the creation of IPC resources.

int newshm(char *name, int oflags, int mode);
void *newshmmap(size_t length, int prot, int flags, int fd, off_t offset);
sem_t *newsem(const char *name, int oflag, mode_t mode, unsigned int value);

#endif
