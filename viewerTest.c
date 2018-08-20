#define _POSIX_C_SOURCE 200809L
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include "include/app_shm.h"

int shmFd;
int sigReceived = 0;
char *shmMap;
sem_t *sem;

void sig_viewer_handler(int signum){
	sigReceived = 1;
}

int main(void){
	sleep(1);
	struct sigaction viewer_action;
	viewer_action.sa_handler = sig_viewer_handler;
	sigemptyset(&viewer_action.sa_mask);
	viewer_action.sa_flags = 0;
	sigaction(SIGUSR2, &viewer_action, NULL);

	shmFd = newshm(APP_SHM, O_RDWR | O_CREAT, S_IRWXU);
	posix_fallocate(shmFd, 0, 1000);
	shmMap = newshmmap(1000, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
	sem = newsem(APP_SEM, O_CREAT, S_IRWXU, 0);

	printf("%d\n", (int) getpid());
	fflush(stdout);
	sleep(10);
	int i = 0;
	if(sigReceived){		
		char *src = "Hello world!Hello world!Hello edsdadaworld!Hello world!Hello world!Hello world!\nafdjnjfnkjdfvsjdjkjkjHello world!\nHello worlsdkfvjjksfvdnkjsdfvkjnvkjnvfdkjnvdsfkjd!\nHello world!Hello world!Hello world!Hello world!Hello world!Hello world!Hello world!\nHello world!\nHello world!\nHello world!Hello world!Hello world!Hello world!uasybuaysHello world!\nHello world!\n+";
		memcpy(shmMap, src, strlen(src));
	}
	for(; i < 8; i++)
		sem_post(sem);

	shm_unlink(APP_SHM);
	sem_close(sem);
	sem_unlink(APP_SEM);
}