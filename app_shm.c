#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#include "app_shm.h"

int newshm(char *name, int oflags, int mode){
	int fd = shm_open(name, oflags, mode);
	if (fd == -1) {
		printf("Error retrieving shared memoy, terminating\n");
		printf("%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	return fd;
}

// Address is NULL, resource allocation is left in the hands of mmap
void *newshmmap(size_t length, int prot, int flags, int fd, off_t offset){
	void *map = mmap(NULL, length, prot, flags, fd, offset);
	if (map == (void*) -1) {
		printf("Error mapping memory, terminating\n");
		printf("%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	return map;
}

sem_t *newsem(const char *name, int oflag, mode_t mode, unsigned int value){
	sem_t *sem = sem_open(name, oflag, mode, value);
	if (sem == SEM_FAILED) {
		printf("Error creating semaphore, terminating\n");
		printf("%s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	return sem;
}
