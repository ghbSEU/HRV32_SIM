#include<stdio.h>
#include<string.h>
#include "config.h"
#include "mem.h"
#include "common.h"



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
#define UimmU ((unsigned int)(inst_decoder.Utype.imm31_12) << 12)
#define UimmS (sext( inst_decoder.Utype.imm31_12,20) << 12 )

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

#define INSTLOG() addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/instlog.txt")

void cpulog()
{
	char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\n",pc_reg);
    addline2txt("------------------------------------\n","/home/ghb/HRV32/HRV32_SIM/cpulog.txt");
    addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/cpulog.txt");
    for(int i = 0; i <=31; i++)
    {
        sprintf(instout,"%s:    %x\n",reg_name[i],rv_reg[i]);
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

    //printf(" inst decoder and excute \n ");
    
    printf("%x\n",inst_decoder.COMRVtype.opcode);
    switch(inst_decoder.COMRVtype.opcode){
        // lui 
        case  (B0110111):{
            // x[rd] = sext(immediate[31:12] << 12)
            RD = UimmU;
            char instout[60]={'\0'};
            sprintf(instout,"lui  %s , %x       %x\n",RDN,UimmU,inst_decoder.inst);
            INSTLOG();
            break;
        };
        // auipc
        case  (B0010111):{
            // x[rd] = pc + sext(immediate[31:12] << 12)
            RD = pc_reg + UimmS;
            char instout[60]={'\0'};
            sprintf(instout,"auipc  %s , %x\n",RDN,UimmS,inst_decoder.inst);
            INSTLOG();
            break;
        };
        // jal
        case  (B1101111):{
            // x[rd] = pc+4; pc += sext(offset)
            RD = pc_reg + 4;
            unsigned int offset = 0;
            offset |= ((unsigned int)(inst_decoder.Jtype.imm20))<<20;
            offset |= ((unsigned int)(inst_decoder.Jtype.imm10_1))<<1;
            offset |= ((unsigned int)(inst_decoder.Jtype.imm11))<<11;
            offset |= ((unsigned int)(inst_decoder.Jtype.imm19_12))<<12;
            pc_reg_next = pc_reg + sext(offset,21);
            printf("pc_reg_next %x\n ", pc_reg_next);
            char instout[60]={'\0'};
            sprintf(instout,"jal  %s , %x\n",RDN,sext(offset,21),inst_decoder.inst);
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
                    sprintf(instout,"jalr  %s , %x\n",RDN,IimmU,inst_decoder.inst);
                    INSTLOG();
                    break;
                };
                default: { 
                    printf("* * * * * illegal inst * * * * *\n");
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
                    sprintf(instout,"beq  %s , %s , %x      %x\n",RS1N,RS2N,offset,inst_decoder.inst);
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
                    sprintf(instout,"bne  %s , %s , %x      %x\n",RS1N,RS2N,offset,inst_decoder.inst);
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
                    if (((signed int)(RS1)) < ((signed int)(RS2)))
                    {
                        pc_reg_next = pc_reg + sext(offset,13);
                    }
                    char instout[60]={'\0'};
                    sprintf(instout,"blt  %s , %s , %x      %x\n",RS1N,RS2N,offset,inst_decoder.inst);
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
                    sprintf(instout,"bge  %s , %s , %x      %x\n",RS1N,RS2N,offset,inst_decoder.inst);
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
                    sprintf(instout,"bltu  %s , %s , %x      %x\n",RS1N,RS2N,offset,inst_decoder.inst);
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
                    sprintf(instout,"bgeu  %s , %s , %x      %x\n",RS1N,RS2N,offset,inst_decoder.inst);
                    INSTLOG();
                    break;
                };
                default: { 
                    printf("* * * * * illegal inst * * * * *\n");
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
                    RD = sext(((char *)(dmem))[(RS1 + IimmS)],8); // 1B
                    char instout[60]={'\0'};
                    sprintf(instout,"lb  %s , %x(%s)       %x\n",RDN,IimmS,RS1N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B001:{
                    // lh rd, offset(rs1) 
                    // x[rd] = sext(M[x[rs1] + sext(offset)][15:0])
                    RD = sext(*((short int *)(&(((char *)(dmem))[(RS1 + IimmS)]))),16); // hw
                    char instout[60]={'\0'};
                    sprintf(instout,"lh  %s , %x(%s)       %x\n",RDN,IimmS,RS1N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B010:{
                    // lw rd, offset(rs1)
                    // x[rd] = sext(M[x[rs1] + sext(offset)][31:0])
                    RD = *((signed int *)(&(((char *)(dmem))[(RS1 + IimmS)])));
                    char instout[60]={'\0'};
                    sprintf(instout,"lw  %s , %x(%s)       %x\n",RDN,IimmS,RS1N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B100:{
                    // lbu rd, offset(rs1)   
                    // x[rd] = M[x[rs1] + sext(offset)][7:0]
                    RD = ((unsigned int)(((char *)(dmem))[(RS1 + IimmS)]));
                    char instout[60]={'\0'};
                    sprintf(instout,"lbu  %s , %x(%s)       %x\n",RDN,IimmS,RS1N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B101:{
                    // lhu rd, offset(rs1)   
                    // x[rd] = M[x[rs1] + sext(offset)][15:0]
                    RD = ((unsigned int )(*((short int *)(&(((char *)(dmem))[(RS1 + IimmS)]))))); // hw
                    char instout[60]={'\0'};
                    sprintf(instout,"lhu  %s , %x(%s)       %x\n",RDN,IimmS,RS1N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                default :{ 
                    printf("* * * * * illegal inst * * * * *\n");
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
                    ((char *)(dmem))[(RS1 + IimmS)] = ((char)(RS2 & 0xff));
                    char instout[60]={'\0'};
                    sprintf(instout,"sb  %s , %x(%s)       %x\n",RS2N,IimmU,RS1N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B001:{
                    // sh rs2, offset(rs1)
                    //M[x[rs1] + sext(offset) = x[rs2][15: 0]
                    *((short int *)(&(((char *)(dmem))[(RS1 + IimmS)]))) = ((short int)(RS2 & 0xffff));
                    char instout[60]={'\0'};
                    sprintf(instout,"sh  %s , %x(%s)       %x\n",RS2N,IimmU,RS1N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B010:{
                    // sw rs2, offset(rs1)
                    // M[x[rs1] + sext(offset) = x[rs2][31: 0]
                    *((int *)(&(((char *)(dmem))[(RS1 + IimmS)]))) = RS2;
                    char instout[60]={'\0'};
                    sprintf(instout,"sw  %s , %x(%s)       %x\n",RS2N,IimmU,RS1N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                default:{ 
                    printf("* * * * * illegal inst * * * * *\n");
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
                    sprintf(instout,"addi  %s , %s, %x       %x\n",RDN,RS1N,IimmU,inst_decoder.inst);
                    INSTLOG();
                 break;
                }
                case B010:{
                    // slti rd, rs1, immediate    
                    // x[rd] = (x[rs1] <s sext(immediate))
                    if((signed int)(RS1) < IimmS) RD = 1;
                    else RD = 0;
                    char instout[60]={'\0'};
                    sprintf(instout,"slti  %s , %s, %x       %x\n",RDN,RS1N,IimmU,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B011:{
                    // sltiu rd, rs1, immediate   
                    // x[rd] = (x[rs1] <u sext(immediate))
                    if((unsigned int)(RS1) < (unsigned int)(IimmS)) RD = 1;
                    else RD = 0;
                    char instout[60]={'\0'};
                    sprintf(instout,"sltiu  %s , %s, %x       %x\n",RDN,RS1N,IimmU,inst_decoder.inst);
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
                    sprintf(instout,"ori  %s , %s, %x       %x\n",RDN,RS1N,IimmU,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B111:{
                    // andi rd, rs1, immediate  
                    // x[rd] = x[rs1] & sext(immediate)
                    RD = RS1 | IimmS;
                    char instout[60]={'\0'};
                    sprintf(instout,"andi  %s , %s, %x       %x\n",RDN,RS1N,IimmU,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B001:{
                    // slli rd, rs1, shamt   
                    // x[rd] = x[rs1] ≪ shamt
                    RD = RS1 << SHAMT;
                    char instout[60]={'\0'};
                    sprintf(instout,"slli  %s , %s, %x       %x\n",RDN,RS1N,SHAMT,inst_decoder.inst);
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
                            sprintf(instout,"srli  %s , %s, %x       %x\n",RDN,RS1N,SHAMT,inst_decoder.inst);
                            INSTLOG();
                            break;
                        }
                        case B0100000:{
                            // srai rd, rs1, shamt    
                            // x[rd] = (x[rs1] ≫s shamt)
                            RD = ((signed int)(RS1)) >> SHAMT;
                            char instout[60]={'\0'};
                            sprintf(instout,"srai  %s , %s, %x       %x\n",RDN,RS1N,SHAMT,inst_decoder.inst);
                            INSTLOG();
                            break;
                        }
                        default:{ 
                            printf("* * * * * illegal inst * * * * *\n");
                            break;
                        };
                    }
                    break;
                }
                default:{ 
                    printf("* * * * * illegal inst * * * * *\n");
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
                            sprintf(instout,"add  %s , %s , %s       %x\n",RDN,RS1N,RS2N,inst_decoder.inst);
                            INSTLOG();
                            break;
                        }
                        case B0100000:{
                            // sub rd, rs1, rs2    
                            // x[rd] = x[rs1] - x[rs2]
                            RD = RS1 - RS2;
                            char instout[60]={'\0'};
                            sprintf(instout,"sub  %s , %s , %s       %x\n",RDN,RS1N,RS2N,inst_decoder.inst);
                            INSTLOG();
                            break;
                        }
                        default:{ 
                            printf("* * * * * illegal inst * * * * *\n");
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
                    sprintf(instout,"sll  %s , %s , %s       %x\n",RDN,RS1N,RS2N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B010:{
                    // slt rd, rs1, rs2    
                    // x[rd] = (x[rs1] <s x[rs2])
                    if(((signed int)(RS1)) < ((signed int)(RS2))) RD = 1;
                    else RD = 0;
                    char instout[60]={'\0'};
                    sprintf(instout,"slt  %s , %s , %s       %x\n",RDN,RS1N,RS2N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B011:{
                    // sltu rd, rs1, rs2     
                    // x[rd] = (x[rs1] <u x[rs2])
                    if(((unsigned int)(RS1)) < ((unsigned int)(RS2))) RD = 1;
                    else RD = 0;
                    char instout[60]={'\0'};
                    sprintf(instout,"sltu  %s , %s , %s       %x\n",RDN,RS1N,RS2N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B100:{
                    // xor rd, rs1, rs2   
                    // x[rd] = x[rs1] ^ x[rs2]
                    RD = RS1 ^ RS2;
                    char instout[60]={'\0'};
                    sprintf(instout,"xor  %s , %s , %s       %x\n",RDN,RS1N,RS2N,inst_decoder.inst);
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
                            sprintf(instout,"srl  %s , %s , %s       %x\n",RDN,RS1N,RS2N,inst_decoder.inst);
                            INSTLOG();
                            break;
                        }
                        case B0100000:{
                            // sra rd, rs1, rs2    
                            // x[rd] = (x[rs1] ≫s x[rs2])
                            RD = ((signed int)(RS1)) >> (RS2 & 0x1f);
                            char instout[60]={'\0'};
                            sprintf(instout,"sra  %s , %s , %s       %x\n",RDN,RS1N,RS2N,inst_decoder.inst);
                            INSTLOG();
                            break;
                        }
                        default:{ 
                            printf("* * * * * illegal inst * * * * *\n");
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
                    sprintf(instout,"or  %s , %s , %s       %x\n",RDN,RS1N,RS2N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                case B111:{
                    // and rd, rs1, rs2   
                    // x[rd] = x[rs1] & x[rs2]
                    RD = RS1 & RS2;
                    char instout[60]={'\0'};
                    sprintf(instout,"and  %s , %s , %s       %x\n",RDN,RS1N,RS2N,inst_decoder.inst);
                    INSTLOG();
                    break;
                }
                default:{ 
                    printf("* * * * * illegal inst * * * * *\n");
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
                    printf("* * * * * illegal inst * * * * *\n");
                    break;
                };
    }
    










    cpulog();
    

}
