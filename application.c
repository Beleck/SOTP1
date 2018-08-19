#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <semaphore.h>
#include "appShm.c"
#include "include/application.h"

static int viewer_signal_received = 0;
static int viewer_shm_fd;
static char *view_mmap = NULL;
static sem_t *view_sem;
static int sigReceived;

// void sig_handle() {
//     return;
// }

void sig_viewer_handler(int signum){
    (void) signum;

	sigReceived = 1;
}

void sig_handle_viewer(){
    viewer_signal_received = 1;

    viewer_shm_fd = newshm(APP_SHM, O_RDWR | O_CREAT, S_IRWXU);
    posix_fallocate(viewer_shm_fd, 0, BASE_SIZE);
    view_mmap = newshmmap(BASE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, viewer_shm_fd, 0);
    view_sem = newsem(APP_SEM, O_CREAT, S_IRWXU, 0);
}

int main (int argc, char * argv[]){
    if (argc == 1) {
        fprintf(stderr, "Need at least one file\n");
        exit(EXIT_FAILURE);
    }
    
    printf("%d\n", getpid());

// Signal initialisation
    struct sigaction application_action;
	application_action.sa_handler = sig_viewer_handler;
	sigemptyset(&application_action.sa_mask);
	application_action.sa_flags = SA_RESTART;
	sigaction(SIGUSR1, &application_action, NULL);
    
    struct sigaction viewer_action;
	viewer_action.sa_handler = sig_handle_viewer;
	sigemptyset(&viewer_action.sa_mask);
	viewer_action.sa_flags = SA_RESTART;
	sigaction(SIGUSR2, &viewer_action, NULL);

    pause();

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
    int child_pid[NUM_WORKERS];
    // Arbitrary value
    int num_init = num_files/(NUM_WORKERS*4) + 1;

// Childs creation and sending them first files
    for ( int i = 0; i < NUM_WORKERS; i++ ) {
        child_pid[i] = fork();
        if ( child_pid[i] == 0) { //child process
            close(master_slave[1]);
            close(slave_master[0]);
    
            dup2(master_slave[0], STDIN_FILENO);
            dup2(slave_master[1], STDOUT_FILENO);
    
            close(master_slave[0]);
            close(slave_master[1]);
    
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


	FILE *reader;
	char *buffer = NULL;
	size_t size;
	int num_char;
	int printed_files = 0;
    int index_shm = 0;
	reader = fdopen(slave_master[0], "r");
	num_char = getline(&buffer, &size, reader);
	while(num_char != -1){
        for (int i = 0; i < num_char - 1; i++) {
            view_mmap[index_shm] = buffer[i];
            index_shm++;            
        }
        view_mmap[index_shm] = '-';
        index_shm++;
        sem_post(view_sem);
		printed_files += 1;
		if (printed_files != total) {
			num_char = getline(&buffer, &size, reader);
		} else {
			//Nothing left to print
            view_mmap[index_shm] = 43;
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
    munmap(view_mmap, BASE_SIZE);
    shm_unlink(APP_SHM);
    return 0;
}
