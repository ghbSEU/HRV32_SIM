#include<stdio.h>
#include<string.h>
#include "config.h"
#include "mem.h"
#include "common.h"
#include "rvcdecoder.h"

int dmem[dmem_size]={0};

char *reg_name[]={"zero","ra","sp","gp","tp","t0","t1","t2","s0","s1",\
				    "a0","a1","a2","a3","a4","a5","a6","a7","s2","s3",\
					"s4","s5","s6","s7","s8","s9","s10","s11","t3","t4","t5","t6"};

union inst inst_fetch;

unsigned int pc_reg = 0;
unsigned int rv_reg[32] = {0};

unsigned int rv_reg_1[32] = {0};
unsigned int rv_reg_2[32] = {0};
unsigned int rv_reg_3[32] = {0};

void copyreg(int *reg,int *reg1)
{
	int i;
	for(i = 0; i <32; i++)
	{
		*reg++ = *reg1++;
	}
}





int inst_is_rvc = 0;
unsigned int pc_reg_next = 0;
int main()
{
	printf("***************** hrv32_sim start ***************\n");
	txt2mem();
	//printmem();
	int inst_count = 0;
	rvc_init();
	while(1){
		inst_count++;
		inst_fetch.inst = *((unsigned int *)(&(((char *)(imem))[pc_reg])));
		
		#ifdef DEBUG
		printf("pc = %x		inst_fetch = %8x\n",pc_reg,inst_fetch.inst);
		#endif

		if(inst_fetch.LHtype.L_is_rvc != 3)
		{
			#ifdef DEBUG
			printf("pc = %x rvc		%x\n",pc_reg,(unsigned short int)(inst_fetch.inst & 0xffff));
			#endif
			pc_reg_next = pc_reg + 2;
			rvc_dec_ex((unsigned short int)(inst_fetch.inst & 0xffff));
		}
		else
		{
			#ifdef DEBUG
			printf("pc = %x rvc		%x\n",pc_reg,inst_fetch.inst);
			#endif
			pc_reg_next = pc_reg + 4;
			decoder_excute(inst_fetch.inst);
		}

		copyreg(rv_reg_3,rv_reg_2);
		copyreg(rv_reg_2,rv_reg_1);
		copyreg(rv_reg_1,rv_reg);
		if(pc_reg >= 8)
	    	cpulog();
		

		if(wdmemflag == 1)
		{
			
			wdmemflag = 0;
			if(wdmemaddr == 0x100000ff)
			{
				printf("%c",(char)(wdmemdata&0xff));
			}
		}

		pc_reg = pc_reg_next;
		rv_reg[0] = 0;
		if(inst_count > 20000) {
			int i = 0;
			for(i=0;i<dmem_size;i++)
			{
				char instout[60]={'\0'};
				sprintf(instout,"%08x\n",dmem[i]);
				addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmem.txt");
			}
			break;
		}
	}
	return 0;
}
