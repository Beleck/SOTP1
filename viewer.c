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

int getAppPid(int argc, char** argv);

//reads from the semaphore until it reads a cero from the shared memory
void printHashes(char *hashes, sem_t *appSem);

//reads a hash from the shm and returns the ammount of characters read.
int readHash(char *hashes, int startOfNextHash);

void printHash(int length, char *filehash, int timesAllocatedMemory);

int main(int argc, char** argv){

int appPid = getAppPid(argc, argv);

	//signal the application process that the view has begun.
	//Using SIGCHLD since the default is to ignore that signal, if not handled properly
	//it at least won't kill the application process.
	if(kill(appPid, SIGUSR2) == -1){
		printf("error signalling the application process, terminating\n");
		printf("%s\n", strerror(errno));
		return -1;
	}

	int appShmFd = newshm(APP_SHM, O_RDWR | O_CREAT, S_IRWXU);
	//memory allocation is left to the applicaction process
	char *hashes = newshmmap(DFLT_SIZE, PROT_READ, MAP_SHARED, appShmFd, 0);
	sem_t *appSem = newsem(APP_SEM, O_CREAT, S_IRWXU, 0);

	printHashes(hashes, appSem);

	shm_unlink(APP_SHM);
	sem_close(appSem);
	sem_unlink(APP_SEM);

	return 0;
}

int getAppPid(int argc, char** argv){

	int appPid;

	if(argc == 2){
		sscanf(argv[1], "%d", &appPid);
		//the process with pid 1 is init so any id less or equal to that will be invalid.
		//If the appPid is greater than this processes pid it means the application process
		//hasn't been run yet so there will be nobody to signal
		if(appPid <= 1 || appPid >= (int) getpid()){
			printf("invalid process id, process ending\n");
			exit(1);
		}
	}
	else {
		do{
			scanf("%d", &appPid);
		}
		while(appPid <= 1 || appPid >= (int) getpid());
	}
	return appPid;
}

void printHashes(char *hashes, sem_t *appSem){

	int startOfNextHash = 0;
	while(1){
		if(sem_wait(appSem)){
			printf("error waiting for semaphore, terminating\n");
			printf("%s\n", strerror(errno));
			break;
		}
		startOfNextHash += readHash(hashes, startOfNextHash) + 1;
		//a cero will be used to signal the end of the list of hashes.
		if(hashes[startOfNextHash] == 0){
			break;
		}
	}
}

int readHash(char *hashes, int startOfNextHash){

	char *filehash = NULL;
	int timesAllocatedMemory = 0;
	int i = startOfNextHash;
	//a dash will be used to signal the end of one hash and the start of the next.
	for(; *(hashes + i) != '-'; i++){
		if((i - startOfNextHash) % BASE_LEN == 0){
			if( (filehash = realloc(filehash, timesAllocatedMemory*(BASE_LEN+1))) == NULL){
				printf("error allocating memory, terminating\n");
				exit(1);
			}
			timesAllocatedMemory++;
		}
		filehash[i - startOfNextHash] = hashes[i];
	}
	printHash(i - startOfNextHash, filehash, timesAllocatedMemory);
	return i - startOfNextHash;
}

void printHash(int length, char *filehash, int timesAllocatedMemory){

	if( length % BASE_LEN == 0){
		if ((filehash = realloc(filehash, timesAllocatedMemory*BASE_LEN + 1)) == NULL){
			printf("error allocating memory, terminating\n");
				exit(1);
		}
	}
	filehash[length] = 0;
	printf("%s\n", filehash);
}