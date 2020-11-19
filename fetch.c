#include "debug_fde_ip.h"
#include "fde_ip.h"
#ifndef __SYNTHESIS__
#ifdef DEBUG_FETCH
#include <stdio.h>
#endif
#endif

void fetch(
  code_address_t current_pc,
  instruction_t *code_mem,
  instruction_t *instruction){
#pragma HLS INLINE off
  *instruction = code_mem[current_pc>>2];
#ifndef __SYNTHESIS__
#ifdef DEBUG_FETCH
  printf("%03d: %08x      ",
    (unsigned int)current_pc,
    (unsigned int)*instruction);
#ifndef DEBUG_DISASSEMBLE
  printf("\n");
#endif
#endif
#endif
}
