#define _POSIX_C_SOURCE 200809L
#define BASE_LEN 10
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

	if(argc == 2){
		sscanf(argv[1], "%d", &appPid);
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
	int startOfNextHash = 0;
	while(1){
		if(sem_wait(appSem)){
			printf("error waiting for semaphore, terminating\n");
			printf("%s\n", strerror(errno));
			break;
		}
		char *filehash = NULL;
		int allocatedMemory = 0;
		int i = startOfNextHash;
		//a dash will be used to signal the end of one hash and the start of the next.
		for(; *(hashes + i) != '-'; i++){
			if((i - startOfNextHash) % BASE_LEN == 0){
				if( (filehash = realloc(filehash, allocatedMemory + BASE_LEN)) == NULL){
					printf("error allocating memory, terminating\n");
					return -1;
				}
				allocatedMemory += BASE_LEN;
			}
			filehash[i - startOfNextHash] = hashes[i];
		}
		startOfNextHash += (i - startOfNextHash) + 1;
		if( (i - startOfNextHash) % BASE_LEN == 0){
			filehash = realloc(filehash, BASE_LEN*sizeof(filehash) + 1);
		}
		filehash[i-startOfNextHash] = 0;
		printf("%s\n", filehash);

		//a cero will be used to signal the end of the list of hashes.
		if(hashes[startOfNextHash] == 0){
			break;
		}
	}

	//if everything goes correctly these should never be reached. The application
	//process will kill the view process using kill() upon having finished calculating the hashes
	//and do the shared memory and semaphore cleanups itself. However, were there to be an error with
	//sem_wait it would be necessary to close these resources.
	shm_unlink(APP_SHM);
	sem_close(appSem);
	sem_unlink(APP_SEM);

	return 0;
}