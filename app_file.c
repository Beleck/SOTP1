#include <stdio.h>

void write_to_file(char *buffer, const char *name, int length) {
    FILE *file = fopen(name, "w+");
    fwrite((void *) buffer, length, 1, file);
    fclose(file);
}
