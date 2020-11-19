#include "ap_cint.h"
#include "debug_fde_ip.h"
#include "fde_ip.h"
#ifndef __SYNTHESIS__
#ifdef DEBUG_EMULATE
#include "emulate.h"
#endif
#endif

void read_reg(
  int *reg_file,
  reg_num_t rs1,
  reg_num_t rs2,
  int *rv1,
  int *rv2){
#pragma HLS INLINE off
  *rv1 = reg_file[rs1];
  *rv2 = reg_file[rs2];
}
void write_reg(
  int *reg_file,
  decoded_instruction_t d_i,
  int result){
#pragma HLS INLINE off
  if (d_i.rd     != 0      &&
      d_i.opcode != BRANCH &&
      d_i.opcode != STORE){
    reg_file[d_i.rd] = result;
  }
}
bit_t compute_branch_result(
  int rv1,
  int rv2,
  decoded_instruction_t d_i){
#pragma HLS INLINE off
  switch(d_i.func3){
    case BEQ : return (bit_t)(rv1 == rv2);
    case BNE : return (bit_t)(rv1 != rv2);
    case 2   :
    case 3   : return (bit_t)0;
    case BLT : return (bit_t)(rv1 <  rv2);
    case BGE : return (bit_t)(rv1 >= rv2);
    case BLTU: return (bit_t)
                      ((unsigned int)rv1 <  (unsigned int)rv2);
    case BGEU: return (bit_t)
                      ((unsigned int)rv1 >= (unsigned int)rv2);
  }
  return (bit_t)0;
}
int compute_op_result(
  int rv1,
  int rv2,
  decoded_instruction_t d_i){
//should be "INLINE ON" to fit in the 10ns cycle
#pragma HLS INLINE
  bit_t f7_6   = d_i.func7>>5;
  bit_t r_type = d_i.type == R_TYPE;
  uint5 shift;
  int result;
  if (r_type)
    shift = rv2;
  else//I_TYPE
    shift = ((uint5)d_i.inst_24_21)<<1 |
             (uint5)d_i.inst_20;
  switch(d_i.func3){
    case ADD : if (r_type && f7_6) result = rv1 - rv2;//SUB
               else result = rv1 + rv2;
               break;
    case SLL : result = rv1 << shift;
               break;
    case SLT : result = rv1 < rv2;
               break;
    case SLTU: result = (unsigned int)rv1 < (unsigned int)rv2;
               break;
    case XOR : result = rv1 ^ rv2;
               break;
    case SRL : if (f7_6) result = rv1 >> shift;//SRA
               else result = (unsigned int)rv1 >> shift;
               break;
    case OR  : result = rv1 | rv2;
               break;
    case AND : result = rv1 & rv2;
               break;
  }
  return result;
}
int compute_result(
  int rv1,
  int rv2,
  decoded_instruction_t d_i,
  code_address_t pc){
#pragma HLS INLINE off
  int result;
  switch(d_i.type){
    case R_TYPE:
      result = compute_op_result(rv1, rv2, d_i);
      break;
    case I_TYPE:
      if (d_i.opcode == JALR)
        result = (unsigned int)pc + 4;
      else if (d_i.opcode == LOAD)
        result = 0;
      else if (d_i.opcode == OP_IMM)
        result = compute_op_result(rv1, (int)d_i.imm, d_i);
      else
        result = 0;//(d_i.opcode == SYSTEM)
      break;
    case S_TYPE:
      result = 0;
      break;
    case B_TYPE:
      result = (unsigned int)
               compute_branch_result(rv1, rv2, d_i);
      break;
    case U_TYPE:
      if (d_i.opcode == LUI)
        result = (unsigned int)
                 ((u_uimmediate_t)(d_i.imm<<12));
      else//AUIPC
        result = (int)pc + 
                 (int)((u_uimmediate_t)(d_i.imm<<12));
      break;
    case J_TYPE:
      result = (unsigned int)pc + 4;
      break;
    default:
      result = 0;
      break;
  }
  return result;
}
code_address_t compute_next_pc(
  code_address_t pc,
  int rv1,
  decoded_instruction_t d_i,
  bit_t cond){
#pragma HLS INLINE off
  code_address_t next_pc;
  switch(d_i.type){
    case R_TYPE:
      next_pc = pc + 4;
      break;
    case I_TYPE:
      next_pc = (d_i.opcode==JALR)?
                (code_address_t)
                ((rv1 + (int)d_i.imm)&0xfffffffe):
                (code_address_t)(pc + 4);
      break;
    case S_TYPE:
      next_pc = pc + 4;
      break;
    case B_TYPE:
      next_pc = (cond)?
                (code_address_t)(pc + (d_i.imm<<1)):
                (code_address_t)(pc + 4);
      break;
    case U_TYPE:
      next_pc = pc + 4;
      break;
    case J_TYPE:
      next_pc = pc + (d_i.imm<<1);
      break;
    default:
      next_pc = 0;
      break;
  }
  return next_pc;
}
void execute(
  int *reg_file,
  code_address_t current_pc,
  decoded_instruction_t d_i,
  code_address_t *next_pc){
#pragma HLS INLINE off
  int rv1, rv2, result;
  read_reg(reg_file, d_i.rs1, d_i.rs2, &rv1, &rv2);
  result = compute_result(rv1,
                          rv2,
                          d_i,
                          current_pc);
  write_reg(reg_file, d_i, result);
  *next_pc = compute_next_pc(current_pc,
                             rv1,
                             d_i,
                            (bit_t)result);
#ifndef __SYNTHESIS__
#ifdef DEBUG_EMULATE
  emulate(reg_file, d_i, *next_pc);
#endif
#endif
}
