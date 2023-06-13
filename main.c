#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <signal.h>
#include <arpa/inet.h>
#include <string.h>
#include "instruction.h"
#include "x16.h"
#include "io.h"
#include "control.h"


// Read Image File. Return 0 on success or -1 for failure
static int read_image_file(x16_t* machine, FILE* fp) {
    // The origin tells us where in memory to place the image
    uint16_t origin;
    if (fread(&origin, sizeof(origin), 1, fp) <= 0) {
        return -1;
    }
    // Swap to host format
    origin = ntohs(origin);

    // we know the maximum file size so we only need one fread
    uint16_t max_read = UINT16_MAX - origin;
    uint16_t* p = x16_memory(machine, origin);
    size_t read = fread(p, sizeof(uint16_t), max_read, fp);
    if (read <= 0) {
        return -1;    // nothing read, or some error in fread
    }

    // swap each 16 bit value to host format
    while (read-- > 0) {
        *p = ntohs(*p);
        ++p;
    }

    return 0;
}

// Read Image into memory. Return 0 on success or -1 for failure.
static int read_image(x16_t* machine, const char* image_path) {
    FILE* fp = fopen(image_path, "rb");
    if (fp == NULL) {
        return -1;
    }
    int rv = read_image_file(machine, fp);
    fclose(fp);
    return rv;
}

static void usage() {
    printf("Usage: x16 [-l] image-file1\n");
    exit(1);
}

int main(int argc, char** argv) {
    int ch;
    while ((ch = getopt(argc, argv, "l")) != -1) {
        switch (ch) {
        case 'l':
            LOG = 1;
            break;

        default:
            usage();
        }
    }
    argc -= optind;
    argv += optind;


    char* filename = "a.obj";
    if (argc > 1) {
        usage();
    } else if (argc == 1) {
        filename = argv[0];
    }

    // Initialize machine
    x16_t* machine = x16_create();

    // Read the image file into memory
    if (read_image(machine, filename) != 0) {
        fprintf(stderr, "Failed to read image: %s\n", filename);
        exit(1);
    }

    // Set up signal handler to clean up TTY state on SIGINT
    signal(SIGINT, handle_interrupt);

    // Disable so we can read keystrokes without newline
    disable_input_buffering();

    // Execute the emulation till we see a halt or some error occurs
    for (;;) {
        if (LOG) {
            x16_print(machine);
        }
        if (execute_instruction(machine) != 0) {
            break;
        }
    }

    // Restore TTY state
    restore_input_buffering();

    x16_free(machine);
}
