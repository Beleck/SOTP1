#define _POSIX_SOURCE
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "appShm.c"

int main(int argc, char** argv){

int appPid = 0;

	if(argc == 1){
		sscanf(argv[0], "%d", &appPid);
		//the process with pid 1 is init so any id less or equal to that will be invalid.
		//If the appPid is greater than this processes pid it means the application process
		//hasn't been run yet so there will be nobody to signal
		if(appPid <= 1 || appPid >= (int) getpid()){
			printf("invalid process id, process ending\n");
			return -1;
		}
	}
	else {
		do{
			scanf("%d", &appPid);
		}
		while(appPid <= 1 || appPid >= (int) getpid());
	}

	//signal the application process that the view has begun.
	//Using SIGCHLD since the default is to ignore that signal, if not handled properly
	//it at least won't kill the application process.
	if(kill(appPid, SIGCHLD) == -1){
		printf("error signalling the application process, terminating\n");
		printf("%s\n", strerror(errno));
		return -1;
	}

	int appShmFd = newshm(APP_SHM, O_RDWR | O_CREAT, S_IRWXU);
	//memory allocation is left to the applicaction process
	char *hashes = newshmmap(DFLT_SIZE, PROT_READ, MAP_SHARED, appShmFd, 0);
	sem_t *appSem = newsem(APP_SEM, O_CREAT, S_IRWXU, 0);

	//reads from the semaphore until signalled by the application process with kill
	while(1){
		if(sem_wait(appSem)){
			printf("error waiting for semaphore\n");
			printf("%s\n", strerror(errno));
			break;
		}
	}

	shm_unlink(APP_SHM);
	sem_close(appSem);

	return 0;
}