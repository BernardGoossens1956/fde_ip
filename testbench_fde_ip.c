#include <stdio.h>
#include "fde_ip.h"

#include "test_op_imm.h"
//#include "test_op.h"
//#include "test_branch.h"
//#include "test_lui_auipc.h"
//#include "test_jal_jalr.h"
//#include "test_sum.h"

int main() {
  unsigned int pc[2];
  unsigned int nbi;
  pc[0] = 0;
  fde_ip(pc, code_mem, &nbi);
  printf("%d fetched and decoded instructions\n", nbi);
  printf("last pc %d\n", pc[1]);
  return 0;
}
