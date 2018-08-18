#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <semaphore.h>
//#include "appShm.c"
#include "include/application.h"

static int viewerSignalReceived = 0;
static int viewerShmFd;
static char *viewmmap = NULL;
static sem_t *viewSem;
static int sigReceived;

// void sig_handle() {
// //    printf("Signal Received\n");
//     return;
// }

void sig_viewer_handler(int signum){
	sigReceived = 1;
}

// void sig_handle_viewer(){
//     viewerSignalReceived = 1;
//
//     viewerShmFd = newshm(APP_SHM, O_RDWR | O_CREAT, S_IRWXU);
//     posix_fallocate(viewerShmFd, 0, BASE_SIZE);
//     viewmmap = newshmmap(BASE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmFd, 0);
//     viewSem = newsem(APP_SEM, O_CREAT, S_IRWXU, 0);
// }

int main (int argc, char * argv[]){
    if (argc == 1) {
        fprintf(stderr, "Need at least one file\n");
        exit(EXIT_FAILURE);
    }

// Signal initialisation
//    signal(SIGUSR1, sig_handle);
//    signal(SIGUSR2, sig_handle_viewer);
    struct sigaction application_action;
	  application_action.sa_handler = sig_viewer_handler;
	  sigemptyset(&application_action.sa_mask);
	  application_action.sa_flags = SA_RESTART;
	  sigaction(SIGUSR1, &application_action, NULL);

// Pipe initialisation
    int master_slave[2];
    int slave_master[2];
    pipe(master_slave);
    pipe(slave_master);

// Variable initialisation

	//We will decrent num_files
    int num_files = argc - 1;
	//keeps the total saved
	int total = num_files;
	int printed_files=0;
    int child_pid[NUM_WORKERS];
    int num_init = num_files/(NUM_WORKERS*4) + 1;
	FILE * reader;
	char* buffer = NULL;
	size_t size;
	int num_char;

// Childs creation and sending them first files
for ( int i = 0; i < NUM_WORKERS; i++ ) {
        child_pid[i] = fork();
        if ( child_pid[i] == 0) { //child process
            close(master_slave[1]);
            close(slave_master[0]);

            dup2(master_slave[0], STDIN_FILENO);
            dup2(slave_master[1], STDOUT_FILENO);

            close(master_slave[0]);

            execl(SLAVE_DIR, "slave", NULL);
        } else if ( child_pid[i]>0 ){ //parent process
            for (int j = 0; j < num_init; j++) {
                dprintf(master_slave[1], "%s\n", argv[i*num_init + j + 1]);
                num_files--;


            }
        } else { //error
            fprintf(stderr,"Error creating child: %s\n",strerror(errno));
            exit(-1);
      }
    }

	while (num_files > 0) {
	    pause();
	    dprintf(master_slave[1], "%s\n", argv[argc - num_files]);
	    num_files--;
	}



	reader = fdopen(slave_master[0], "r");
	num_char = getline(&buffer, &size, reader);
	application_action.sa_flags = SA_RESTART;
	while(num_char != -1){
		application_action.sa_flags = SA_RESTART;
		buffer[num_char-1] = 0;
		printf("%s\n", buffer);
		printed_files+=1;
		if(printed_files != total){
			num_char = getline(&buffer, &size, reader);
		}else{
			//Nothing left to print
			num_char = -1;
			close(master_slave[1]);
		}

	}

	// Ending of the entire program
	    close(master_slave[0]);
	    close(slave_master[1]);
	    for (int i = 0; i < NUM_WORKERS; i++) {
	        waitpid(child_pid[i], NULL, 0);
	    }

	    return 0;
}
