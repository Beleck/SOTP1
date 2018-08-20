#define _POSIX_C_SOURCE 200809L
#define BASE_LEN 10
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include "include/app_shm.h"
#include "CuTest.h"

int get_app_pid(int argc, char **argv);

// Reads from the semaphore until it reads a cero from the shared memory
void print_hashes(char *hashes, sem_t *app_sem);

// Reads a hash from the shm and returns the ammount of characters read.
int read_hash(char *hashes, int start_next_hash);

void print_hash(int length, char *filehash);

void checkForTests(int argc, char **argv);


int main(int argc, char **argv){

	//checkForTests(argc, argv);
	int app_pid = get_app_pid(argc, argv);

	// Signal the application process that the view has begun.
	if (kill(app_pid, SIGUSR2) == -1) {
		printf("error signalling the application process, terminating\n");
		printf("%s\n", strerror(errno));
		exit(1);
	}

	int app_shm_fd = newshm(APP_SHM, O_RDWR | O_CREAT, S_IRWXU);
	// Memory allocation is left to the applicaction process
	char *hashes = newshmmap(DFLT_SIZE, PROT_READ, MAP_SHARED, app_shm_fd, 0);
	sem_t *app_sem = newsem(APP_SEM, O_CREAT, S_IRWXU, 0);

	print_hashes(hashes, app_sem);

	shm_unlink(APP_SHM);
	sem_close(app_sem);
	sem_unlink(APP_SEM);

	return 0;
}

int get_app_pid(int argc, char** argv){

	int app_pid;

	if (argc == 2) {
		sscanf(argv[1], "%d", &app_pid);
		//the process with pid 1 is init so any id less or equal to that will be invalid.
		//If the app_pid is greater than this processes pid it means the application process
		//hasn't been run yet so there will be nobody to signal
		if (app_pid <= 1 || app_pid >= (int) getpid()) {
			printf("invalid process id, process ending\n");
			exit(1);
		}
	} else {
		do{
			scanf("%d", &app_pid);
            printf("app_pid : %d\n", app_pid);
		}
		while (app_pid <= 1);
	}
	return app_pid;
}

void print_hashes(char *hashes, sem_t *app_sem){

	int start_next_hash = 0;
	while (1) {
		if (sem_wait(app_sem)) {
			printf("error waiting for semaphore, terminating\n");
			printf("%s\n", strerror(errno));
			break;
		}
		start_next_hash += read_hash(hashes, start_next_hash) + 1;
		// A plus will be used to signal the end of the list of hashes.
		if (hashes[start_next_hash] == 43) {
			break;
		}
	}
}

int read_hash(char *hashes, int start_next_hash){

	char *filehash = NULL;
	int length_cur_hash = 0;
	// A dash will be used to signal the end of one hash and the start of the next.
	for (int i = start_next_hash; hashes[i] != '\n' && hashes[i] != '\0'; i++, length_cur_hash++) {
		if (length_cur_hash % BASE_LEN == 0) {
            filehash = realloc(filehash, length_cur_hash + BASE_LEN);
			if (filehash == NULL) {
				printf("error allocating memory, terminating\n");
				exit(1);
			}
		}
		filehash[length_cur_hash] = hashes[i];
	}
	print_hash(length_cur_hash, filehash);
    free(filehash);
	return length_cur_hash;
}

void print_hash(int length, char *filehash){

	if (length % BASE_LEN == 0) {
        filehash = realloc(filehash, length + 1);
		if (filehash == NULL) {
			printf("error allocating memory, terminating\n");
			exit(1);
		}
	}
	filehash[length] = 0;
	printf("%s\n", filehash);
}

//void checkForTests(int argc, char** argv){
//	printf("%s\n", argv[1]);
//	if(argc == 2 && strcmp(argv[1], "--test") == 0){
//		RunAllTests();
//		exit(0);
//	}
//}
