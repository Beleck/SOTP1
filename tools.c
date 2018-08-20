#include <stdio.h>

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
