#include<stdio.h>
#include<string.h>
#include "config.h"
#include "mem.h"
#include "common.h"


int wdmemaddr = 0 ;
int wdmemdata = 0 ;
int wdmemflag = 0 ;

#define RS1 rv_reg[inst_decoder.COMRVtype.rs1]
#define RS2 rv_reg[inst_decoder.COMRVtype.rs2]
#define RD  rv_reg[inst_decoder.COMRVtype.rd]

#define RDN reg_name[inst_decoder.COMRVtype.rd]
#define RS1N reg_name[inst_decoder.COMRVtype.rs1]
#define RS2N reg_name[inst_decoder.COMRVtype.rs2]

#define SHAMT inst_decoder.COMRVtype.rs2
#define Iimm  inst_decoder.Itype.imm11_0
#define IimmS sext(inst_decoder.Itype.imm11_0,12)
#define IimmU ((unsigned int)(inst_decoder.Itype.imm11_0))

#define Uimm  inst_decoder.Utype.imm31_12
#define UimmU ((unsigned int)(inst_decoder.Utype.imm31_12)<<12)
#define UimmS (sext(inst_decoder.Utype.imm31_12,20)<<12)

#define Simm (((unsigned int)(inst_decoder.Stype.imm11_5))<<5)|((unsigned int)(inst_decoder.Stype.imm4_0))
#define SimmU Simm
#define SimmS (sext(SimmU,12))


/*
#define JimmU  (unsgined int offset = JimmUe(inst_decoder.inst))
#define JimmS sext(JimmU)


unsigned int JimmUe(int inst){
    union inst inst_decoder;
    inst_decoder.inst = inst;
    unsigned int offset = 0;
    offset |= ((unsigned int)(inst_decoder.Jtype.imm20))<<20;
    offset |= ((unsigned int)(inst_decoder.Jtype.imm10_1))<<1;
    offset |= ((unsigned int)(inst_decoder.Jtype.imm11))<<11;
    offset |= ((unsigned int)(inst_decoder.Jtype.imm19_12))<<12;
    return offset;
}
*/


void cpulog()
{
	char instout[60]={'\0'};
    sprintf(instout,"pc : %08x\t\n",pc_reg);
    addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/cpulog.txt");
    for(int i = 0; i <=31; i++)
    {
        sprintf(instout,"x%02d : %08x\n",i,rv_reg_3[i]);
        addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/cpulog.txt");
    }        
}

signed int sext(unsigned int data,int length)
{
	return ((signed int)(data << (32 - length))) >> ( 32 - length ) ;
}

void addline2txt(char *data,char *filename)
{
	FILE *fp =NULL;
	fp = fopen(filename,"a+");
	fputs(data,fp);
	fclose(fp);
}

void decoder_excute(int inst)
{
    union inst inst_decoder;
    inst_decoder.inst = inst;


    #ifdef DEBUG
    printf("%x\t\n",inst_decoder.COMRVtype.opcode);
    #endif
    switch(inst_decoder.COMRVtype.opcode){
        // lui 
        case  (B0110111):{
            // x[rd] = sext(immediate[31:12] << 12)
            RD = UimmU;
            char instout[60]={'\0'};
            sprintf(instout,"pc:    %x\t lui  %s\t , %x\t       %x\t\n",pc_reg,RDN,UimmU,inst_decoder.inst);
            INSTLOG();
            break;
        };
        // auipc
        case  (B0010111):{
            // x[rd] = pc + sext(immediate[31:12] << 12)
            RD = pc_reg + UimmS;
            char instout[60]={'\0'};
            sprintf(instout,"pc:    %x\t auipc  %s\t , %x\t\n",pc_reg,RDN,UimmS,inst_decoder.inst);
            INSTLOG();
            break;
        };
        // jal
        case  (B1101111):{
            // x[rd] = pc+4; pc += sext(offset)
            RD = pc_reg + 4;
            #ifdef DEBUG
            printf("========================================%s\t = %x\t\n",RDN,RD);
            #endif
            unsigned int offset = 0;
            offset |= ((unsigned int)(inst_decoder.Jtype.imm20))<<20;
            offset |= ((unsigned int)(inst_decoder.Jtype.imm10_1))<<1;
            offset |= ((unsigned int)(inst_decoder.Jtype.imm11))<<11;
            offset |= ((unsigned int)(inst_decoder.Jtype.imm19_12))<<12;
            pc_reg_next = pc_reg + sext(offset,21);
            #ifdef DEBUG
            printf("pc_reg_next %x\t\n ", pc_reg_next);
            #endif
            char instout[60]={'\0'};
            sprintf(instout,"pc:    %x\t jal  %s\t , %x\t   %x\t\n",pc_reg,RDN,(signed int )pc_reg + (signed int)sext(offset,21),inst_decoder.inst);
            INSTLOG();
            break;
        };
        // jalr
        case  (B1100111):{
            switch(inst_decoder.Itype.fun3){
                case  (B000):{
                    // t =pc+4; pc=(x[rs1]+sext(offset))&~1; x[rd]=t
                    unsigned int offset = 0;
                    RD = pc_reg + 4;
                    pc_reg_next = RS1 + IimmS&0xfffffffe;
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t jalr  %s\t , %x\t\n",pc_reg,RDN,IimmU,inst_decoder.inst);
                    INSTLOG();
                    break;
                };
                default: { 
                    #ifdef DEBUG
                    printf("* * * * * illegal inst * * * * *\n");
                    #endif
                    break;
                };
            }
            break;
        };
        // beq,bne,blt,bge,bltu,bgeu
        case  (B1100011):{
            switch(inst_decoder.Btype.fun3){
                case  (B000):{
                    // beq
                    // beq rs1, rs2, offset   //if (rs1 == rs2) pc += sext(offset)
                    unsigned int offset = 0;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm12))<<12;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm11))<<11;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm10_5))<<5;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm4_1))<<1;
                    if (RS1 == RS2)
                    {
                        pc_reg_next = pc_reg + sext(offset,13);
                    }

                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t beq  %s\t , %s\t , %x\t      %x\t\n",pc_reg,RS1N,RS2N,offset,inst_decoder.inst);
                    INSTLOG();
                    break;
                };
                case  (B001):{
                    // bne rs1, rs2, offset
                     //if (rs1 ≠ rs2) pc += sext(offset)
                    unsigned int offset = 0;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm12))<<12;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm11))<<11;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm10_5))<<5;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm4_1))<<1;
                    if (RS1 != RS2)
                    {
                        pc_reg_next = pc_reg + sext(offset,13);
                    }
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t bne  %s\t , %s\t , %x\t      %x\t\n",pc_reg,RS1N,RS2N,offset,inst_decoder.inst);
                    INSTLOG();
                    break;
                };
                case  (B100):{
                    // blt rs1, rs2, offset
                    //if (rs1 < rs2) pc += sext(offset)
                    unsigned int offset = 0;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm12))<<12;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm11))<<11;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm10_5))<<5;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm4_1))<<1;
                   // printf("RS1 = %x        RS2  = %x\n",RS1,RS2);
                    if (((signed int)(RS1)) < ((signed int)(RS2)))
                    {
                        pc_reg_next = pc_reg + sext(offset,13);
                
                    }
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t blt  %s\t , %s\t , %x\t      %x\t\n",pc_reg,RS1N,RS2N,offset,inst_decoder.inst);
                    INSTLOG();
                    break;
                };
                case  (B101):{
                    // bge rs1, rs2, offset 
                    //if (rs1 ≥s rs2) pc += sext(offset)
                    unsigned int offset = 0;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm12))<<12;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm11))<<11;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm10_5))<<5;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm4_1))<<1;
                    if (((signed int)(RS1)) >= ((signed int)(RS2)))
                    {
                        pc_reg_next = pc_reg + sext(offset,13);
                    }
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t bge  %s\t , %s\t , %x\t      %x\t\n",pc_reg,RS1N,RS2N,offset,inst_decoder.inst);
                    INSTLOG();
                    break;
                };
                case  (B110):{
                    // bltu rs1, rs2, offset 
                     //if (rs1 < rs2) pc += sext(offset)
                    unsigned int offset = 0;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm12))<<12;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm11))<<11;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm10_5))<<5;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm4_1))<<1;
                    if (((unsigned int)(RS1)) < ((unsigned int)(RS2)))
                    {
                        pc_reg_next = pc_reg + sext(offset,13);
                    }
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t bltu  %s\t , %s\t , %x\t      %x\t\n",pc_reg,RS1N,RS2N,offset,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case  (B111):{
                    // bgeu rs1, rs2, offset
                    //if (rs1 ≥s rs2) pc += sext(offset)
                    unsigned int offset = 0;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm12))<<12;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm11))<<11;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm10_5))<<5;
                    offset |= ((unsigned int)(inst_decoder.Btype.imm4_1))<<1;
                    if (((unsigned int)(RS1)) >= ((unsigned int)(RS2)))
                    {
                        pc_reg_next = pc_reg + sext(offset,13);
                    }
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t bgeu  %s\t , %s\t , %x\t      %x\t\n",pc_reg,RS1N,RS2N,offset,inst_decoder.inst);
                    INSTLOG();
                    break;
                };
                default: { 
                    #ifdef DEBUG
                    printf("* * * * * illegal inst * * * * *\n");
                    #endif
                    break;
                };
            }
            break;
        };
        // lb,lh,lw,lbu,lhu
        case  (B0000011):{
            switch(inst_decoder.Itype.fun3){
                case B000:{
                    // lb rd, offset(rs1) 
                    // x[rd] = sext(M[x[rs1] + sext(offset)][7:0])
                    

                    if(( (RS1 + IimmS) >= 0 ) && ( (RS1 + IimmS) <= 0x10000000 ) )
                    {
                        RD = sext(((char *)(imem))[(RS1 + IimmS)],8); // 1B
                    }
                    else if(( (RS1 + IimmS) >= 0x10000000 ) && ( (RS1 + IimmS) <= 0x20000000 ) )
                    {
                        RD = sext(((char *)(dmem))[(RS1 + IimmS)-0x10000000],8); // 1B
                    }

                    



                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t lb  %s\t , %d\t(%s\t)       %x\t\n",pc_reg,RDN,IimmS,RS1N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B001:{
                    // lh rd, offset(rs1) 
                    // x[rd] = sext(M[x[rs1] + sext(offset)][15:0])
                  
                    
                    if(( (RS1 + IimmS) >= 0 ) && ( (RS1 + IimmS) <= 0x10000000 ) )
                    {
                        RD = sext(*((short int *)(&(((char *)(imem))[(RS1 + IimmS)]))),16); // hw
                    }
                    else if(( (RS1 + IimmS) >= 0x10000000 ) && ( (RS1 + IimmS) <= 0x20000000 ) )
                    {
                        RD = sext(*((short int *)(&(((char *)(dmem))[(RS1 + IimmS)-0x10000000]))),16); // hw
                    }
                    
                    
                    
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t lh  %s\t , %d\t(%s\t)       %x\t\n",pc_reg,RDN,IimmS,RS1N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B010:{
                    // lw rd, offset(rs1)
                    // x[rd] = sext(M[x[rs1] + sext(offset)][31:0])
                    
                    
                    if(( (RS1 + IimmS) >= 0 ) && ( (RS1 + IimmS) <= 0x10000000 ) )
                    {
                        RD = *((signed int *)(&(((char *)(imem))[(RS1 + IimmS)])));
                    }
                    else if(( (RS1 + IimmS) >= 0x10000000 ) && ( (RS1 + IimmS) <= 0x20000000 ) )
                    {
                        RD = *((signed int *)(&(((char *)(dmem))[(RS1 + IimmS)-0x10000000])));
                    }
                    
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t lw  %s\t , %d\t(%s\t)       %x\t\n",pc_reg,RDN,IimmS,RS1N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B100:{
                    // lbu rd, offset(rs1)   
                    // x[rd] = M[x[rs1] + sext(offset)][7:0]
                    
                    
                    
                    if(( (RS1 + IimmS) >= 0 ) && ( (RS1 + IimmS) <= 0x10000000 ) )
                    {
                        RD = ((unsigned int)(((char *)(imem))[(RS1 + IimmS)]));
                    }
                    else if(( (RS1 + IimmS) >= 0x10000000 ) && ( (RS1 + IimmS) <= 0x20000000 ) )
                    {
                        RD = ((unsigned int)(((char *)(dmem))[(RS1 + IimmS)-0x10000000]));
                    }
                    
                    
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t lbu  %s\t , %d\t(%s\t)       %x\t\n",pc_reg,RDN,IimmS,RS1N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B101:{
                    // lhu rd, offset(rs1)   
                    // x[rd] = M[x[rs1] + sext(offset)][15:0]
                    
                    
                       if(( (RS1 + IimmS) >= 0 ) && ( (RS1 + IimmS) <= 0x10000000 ) )
                    {
                        RD = ((unsigned int )(*((short int *)(&(((char *)(imem))[(RS1 + IimmS)]))))); // hw
                    }
                    else if(( (RS1 + IimmS) >= 0x10000000 ) && ( (RS1 + IimmS) <= 0x20000000 ) )
                    {
                        RD = ((unsigned int )(*((short int *)(&(((char *)(dmem))[(RS1 + IimmS)-0x10000000]))))); // hw
                    }
                    
                    
                    
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t lhu  %s\t , %d\t(%s\t)       %x\t\n",pc_reg,RDN,IimmS,RS1N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                default :{ 
                    #ifdef DEBUG
                    printf("* * * * * illegal inst * * * * *\n");
                    #endif
                    break;
                };
            }
            break;
        };
        // sb,sh,sw
        case  (B0100011):{
            switch(inst_decoder.Stype.fun3){
                case B000:{
                    // sb rs2, offset(rs1)
                    // M[x[rs1] + sext(offset)] = x[rs2][7: 0]

                    char instout[60]={'\0'};
				    sprintf(instout,"\npc : %08x\n",pc_reg);
				    addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                    sprintf(instout,"wdata : %08x\n",((char)(RS2 & 0xff)));
				    addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                    sprintf(instout,"RS1 + SimmS : %08x\t%d\n",RS1,SimmS);
				    addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                    
                   
                    
                    
                     if(( (RS1 + SimmS) >= 0 ) && ( (RS1 + SimmS) <= 0x10000000 ) )
                    {
                        sprintf(instout,"addr : %08x\t : %08x\n",(RS1 + SimmS),*((int *)(&(((char *)(imem))[(RS1 + SimmS)]))));
				        addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                        ((char *)(imem))[(RS1 + SimmS)] = ((char)(RS2 & 0xff));
                        sprintf(instout,"addr : %08x\t : %08x\n",(RS1 + SimmS),*((int *)(&(((char *)(imem))[(RS1 + SimmS)]))));
				        addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                    }
                    else if(( (RS1 + SimmS) >= 0x10000000 ) && ( (RS1 + SimmS) <= 0x20000000 ) )
                    {

                        sprintf(instout,"addr : %08x\t : %08x\n",(RS1 + SimmS),*((int *)(&(((char *)(dmem))[(RS1 + SimmS)-0x10000000]))));
				        addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                        ((char *)(dmem))[(RS1 + SimmS)-0x10000000] = ((char)(RS2 & 0xff));
                        sprintf(instout,"addr : %08x\t : %08x\n",(RS1 + SimmS),*((int *)(&(((char *)(dmem))[(RS1 + SimmS)-0x10000000]))));
				        addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                    }
                    
                    
                    
                    wdmemaddr = (RS1 + SimmS) ;
                    wdmemdata =  ((char)(RS2 & 0xff)) ;
                    wdmemflag = 1 ;
        
                    sprintf(instout,"pc:    %x\t sb  %s\t , %d\t(%s\t)       %x\t\n",pc_reg,RS2N,SimmS,RS1N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B001:{
                    // sh rs2, offset(rs1)
                    //M[x[rs1] + sext(offset) = x[rs2][15: 0]
                    char instout[60]={'\0'};
                    sprintf(instout,"\npc : %08x\n",pc_reg);
				    addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                    sprintf(instout,"wdata : %08x\n",((short int)(RS2 & 0xffff)));
				    addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                    sprintf(instout,"RS1 + SimmS : %08x\t%d\n",RS1,SimmS);
				    addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                    
                    
                    
                    
                    
                    if(( (RS1 + SimmS) >= 0 ) && ( (RS1 + SimmS) <= 0x10000000 ) )
                    {
                        sprintf(instout,"addr : %08x\t : %08x\n",(RS1 + SimmS),*((int *)(&(((char *)(imem))[(RS1 + SimmS)]))));
				        addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                        *((short int *)(&(((char *)(imem))[(RS1 + SimmS)]))) = ((short int)(RS2 & 0xffff));
                        sprintf(instout,"addr : %08x\t : %08x\n",(RS1 + SimmS),*((int *)(&(((char *)(imem))[(RS1 + SimmS)]))));
				        addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                    }
                    else if(( (RS1 + SimmS) >= 0x10000000 ) && ( (RS1 + SimmS) <= 0x20000000 ) )
                    {
                        sprintf(instout,"addr : %08x\t : %08x\n",(RS1 + SimmS),*((int *)(&(((char *)(dmem))[(RS1 + SimmS)-0x10000000]))));
				        addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                        *((short int *)(&(((char *)(dmem))[(RS1 + SimmS)-0x10000000]))) = ((short int)(RS2 & 0xffff));
                        sprintf(instout,"addr : %08x\t : %08x\n",(RS1 + SimmS),*((int *)(&(((char *)(dmem))[(RS1 + SimmS)-0x10000000]))));
				        addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                    }
                    
                    
                    
                    
                    
                    wdmemaddr = (RS1 + SimmS) ;
                    wdmemdata =  ((short int)(RS2 & 0xffff));
                    wdmemflag = 1 ;

                    
                    sprintf(instout,"pc:    %x\t sh  %s\t , %d\t(%s\t)       %x\t\n",pc_reg,RS2N,SimmS,RS1N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B010:{
                    // sw rs2, offset(rs1)
                    // M[x[rs1] + sext(offset) = x[rs2][31: 0]
                    char instout[60]={'\0'};
                    sprintf(instout,"\npc : %08x\n",pc_reg);
				    addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                    sprintf(instout,"wdata : %08x\n",RS2);
				    addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                    sprintf(instout,"RS1 + SimmS : %08x\t%d\n",RS1,SimmS);
				    addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                    
                    
                    
                    
                    if(( (RS1 + SimmS) >= 0 ) && ( (RS1 + SimmS) <= 0x10000000 ) )
                    {
                        sprintf(instout,"addr : %08x\t : %08x\n",(RS1 + SimmS),*((int *)(&(((char *)(imem))[(RS1 + SimmS)]))));
				        addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                        *((int *)(&(((char *)(imem))[(RS1 + SimmS)]))) = RS2;
                        sprintf(instout,"addr : %08x\t : %08x\n",(RS1 + SimmS),*((int *)(&(((char *)(imem))[(RS1 + SimmS)]))));
				        addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                    }
                    else if(( (RS1 + SimmS) >= 0x10000000 ) && ( (RS1 + SimmS) <= 0x20000000 ) )
                    {
                        sprintf(instout,"addr : %08x\t : %08x\n",(RS1 + SimmS),*((int *)(&(((char *)(dmem))[(RS1 + SimmS)-0x10000000]))));
				        addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                        *((int *)(&(((char *)(dmem))[(RS1 + SimmS)-0x10000000]))) = RS2;
                        sprintf(instout,"addr : %08x\t : %08x\n",(RS1 + SimmS),*((int *)(&(((char *)(dmem))[(RS1 + SimmS)-0x10000000]))));
				        addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/dmemlog.txt");
                    }
                    
                    
                    
                    
                    wdmemaddr = (RS1 + SimmS) ;
                    wdmemdata =  RS2;
                    wdmemflag = 1 ;
                    
                    sprintf(instout,"pc:    %x\t sw  %s\t , %d\t(%s\t)       %x\t\n",pc_reg,RS2N,SimmS,RS1N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                default:{ 
                    #ifdef DEBUG
                    printf("* * * * * illegal inst * * * * *\n");
                    #endif
                    break;
                };
            }
            break;
        };
        // addi,slti,sltiu,xori,ori,andi,slli,srli,srai
        case  (B0010011):{
            switch(inst_decoder.Itype.fun3){
                case B000:{
                    // addi rd, rs1, immediate
                    // x[rd] = x[rs1] + sext(immediate)
                    RD = RS1 + IimmS;
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t addi  %s\t , %s\t, %x\t       %x\t\n",pc_reg,RDN,RS1N,IimmU,inst_decoder.inst);
                    INSTLOG();
                 break;
                }
                case B010:{
                    // slti rd, rs1, immediate    
                    // x[rd] = (x[rs1] <s sext(immediate))
                    if((signed int)(RS1) < IimmS) RD = 1;
                    else RD = 0;
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t slti  %s\t , %s\t, %x\t       %x\t\n",pc_reg,RDN,RS1N,IimmU,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B011:{
                    // sltiu rd, rs1, immediate   
                    // x[rd] = (x[rs1] <u sext(immediate))
                    if((unsigned int)(RS1) < (unsigned int)(IimmS)) RD = 1;
                    else RD = 0;
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t sltiu  %s\t , %s\t, %x\t       %x\t\n",pc_reg,RDN,RS1N,IimmU,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B100:{
                    // xori rd, rs1, immediate   
                    // x[rd] = x[rs1] ^ sext(immediate)
                    RD = RS1 ^ IimmS;
                    break;
                }
                 case B110:{
                    // ori rd, rs1, immediate   
                    // x[rd] = x[rs1] | sext(immediate)
                    RD = RS1 | IimmS;
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t ori  %s\t , %s\t, %x\t       %x\t\n",pc_reg,RDN,RS1N,IimmU,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B111:{
                    // andi rd, rs1, immediate  
                    // x[rd] = x[rs1] & sext(immediate)
                    RD = RS1 & IimmS;
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t andi  %s\t , %s\t, %x\t       %x\t\n",pc_reg,RDN,RS1N,IimmU,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B001:{
                    // slli rd, rs1, shamt   
                    // x[rd] = x[rs1] ≪ shamt
                    RD = RS1 << SHAMT;
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t slli  %s\t , %s\t, %x\t       %x\t\n",pc_reg,RDN,RS1N,SHAMT,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B101:{
                    switch (inst_decoder.COMRVtype.fun7)
                    {
                        case B0000000:{
                            // srli rd, rs1, shamt   
                            // x[rd] = (x[rs1] ≫u shamt)
                            RD = ((unsigned int)(RS1)) >> SHAMT;
                            char instout[60]={'\0'};
                            sprintf(instout,"pc:    %x\t srli  %s\t , %s\t, %x\t       %x\t\n",pc_reg,RDN,RS1N,SHAMT,inst_decoder.inst);
                            INSTLOG();
                            break;
                        }
                        case B0100000:{
                            // srai rd, rs1, shamt    
                            // x[rd] = (x[rs1] ≫s shamt)
                            RD = ((signed int)(RS1)) >> SHAMT;
                            char instout[60]={'\0'};
                            sprintf(instout,"pc:    %x\t srai  %s\t , %s\t, %x\t       %x\t\n",pc_reg,RDN,RS1N,SHAMT,inst_decoder.inst);
                            INSTLOG();
                            break;
                        }
                        default:{ 
                            #ifdef DEBUG
                            printf("* * * * * illegal inst * * * * *\n");
                            #endif
                            break;
                        };
                    }
                    break;
                }
                default:{ 
                    #ifdef DEBUG
                    printf("* * * * * illegal inst * * * * *\n");
                    #endif
                    break;
                };
            }            
            break;
        };
        // add,sub,sll,slt,sltu,xor,srl,sra,or,and
        case  (B0110011):{
            switch(inst_decoder.Rtype.fun3){
                case B000:{
                    switch (inst_decoder.Rtype.fun7){
                        case B0000000:{
                            // add rd, rs1, rs2    
                            // x[rd] = x[rs1] + x[rs2]
                            RD = RS1 + RS2;
                            char instout[60]={'\0'};
                            sprintf(instout,"pc:    %x\t add  %s\t , %s\t , %s\t       %x\t\n",pc_reg,RDN,RS1N,RS2N,inst_decoder.inst);
                            INSTLOG();
                            break;
                        }
                        case B0100000:{
                            // sub rd, rs1, rs2    
                            // x[rd] = x[rs1] - x[rs2]
                            RD = RS1 - RS2;
                            char instout[60]={'\0'};
                            sprintf(instout,"pc:    %x\t sub  %s\t , %s\t , %s\t       %x\t\n",pc_reg,RDN,RS1N,RS2N,inst_decoder.inst);
                            INSTLOG();
                            break;
                        }
                        default:{ 
                            #ifdef DEBUG
                            printf("* * * * * illegal inst * * * * *\n");
                            #endif
                            break;
                        };
                    }
                    break;
                }
                case B001:{
                    // sll rd, rs1, rs2   
                    //  x[rd] = x[rs1] ≪ x[rs2]
                    RD = RS1 << (RS2 & 0x1f);
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t sll  %s\t , %s\t , %s\t       %x\t\n",pc_reg,RDN,RS1N,RS2N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B010:{
                    // slt rd, rs1, rs2    
                    // x[rd] = (x[rs1] <s x[rs2])
                    if(((signed int)(RS1)) < ((signed int)(RS2))) RD = 1;
                    else RD = 0;
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t slt  %s\t , %s\t , %s\t       %x\t\n",pc_reg,RDN,RS1N,RS2N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B011:{
                    // sltu rd, rs1, rs2     
                    // x[rd] = (x[rs1] <u x[rs2])
                    if(((unsigned int)(RS1)) < ((unsigned int)(RS2))) RD = 1;
                    else RD = 0;
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t sltu  %s\t , %s\t , %s\t       %x\t\n",pc_reg,RDN,RS1N,RS2N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B100:{
                    // xor rd, rs1, rs2   
                    // x[rd] = x[rs1] ^ x[rs2]
                    RD = RS1 ^ RS2;
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t xor  %s\t , %s\t , %s\t       %x\t\n",pc_reg,RDN,RS1N,RS2N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B101:{
                    switch (inst_decoder.Rtype.fun7){
                        case B0000000:{
                            // srl rd, rs1, rs2  
                            // x[rd] = (x[rs1] ≫u x[rs2])
                            RD = ((unsigned int)(RS1)) >> (RS2 & 0x1f);
                            char instout[60]={'\0'};
                            sprintf(instout,"pc:    %x\t srl  %s\t , %s\t , %s\t       %x\t\n",pc_reg,RDN,RS1N,RS2N,inst_decoder.inst);
                            INSTLOG();
                            break;
                        }
                        case B0100000:{
                            // sra rd, rs1, rs2    
                            // x[rd] = (x[rs1] ≫s x[rs2])
                            RD = ((signed int)(RS1)) >> (RS2 & 0x1f);
                            char instout[60]={'\0'};
                            sprintf(instout,"pc:    %x\t sra  %s\t , %s\t , %s\t       %x\t\n",pc_reg,RDN,RS1N,RS2N,inst_decoder.inst);
                            INSTLOG();
                            break;
                        }
                        default:{ 
                            #ifdef DEBUG
                            printf("* * * * * illegal inst * * * * *\n");
                            #endif
                            break;
                        };
                    }
                    break;
                }
                case B110:{
                    // or rd, rs1, rs2    
                    // x[rd] = x[rs1] | x[rs2]
                    RD = RS1 | RS2;
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t or  %s\t , %s\t , %s\t       %x\t\n",pc_reg,RDN,RS1N,RS2N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B111:{
                    // and rd, rs1, rs2   
                    // x[rd] = x[rs1] & x[rs2]
                    RD = RS1 & RS2;
                    char instout[60]={'\0'};
                    sprintf(instout,"pc:    %x\t and  %s\t , %s\t , %s\t       %x\t\n",pc_reg,RDN,RS1N,RS2N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                default:{ 
                    #ifdef DEBUG
                    printf("* * * * * illegal inst * * * * *\n");
                    #endif
                    break;
                };
            }            
            break;
        };
        // fence,fence.i
        case  (B0001111):{
            switch(inst_decoder.Itype.fun3){
                
            }            
            break;
        };
        // ecall,ebreak,csrrw,csrrs,csrrc,csrrwi,csrrsi,csrrci
        case  (B1110011):{
            switch(inst_decoder.Itype.fun3){
                
            }            
            break;
        };
        default: { 
                    #ifdef DEBUG
                    printf("* * * * * illegal inst * * * * *\n");
                    #endif
                    break;
                };
    }
    











    

}
