#ifndef APP_FILE_H
#define APP_FILE_H

void write_to_file(char *buffer, const char *name, int length);

int write_to_buffer(char *buffer, char *line, int index_shm, int num_char);

int check_flags(char *option, int *num_workers, int arg_num, char *args[]);

int calc_init(int nb_files, int num_workers);

#endif
