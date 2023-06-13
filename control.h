#ifndef CONTROL_H_
#define CONTROL_H_

#include "x16.h"

// Execute a single instruction in the given X16 machine. Update
// memory and registers as required. PC is advanced as appropriate.
// Return 0 on success, or -1 if an error or HALT is encountered.
int execute_instruction(x16_t* machine);

// Update condition code in R_COND based on result in the given register
void update_cond(x16_t* machine, reg_t reg);

#endif   // CONTROL_H_
