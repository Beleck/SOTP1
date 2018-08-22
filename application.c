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
#include "application.h"
#include "app_signal.h"
#include "tools.h"

int main (int argc, char * argv[]){
    if (argc == 1) {
        fprintf(stderr, "Need at least one file\n");
        exit(EXIT_FAILURE);
    }

    int num_workers = 2; //default
    char custom_filename[FILENAME_SIZE] = "results.res";
    int has_flags = check_flags(custom_filename, &num_workers, argc, argv);


// Print pid for viewer
    printf("%d\n", getpid());
    fflush(stdout);

// Signal initialisation
    NEW_SIGNAL(SIGUSR1, application_action, sig_slave_handler, SA_RESTART);
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
    int nb_flags = 2 * has_flags;
    int nb_files = argc - 1 - nb_flags;
	//Current number of files to send to slave
    int calculed_files = nb_files;
    // Array of slave pid
    int child_pid[num_workers];
    // number of files to send first to slaves
    int num_init = calc_init(nb_files, num_workers);

// Children creation and sending them first files
    for (int i = 0; i < num_workers; i++) {
        child_pid[i] = fork();
        if (!child_pid[i]) { // Child process
            close(master_slave[1]);
            close(slave_master[0]);

            dup2(master_slave[0], STDIN_FILENO);
            dup2(slave_master[1], STDOUT_FILENO);

            close(master_slave[0]);
            close(slave_master[1]);
            char *temp[num_init + 2];
            temp[0] = "slave";
            for (int j = 0; j < num_init; j++) {
                temp[j + 1] = argv[i*num_init + j + 1 + nb_flags];
            }
            temp[num_init + 1] = NULL;
            execv(SLAVE_DIR, temp);
        } else if (child_pid[i] > 0) { // Parent process
            calculed_files -= num_init;
        } else { // Error
            fprintf(stderr,"Error creating child: %s\n",strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

	FILE *reader = fdopen(slave_master[0], "r");
	char *line = NULL;
	size_t size;
	int num_char;
    int index_shm = 0;
    int printed_files = 0;
// Sending files to slaves
	while (calculed_files > 0) {
        if (!slave_sig_received) {
	        pause();
        } else {
            // Preventing from for loop bound modification by SIGUSR1
            int temp = slave_sig_received;
            for (int i = 0; i < temp; i++) {
	            dprintf(master_slave[1], "%s\n", argv[argc - calculed_files]);
	            calculed_files--;
	            num_char = getline(&line, &size, reader);
                index_shm = write_to_buffer(buffer, line, index_shm, num_char);
                printed_files++;
                if (viewer_signal_received) {
                    sem_post(view_sem);
                }
            }
            slave_sig_received -= temp;
        }
	}
    for (int i = printed_files; i < nb_files; i++) {
	    num_char = getline(&line, &size, reader);
        index_shm = write_to_buffer(buffer, line, index_shm, num_char);
        if (viewer_signal_received) {
            sem_post(view_sem);
        }
    }

// Get md5 from slaves and fill buffer
    // End of the buffer
    buffer[index_shm] = 43;
    write_to_file(buffer, custom_filename, index_shm);


// Ending of the entire program
    close(master_slave[0]);
    close(master_slave[1]);
    close(slave_master[0]);
    close(slave_master[1]);

    for (int i = 0; i < num_workers; i++) {
        waitpid(child_pid[i], NULL, 0);
    }
    if (viewer_signal_received) {
        munmap(view_mmap, BASE_SIZE);
        shm_unlink(APP_SHM);
        sem_close(view_sem);
        sem_unlink(APP_SEM);
    } else {
        free(buffer);
    }
    fclose(reader);
    free(line);

    return 0;
}
