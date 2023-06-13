#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include "io.h"

/* Input Buffering */
struct termios original_tio;

void disable_input_buffering() {
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;
    new_tio.c_lflag &= ~ICANON & ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restore_input_buffering() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
    // set back to canonical
    // tcsetattr(0, TCSANOW, &original_tio_out);
}

/* Handle Interrupt */
void handle_interrupt(int signal) {
    restore_input_buffering();
    printf("Control-C, quitting\n");
    exit(-2);
}
