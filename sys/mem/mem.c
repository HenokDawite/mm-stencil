#include "sys/mem/mem.h"

#include <fcntl.h>
#include <stdlib.h> // for malloc
#include <stdio.h>  // for fprintf

int mem_init()
{
    // find memory size in config file
    FILE *cfg = fopen("config.cfg", "r");
    if (!cfg) {
        fprintf(stderr, "Could not read config.cfg. Make sure that you are running from the correct directory...\n");
        return -1;
    }

    char dummy_buf[256];
    fgets(dummy_buf, 256, cfg); // ignore one line...
    fgets(dummy_buf, 256, cfg); // ignore one more line... we'll catch any errors next

    int size;
    if (fscanf(cfg, "mem_size : %d", &size) != 1) {
        fprintf(stderr, "Error reading memory size\n");
        return -1;
    }

    if (fclose(cfg) == EOF) {
        fprintf(stderr, "Error closing config");
        return -1;
    }

    dram = malloc(sizeof(char) * size);
    if (!dram) {
        fprintf(stderr, "Could not allocate memory\n");
        return -1;
    }

    return 0;
}
