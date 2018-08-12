#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <string.h>
#include <stdlib.h>
#include <semaphore.h>
#define APP_SHM "/appShm"
#define APP_SEM "/appSem"
#define DFLT_SIZE 1000

int newshm(char *name, int oflags, int mode){
	int fd;
	if( (fd = shm_open(name, oflags, mode)) == -1){
		printf("error retrieving shared memoy, terminating\n");
		printf("%s\n", strerror(errno));
		exit(1);
	}
	return fd;
}

//address is NULL, resource allocation is left in the hands of mmap
void *newshmmap(size_t length, int prot, int flags, int fd, off_t offset){
	void *map;
	if( (map = mmap(NULL, length, prot, flags, fd, offset)) == (void*)-1){
		printf("error mapping memory, terminating\n");
		printf("%s\n", strerror(errno));
		exit(1);
	}
	return map;
}

sem_t *newsem(const char *name, int oflag, mode_t mode, unsigned int value){
	sem_t *sem;
	if( (sem = sem_open(name, oflag, mode, value)) == SEM_FAILED){
		printf("error creating semaphore, terminating\n");
		printf("%s\n", strerror(errno));
		exit(1);
	}
	return sem;
}