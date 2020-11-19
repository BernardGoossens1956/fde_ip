#ifndef __EXECUTE
#define __EXECUTE

#include "fde_ip.h"

void execute(
  int *reg_file,
  code_address_t current_pc,
  decoded_instruction_t d_i,
  code_address_t *next_pc);

#endif
