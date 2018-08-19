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
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include "include/application.h"
#include "include/app_signal.h"

int main (int argc, char * argv[]){
    if (argc == 1) {
        fprintf(stderr, "Need at least one file\n");
        exit(EXIT_FAILURE);
    }
    
// Print pid for viewer
    printf("%d\n", getpid());

// Signal initialisation
    NEW_SIGNAL(SIGUSR1, application_action, sig_viewer_handler, SA_RESTART);
    NEW_SIGNAL(SIGUSR2, viewer_action, sig_handle_viewer, SA_RESTART);

// Wait for viewer to connect
    sleep(5);
    char *buffer;
    if (viewer_signal_received) {
        buffer = view_mmap;        
    } else {
        buffer = malloc(sizeof(char) * BASE_SIZE);
    }

// Pipe initialisation
    int master_slave[2];
    int slave_master[2];
    pipe(master_slave);
    pipe(slave_master);

// Variable initialisation

	//Current number of files to send to slave
    int num_files = argc - 1;
	//keeps the total saved
	int total = num_files;
    // Array of slave pid
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
	char *line = NULL;
	size_t size;
	int num_char;
	int printed_files = 0;
    int index_shm = 0;
	reader = fdopen(slave_master[0], "r");
	num_char = getline(&line, &size, reader);
	while(num_char != -1){
        for (int i = 0; i < num_char - 1; i++) {
            buffer[index_shm] = line[i];
            index_shm++;            
        }
        buffer[index_shm] = '-';
        index_shm++;
        sem_post(view_sem);
		printed_files += 1;
		if (printed_files != total) {
			num_char = getline(&line, &size, reader);
		} else {
			//Nothing left to print
            buffer[index_shm] = 43;
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
