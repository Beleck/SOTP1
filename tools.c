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

int check_flags(char *option, int *num_workers, int argc, char *argv[]) {
    int flag = 0;
    int flag_acum = 0;
    for (int i = 0; i < argc; i++) {
        if (flag == 1) {
            strcpy(option, argv[i]);
            strcat(option, ".res");
            flag_acum++;
            flag = 0;
        } else {
            if (flag == 2) {
                *num_workers = *argv[i] - '0';
                flag = 0;
                flag_acum++;
            }
        }
        if (*(argv[i]) == '-' && *(argv[i]+1) == 'f') {
            flag = 1;
        } else {
            if(*(argv[i]) == '-' && *(argv[i]+1) == 'w') {
                flag = 2;
            }
        }
    }
    return flag_acum;
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
