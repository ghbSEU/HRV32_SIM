#include<stdio.h>
#include<string.h>
#include "config.h"
#include "mem.h"
#include "common.h"

int dmem[dmem_size];

char *reg_name[]={"zero","ra","sp","gp","tp","t0","t1","t2","s0","s1","a0","a1","a2","a3",\
            "a4","a5","a6","a7","s2","s3","s4","s5","s6","s7","s8","s9","s10","s11",\
            "t3","t4","t5","t6"};

union inst inst_fetch;

unsigned int pc_reg = 0;
unsigned int rv_reg[32] = {0};
int inst_is_rvc = 0;
unsigned int pc_reg_next = 0;
int main()
{
	printf("***************** hrv32_sim start ***************\n");
	txt2mem();
	//printmem();
	int inst_count = 0;
	while(1){
		inst_count++;
		inst_fetch.inst = imem[pc_reg / 4];
		#ifdef DEBUG
		printf("inst_fetch = %8x\n",inst_fetch.inst);
		#endif

		if(inst_fetch.LHtype.L_is_rvc != 3)
		{
			pc_reg_next = pc_reg + 2;
		}
		else
		{
			pc_reg_next = pc_reg + 4;
		}

		decoder_excute(inst_fetch.inst);
		pc_reg = pc_reg_next;
		rv_reg[0] = 0;
		if(inst_count > 100) break;
	}
	return 0;
}
