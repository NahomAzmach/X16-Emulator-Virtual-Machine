#ifndef IO_H_
#define IO_H_

void disable_input_buffering(void);

void restore_input_buffering(void);

void handle_interrupt(int signal);

#endif  // IO_H_
