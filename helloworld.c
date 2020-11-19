#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xfde_ip_hw.h"

unsigned int *pc  =
  (unsigned int *)
  (XPAR_XFDE_IP_0_S_AXI_CONTROL_BASEADDR
 +      XFDE_IP_CONTROL_ADDR_PC_BASE);
unsigned int *mem =
  (unsigned int *)
  (XPAR_XFDE_IP_0_S_AXI_CONTROL_BASEADDR
 +      XFDE_IP_CONTROL_ADDR_CODE_MEM_BASE);
unsigned int *nbi =
  (unsigned int *)
  (XPAR_XFDE_IP_0_S_AXI_CONTROL_BASEADDR
 +      XFDE_IP_CONTROL_ADDR_NB_INSTRUCTION_DATA);

unsigned int code_memory[16]=
{
  0x00500593,
  0x00158613,
  0x00c67693,
  0xfff68713,
  0x00576793,
  0x00c7c813,
  0x00d83893,
  0x00b83293,
  0x01c81313,
  0xff632393,
  0x7e633e13,
  0x01c35e93,
  0x41c35f13,
  0x00008067
};

int main()
{
  unsigned int i;
  pc[0] = 0;
  pc[1] = 0;
  for (i=0; i<16; i++)
    mem[i] = code_memory[i];

  init_platform();

  while (pc[1]==0);
  print("pc ");
  xil_printf("%d", pc[1]);
  print("\n\r");
  print("nb instructions ");
  xil_printf("%d", *nbi);
  print("\n\r");

  cleanup_platform();
  return 0;
}
