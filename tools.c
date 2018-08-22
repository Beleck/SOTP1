#include <stdio.h>
#include <string.h>

void write_to_file(char *buffer, const char *name, int length) {
    FILE *file = fopen(name, "w+");
    fwrite((void *) buffer, 1, length, file);
    fclose(file);
}

int write_to_buffer(char *buffer, char *line, int index_shm, int num_char) {
    for (int i = 34; i < num_char - 1; i++) {
        buffer[index_shm] = line[i];
        index_shm++;
    }
    buffer[index_shm] = ':';
    index_shm++;
    buffer[index_shm] = ' ';
    index_shm++;
    for (int i = 0; i < 32; i++) {
        buffer[index_shm] = line[i];
        index_shm++;
    }
    buffer[index_shm] = '\n';
    index_shm++;
    return index_shm;
}

int check_flags(char *option, int argc, char *argv[]) {
    int flag = 0;
    for (int i = 0; i < argc; i++) {
        if (flag) {
            strcpy(option, argv[i]);
            return 1;
        }
        if (*(argv[i]) == '-' && *(argv[i]+1) == 'f') {
            flag = 1;
        }
    }
    return 0;
}

int calc_init(int nb_files, int num_workers) {
    if (nb_files < num_workers) {
        return 0;
    } else if (nb_files <= num_workers*2) {
        return 1;
    } else if (nb_files <= num_workers*3) {
        return nb_files/(num_workers*2);
    } else {
       return nb_files/(num_workers*3); 
    }
}
