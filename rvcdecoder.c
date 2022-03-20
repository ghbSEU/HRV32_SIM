#include<stdio.h>
#include "rvcdecoder.h"



unsigned short int rvcinst[RVC_NUM];
unsigned short int rvcinst_mask[RVC_NUM];
void (*rvc_ex_fun[RVC_NUM])(unsigned short int);


#define BIT4_2 CN(B0000,B0000,B0001,B1100)
#define BIT6_2 CN(B0000,B0000,B0111,B1100)
#define BIT9_7 CN(B0000,B0011,B1000,B0000)
#define BI11_7 CN(B0000,B1111,B1000,B0000)

#define RS1_9_7(cinst) ((BIT9_7&cinst)>>7)
#define RS1_11_7(cinst) ((BIT11_7&cinst)>>7)

#define RS2_4_2(cinst) ((BIT4_2&cinst)>>2)
#define RS2_6_2(cinst) ((BIT6_2&cinst)>>2)

#define RD1_1_7(cinst) ((BIT11_7&cinst)>>7)
#define RD_4_2(cinst) ((BIT4_2&cinst)>>2)
#define RD_9_7(cinst) ((BIT9_7&cinst)>>7)

#define RS1 rv_reg[inst_decoder.COMRVtype.rs1]
#define RS2 rv_reg[inst_decoder.COMRVtype.rs2]
#define RD  rv_reg[inst_decoder.COMRVtype.rd]

////////////////////////////////////////////////////////////////////////////////                                                  
//   ##                                                                       //
//   ##                                                                       // 
//   ##    ######### ######     ########  ######                              //  
//   ##    ###    ###     ##    ##     ###    ###                             //
//   ##    ##     ##      ##    ##     ##      ##                             //
//   ##    ##     ##      ##    ##     ##      ##                             //
//   ##    ##     ##      ##    ##     ##      ##                             //
//   ##    ##     ##      ##    ##     ##      ##                             //
////////////////////////////////////////////////////////////////////////////////


#define BIT1 CN(B0000,B0000,B0000,B0010)
#define BIT2 CN(B0000,B0000,B0000,B0100)
#define BIT3 CN(B0000,B0000,B0000,B1000)
#define BIT4 CN(B0000,B0000,B0001,B0000)
#define BIT5 CN(B0000,B0000,B0010,B0000)
#define BIT6 CN(B0000,B0000,B0100,B0000)
#define BIT7 CN(B0000,B0000,B1000,B0000)
#define BIT8 CN(B0000,B0001,B0000,B0000)
#define BIT9 CN(B0000,B0010,B0000,B0000)
#define BIT10 CN(B0000,B0100,B0000,B0000)
#define BIT11 CN(B0000,B1000,B0000,B0000)
#define BIT12 CN(B0001,B0000,B0000,B0000)
#define BIT10_7 CN(B0000,B0111,B1000,B0000)
#define BIT12_10 CN(B0001,B1100,B1000,B0000)
#define BIT12_9 CN(B0001,B1110,B1000,B0000)


//  12          5
//  6-2         4-0
#define imm1(cinst) (((BIT12&cinst)>>(12-5))\                 
+((BIT6_2&cinst)>>2))                      
//  12          11
//  8           10
//  10-9        9-8
//  6           7
//  7           6
//  2           5
//  11          4
//  5-3         3-1
#define imm2(cinst) (((BIT12&cinst)>>(12-11))\
+((BIT8&cinst)<<(10-8))\
+(((BIT10|BIT9)&cinst)>>(9-8))\
+((BIT6&cinst)<<(7-6))\
+((BIT7&cinst)>>(7-6))\
+((BIT2&cinst)<<(5-2))\
+((BIT11&cinst)>>(11-4))\
+(((BIT5|BIT4|BIT3)&cinst)>>(3-1)))
//  12          9
//  4-3         8-7
//  5           6
//  2           5
//  6           4
#define imm3(cinst) (((BIT12&cinst)>>(12-9))\
+(((BIT4|BIT3)&cinst)<<(7-3))\
+((BIT5&cinst)<<(6-5))\
+((BIT2&cinst)<<(5-2))\
+((BIT6&cinst)>>(6-4)))
//  12          17
//  6-2         16-12
#define imm4(cinst) (((BIT12&cinst)<<(17-12))\
+((BIT6_2&cinst)<<(12-2)))
//  12          8
//  6-5         7-6
//  2           5
//  11-10       4-3
//  4-3         2-1
#define imm5(cinst) (((BIT12&cinst)>>(12-8))\
+(((BIT6|BIT5)&cinst)<<(6-5))\
+((BIT2&cinst)<<(5-2))\
+(((BIT11|BIT10)&cinst)>>(10-3))\
+(((BIT4|BIT3)&cinst)>>(3-1)))
//  10-7    9-6
//  12-11   5-4
//  5       3
//  6       2
#define imm6(cinst) (((BIT10_7&cinst)>>(7-6))\
+(((BIT12|BIT11)&cinst)>>(11-4))\
+((BIT5&cinst)>>(5-3))\
+((BIT6&cinst)>>(6-2)))
//  6-5     7-6
//  12-10   5-3
#define imm7(cinst) ((((BIT6|BIT5)&cinst)<<(6-5))\
+((BIT12_10&cinst)>>(10-3)))
//  5       6
//  12-10   5-3
//  6       2
#define imm8(cinst) ((((BIT5)&cinst)<<(6-5))\
+((BIT12_10&cinst)>>(10-3))\
+((BIT6&cinst)>>(6-2)))
//  4-2     8-6
//  12      5
//  6-5     4-3
#define imm9(cinst) ((((BIT4|BIT3|BIT2)&cinst)<<(6-2))\
+((BIT12&cinst)>>(12-5))\
+(((BIT6|BIT5)&cinst)>>(5-3)))
//  3-2     7-6
//  12      5
//  6-4     4-2
#define imm10(cinst) ((((BIT3|BIT2)&cinst)<<(6-2))\
+((BIT12&cinst)>>(12-5))\
+(((BIT6|BIT5|BIT4)&cinst)>>(4-2)))
//  9-7     8-6
//  12-10   5-3
#define imm11(cinst) ((((BIT9|BIT8|BIT7)&cinst)>>(7-6))\
+((BIT12_10&cinst)>>(10-3)))
//  8-7     7-6
//  12-9    5-2
#define imm12(cinst) ((((BIT8|BIT7)&cinst)>>(7-6))\
+((BIT12_9&cinst)>>(9-2)))





#define BIT4_2 CN(B0000,B0000,B0001,B1100)
#define BIT6_2 CN(B0000,B0000,B0111,B1100)
#define BIT9_7 CN(B0000,B0011,B1000,B0000)
#define BIT11_7 CN(B0000,B1111,B1000,B0000)

#define RS1_9_7(cinst) ((BIT9_7&cinst)>>7)
#define RS1_11_7(cinst) ((BIT11_7&cinst)>>7)

#define RS2_4_2(cinst) ((BIT4_2&cinst)>>2)
#define RS2_6_2(cinst) ((BIT6_2&cinst)>>2)

#define RD_11_7(cinst) ((BIT11_7&cinst)>>7)
#define RD_4_2(cinst) ((BIT4_2&cinst)>>2)
#define RD_9_7(cinst) ((BIT9_7&cinst)>>7)

#define RS1_E_9_7 rv_reg[RS1_9_7(c_inst_ex)+8]
#define RS1_E_11_7 rv_reg[RS1_11_7(c_inst_ex)]

#define RS2_E_4_2 rv_reg[RS2_4_2(c_inst_ex)+8]
#define RS2_E_6_2 rv_reg[RS2_6_2(c_inst_ex)]

#define RD_E_4_2  rv_reg[RD_4_2(c_inst_ex)+8]
#define RD_E_9_7  rv_reg[RD_9_7(c_inst_ex)+8]
#define RD_E_11_7  rv_reg[RD_11_7(c_inst_ex)]

#define RS1_N_9_7 reg_name[RS1_9_7(c_inst_ex)+8]
#define RS1_N_11_7 reg_name[RS1_11_7(c_inst_ex)]

#define RS2_N_4_2 reg_name[RS2_4_2(c_inst_ex)+8]
#define RS2_N_6_2 reg_name[RS2_6_2(c_inst_ex)]

#define RD_N_4_2  reg_name[RD_4_2(c_inst_ex)+8]
#define RD_N_9_7  reg_name[RD_9_7(c_inst_ex)+8]
#define RD_N_11_7  reg_name[RD_11_7(c_inst_ex)]


#define SImm1 sext(imm1(c_inst_ex),5+1)
#define UImm1 imm1(c_inst_ex)
#define SImm2 sext(imm2(c_inst_ex),11+1)
#define UImm2 imm2(c_inst_ex)
#define SImm3 sext(imm3(c_inst_ex),9+1)
#define UImm3 imm3(c_inst_ex)
#define SImm4 sext(imm4(c_inst_ex),17+1)
#define UImm4 imm4(c_inst_ex)
#define SImm5 sext(imm5(c_inst_ex),8+1)
#define UImm5 imm5(c_inst_ex)
#define SImm6 sext(imm6(c_inst_ex),9+1)
#define UImm6 imm6(c_inst_ex)
#define SImm8 sext(imm8(c_inst_ex),6+1)
#define UImm8 imm8(c_inst_ex)
#define SImm10 sext(imm10(c_inst_ex),7+1)
#define UImm10 imm10(c_inst_ex)
#define SImm12 sext(imm12(c_inst_ex),7+1)
#define UImm12 imm12(c_inst_ex)





void c_nop_ex(unsigned short int c_inst_ex){
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_nop            %x\t\n",pc_reg,c_inst_ex);
    INSTLOG();
}

void c_addi_ex(unsigned short int c_inst_ex){
    /*
    printf("PC %x\n",pc_reg);
    printf("RD_E_11_7 %x\n",RD_E_11_7);
    printf("imm1 %x\n",imm1(c_inst_ex));
    printf("SImm1 = %x\n",SImm1);
    */
    RD_E_11_7 = RD_E_11_7 + SImm1;
    /*
    printf("RD_E_11_7 %x\n\n",RD_E_11_7);
    */
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_addi %s\t  %d\t  %x\t\n",pc_reg,RD_N_11_7,SImm1,c_inst_ex);
    INSTLOG();
}

void c_jal_ex(unsigned short int c_inst_ex){
    rv_reg[1] = pc_reg + 2;
    pc_reg_next = pc_reg + SImm2;
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_jal %s\t  %x\t  %x\t\n",pc_reg,reg_name[1],UImm2,c_inst_ex);
    INSTLOG();
}

void c_li_ex(unsigned short int c_inst_ex){
    RD_E_11_7 = SImm1;
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_li %s\t  %x\t  %x\t\n",pc_reg,RD_N_11_7,UImm1,c_inst_ex);
    INSTLOG();
}

void c_addi16sp_ex(unsigned short int c_inst_ex){
    rv_reg[2] +=  SImm3;
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_addi16sp   %x\t  %x\t\n",pc_reg,UImm3,c_inst_ex);
    INSTLOG();
}

void c_lui_ex(unsigned short int c_inst_ex){
    RD_E_11_7 = SImm4;
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_lui  %s\t  %x\t  %x\t\n",pc_reg,RD_N_11_7,UImm4,c_inst_ex);
    INSTLOG();
}

void c_srli_ex(unsigned short int c_inst_ex){
    RD_E_9_7 = (unsigned int)(RD_E_9_7) >> UImm1;
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_srli  %s\t  %x\t  %x\t\n",pc_reg,RD_N_9_7,UImm1,c_inst_ex);
    INSTLOG();
}

void c_srai_ex(unsigned short int c_inst_ex){
    RD_E_9_7 = (signed int )(RD_E_9_7) >> UImm1;
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_srli  %s\t  %x\t  %x\t\n",pc_reg,RD_N_9_7,UImm1,c_inst_ex);
    INSTLOG();
}

void c_andi_ex(unsigned short int c_inst_ex){
    RD_E_9_7 = RD_E_9_7 & UImm1;
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_andi  %s\t  %x\t  %x\t\n",pc_reg,RD_N_9_7,UImm1,c_inst_ex);
    INSTLOG();
}


void c_sub_ex(unsigned short int c_inst_ex){
    RD_E_9_7 = RD_E_9_7 - RS2_E_4_2;
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_sub  %s\t  %s\t  %x\t\n",pc_reg,RD_N_9_7,RS2_N_4_2,c_inst_ex);
    INSTLOG();
}

void c_xor_ex(unsigned short int c_inst_ex){
    RD_E_9_7 = RD_E_9_7 ^ RS2_E_4_2;
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_xor  %s\t  %s\t  %x\t\n",pc_reg,RD_N_9_7,RS2_N_4_2,c_inst_ex);
    INSTLOG();
}

void c_or_ex(unsigned short int c_inst_ex){
    RD_E_9_7 = RD_E_9_7 | RS2_E_4_2;
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_or  %s\t  %s\t  %x\t\n",pc_reg,RD_N_9_7,RS2_N_4_2,c_inst_ex);
    INSTLOG();
}

void c_and_ex(unsigned short int c_inst_ex){
    RD_E_9_7 = RD_E_9_7 & RS2_E_4_2;
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_and  %s\t  %s\t  %x\t\n",pc_reg,RD_N_9_7,RS2_N_4_2,c_inst_ex);
    INSTLOG();
}

void c_j_ex(unsigned short int c_inst_ex){
    pc_reg_next = pc_reg + SImm2;
    char instout[60]={'\0'};
    #ifdef DEBUG
    printf("SImm2 = %x\t",SImm2);
    #endif
    sprintf(instout,"pc:    %x\t c_j %s\t  %x\t  %x\t\n",pc_reg,reg_name[1],UImm2,c_inst_ex);
    INSTLOG();
}

void c_beqz_ex(unsigned short int c_inst_ex){
    if(RS1_E_9_7 == 0){
        pc_reg_next = pc_reg + SImm5;
    }
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_beqz  %s\t  %s\t  %x\t\n",pc_reg,RD_N_9_7,RS2_N_4_2,c_inst_ex);
    INSTLOG();
}

void c_bnez_ex(unsigned short int c_inst_ex){
    if(RS1_E_9_7 != 0){
        pc_reg_next = pc_reg + SImm5;
    }
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_bnez  %s\t  %s\t  %x\t\n",pc_reg,RD_N_9_7,RS2_N_4_2,c_inst_ex);
    INSTLOG();
}

void c_illegal_ex(unsigned short int c_inst_ex){
    #ifdef DEBUG
    printf("* * * * * illegal inst * * * * *\n");
    #endif
}

void c_addi4spn_ex(unsigned short int c_inst_ex){
    RD_E_4_2 = rv_reg[2] + UImm6;
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_addi4spn  %s\t  %x\t  %x\t\n",pc_reg,RD_N_4_2,UImm6,c_inst_ex);
    INSTLOG();
}

void c_lw_ex(unsigned short int c_inst_ex){
    

    if(( (RS1_E_9_7 + UImm8) >= 0 ) && ( (RS1_E_9_7 + UImm8) <= 0x10000000 ) )
    {
       RD_E_4_2 = *((signed int *)(&(((char *)(imem))[(RS1_E_9_7 + UImm8) ])));
    }
    else if(( (RS1_E_9_7 + UImm8) >= 0x10000000 ) && ( (RS1_E_9_7 + UImm8) <= 0x20000000 ) )
    {
        RD_E_4_2 = *((signed int *)(&(((char *)(dmem))[(RS1_E_9_7 + UImm8)-0x10000000 ])));
    }



    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_lw  %s\t  %d(%s\t)  %x\t\n",pc_reg,RD_N_4_2,SImm8,RS1_N_9_7,c_inst_ex);
    INSTLOG();
}

void c_sw_ex(unsigned short int c_inst_ex){
    

    if(( (RS1_E_9_7 + UImm8) >= 0 ) && ( (RS1_E_9_7 + UImm8) <= 0x10000000 ) )
    {
       *((signed int *)(&(((char *)(imem))[(RS1_E_9_7 + UImm8) ]))) = RS2_E_4_2;
    }
    else if(( (RS1_E_9_7 + UImm8) >= 0x10000000 ) && ( (RS1_E_9_7 + UImm8) <= 0x20000000 ) )
    {
        *((signed int *)(&(((char *)(dmem))[(RS1_E_9_7 + UImm8)-0x10000000 ]))) = RS2_E_4_2;
    }



    wdmemaddr = (RS1_E_9_7 + UImm8) ;
    wdmemdata =  RS2_E_4_2 ;
    wdmemflag = 1 ;

    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_sw  %s\t  %d(%s\t)  %x\t\n",pc_reg,RS2_N_4_2,SImm8,RS1_N_9_7,c_inst_ex);
    INSTLOG();
}

void c_slli_ex(unsigned short int c_inst_ex){
    RD_E_11_7 = RD_E_11_7 << UImm1;
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_slli  %s\t  %x\t  %x\t\n",pc_reg,RD_N_11_7,UImm1,c_inst_ex);
    INSTLOG();
}

void c_lwsp_ex(unsigned short int c_inst_ex){
    

    if(( (rv_reg[2] + UImm10) >= 0 ) && ( (rv_reg[2] + UImm10) <= 0x10000000 ) )
    {
       RD_E_11_7 = *((signed int *)(&(((char *)(imem))[(rv_reg[2] + UImm10) ])));
    }
    else if(( (rv_reg[2] + UImm10) >= 0x10000000 ) && ( (rv_reg[2] + UImm10) <= 0x20000000 ) )
    {
        RD_E_11_7 = *((signed int *)(&(((char *)(dmem))[(rv_reg[2] + UImm10) - 0x10000000 ])));
    }

    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_lwsp  %s\t  %d(x2)  %x\t\n",pc_reg,RD_N_11_7,SImm10,c_inst_ex);
    INSTLOG();
}

void c_jr_ex(unsigned short int c_inst_ex){
    pc_reg_next = RS1_E_11_7;
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_jr %s\t    %x\t\n",pc_reg,RS1_N_11_7,c_inst_ex);
    INSTLOG();
}

void c_mv_ex(unsigned short int c_inst_ex){
    RD_E_11_7 = RS2_E_6_2;
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_mv %s\t  %s\t  %x\t\n",pc_reg,RD_N_11_7,RS2_N_6_2,c_inst_ex);
    INSTLOG();
}

void c_ebreak_ex(unsigned short int c_inst_ex){
    
}

void c_jalr_ex(unsigned short int c_inst_ex){
    rv_reg[1] = pc_reg + 2;
    pc_reg_next = RS1_E_11_7;
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_jalr %s\t    %x\t\n",pc_reg,RS1_N_11_7,c_inst_ex);
    INSTLOG();
}

void c_add_ex(unsigned short int c_inst_ex){
    RD_E_11_7 += RS2_E_6_2;
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_add %s\t  %s\t  %x\t\n",pc_reg,RD_N_11_7,RS2_N_6_2,c_inst_ex);
    INSTLOG();
}

void c_swsp_ex(unsigned short int c_inst_ex){
    


     if(( (rv_reg[2] + UImm12) >= 0 ) && ( (rv_reg[2] + UImm12) <= 0x10000000 ) )
    {
       *((signed int *)(&(((char *)(imem))[(rv_reg[2] + UImm12)]))) = RS2_E_6_2;
    }
    else if(( (rv_reg[2] + UImm12) >= 0x10000000 ) && ( (rv_reg[2] + UImm12) <= 0x20000000 ) )
    {
        *((signed int *)(&(((char *)(dmem))[(rv_reg[2] + UImm12)-0x10000000]))) = RS2_E_6_2;
    }



    wdmemaddr = (rv_reg[2] + UImm12) ;
    wdmemdata =  RS2_E_6_2 ;
    wdmemflag = 1 ;
    char instout[60]={'\0'};
    sprintf(instout,"pc:    %x\t c_swsp %s\t  %d(x2)  %x\t\n",pc_reg,RS2_N_6_2,SImm12,c_inst_ex);
    INSTLOG();
}

void rvc_init(){
                                //   15-12 11-8   7-4   3-0
                                //    000x  0000  0xxxx xx01
    rvcinst[c_nop]              = CN(B0000,B0000,B0000,B0001);
    rvcinst_mask[c_nop]         = CN(B1110,B1111,B1000,B0011);
    rvc_ex_fun[c_nop] = c_nop_ex;
    // c_nop must before c_addi //    000x  xxxx  xxxx  xx01
    rvcinst[c_addi]             = CN(B0000,B0000,B0000,B0001);
    rvcinst_mask[c_addi]        = CN(B1110,B0000,B0000,B0011);
    rvc_ex_fun[c_addi] = c_addi_ex;
                                //    001x  xxxx  xxxx  xx01
    rvcinst[c_jal]              = CN(B0010,B0000,B0000,B0001);
    rvcinst_mask[c_jal]         = CN(B1110,B0000,B0000,B0011);
    rvc_ex_fun[c_jal] = c_jal_ex;
                                //    010x  xxxx  xxxx  xx01
    rvcinst[c_li]               = CN(B0100,B0000,B0000,B0001);
    rvcinst_mask[c_li]          = CN(B1110,B0000,B0000,B0011);
    rvc_ex_fun[c_li] = c_li_ex;
                                //    011x  0001  0xxx  xx01
    rvcinst[c_addi16sp]         = CN(B0110,B0001,B0000,B0001);
    rvcinst_mask[c_addi16sp]    = CN(B1110,B1111,B1000,B0011);
    rvc_ex_fun[c_addi16sp] = c_addi16sp_ex;
                                //    011x  xxxx  xxxx  xx01
    rvcinst[c_lui]              = CN(B0110,B0000,B0000,B0001);
    rvcinst_mask[c_lui]         = CN(B1110,B0000,B0000,B0011);
    rvc_ex_fun[c_lui] = c_lui_ex;
                                //    100x  00xx  xxxx  xx01
    rvcinst[c_srli]             = CN(B1000,B0000,B0000,B0001);
    rvcinst_mask[c_srli]        = CN(B1110,B1100,B0000,B0011);
    rvc_ex_fun[c_srli] = c_srli_ex;
                                //    100x  01xx  xxxx  xx01
    rvcinst[c_srai]             = CN(B1000,B0100,B0000,B0001);
    rvcinst_mask[c_srai]        = CN(B1110,B1100,B0000,B0011);
    rvc_ex_fun[c_srai] = c_srai_ex;
                                //    100x  10xx  xxxx  xx01
    rvcinst[c_andi]             = CN(B1000,B1000,B0000,B0001);
    rvcinst_mask[c_andi]        = CN(B1110,B1100,B0000,B0011);
    rvc_ex_fun[c_andi] = c_andi_ex;
                                //    1000  11xx  x00x  xx01
    rvcinst[c_sub]              = CN(B1000,B1100,B0000,B0001);
    rvcinst_mask[c_sub]         = CN(B1111,B1100,B0110,B0011);
    rvc_ex_fun[c_sub] = c_sub_ex;
                                //    1000  11xx  x01x  xx01
    rvcinst[c_xor]              = CN(B1000,B1100,B0010,B0001);
    rvcinst_mask[c_xor]         = CN(B1111,B1100,B0110,B0011);
    rvc_ex_fun[c_xor] = c_xor_ex;
                                //    1000  11xx  x10x  xx01
    rvcinst[c_or]               = CN(B1000,B1100,B0100,B0001);
    rvcinst_mask[c_or]          = CN(B1111,B1100,B0110,B0011);
    rvc_ex_fun[c_or] = c_or_ex;
                                //    1000  11xx  x11x  xx01
    rvcinst[c_and]              = CN(B1000,B1100,B0110,B0001);
    rvcinst_mask[c_and]         = CN(B1111,B1100,B0110,B0011);
    rvc_ex_fun[c_and] = c_and_ex;
                                //    101x  xxxx  xxxx  xx01
    rvcinst[c_j]                = CN(B1010,B0000,B0000,B0001);
    rvcinst_mask[c_j]           = CN(B1110,B0000,B0000,B0011);
    rvc_ex_fun[c_j] = c_j_ex;
                                //    110x  xxxx  xxxx  xx01
    rvcinst[c_beqz]             = CN(B1100,B0000,B0000,B0001);
    rvcinst_mask[c_beqz]        = CN(B1110,B0000,B0000,B0011);
    rvc_ex_fun[c_beqz] = c_beqz_ex;
                                //    111x  xxxx  xxxx  xx01
    rvcinst[c_bnez]             = CN(B1110,B0000,B0000,B0001);
    rvcinst_mask[c_bnez]        = CN(B1110,B0000,B0000,B0011);
    rvc_ex_fun[c_bnez] = c_bnez_ex;
/********************************************************************/
    rvcinst[c_illegal]          = CN(B0000,B0000,B0000,B0000);
    rvcinst_mask[c_illegal]     = CN(B1111,B1111,B1111,B1111);
    rvc_ex_fun[c_illegal] = c_illegal_ex;
    // c_illegal must before c_addi4spn
                                //    000x  xxxx  xxxx  xx00  
    rvcinst[c_addi4spn]         = CN(B0000,B0000,B0000,B0000);
    rvcinst_mask[c_addi4spn]    = CN(B1110,B0000,B0000,B0011);
    rvc_ex_fun[c_addi4spn] = c_addi4spn_ex;
                                //    010x  xxxx  xxxx  xx00  
    rvcinst[c_lw]               = CN(B0100,B0000,B0000,B0000);
    rvcinst_mask[c_lw]          = CN(B1110,B0000,B0000,B0011);
    rvc_ex_fun[c_lw] = c_lw_ex;
                                //    110x  xxxx  xxxx  xx00  
    rvcinst[c_sw]               = CN(B1100,B0000,B0000,B0000);
    rvcinst_mask[c_sw]          = CN(B1110,B0000,B0000,B0011);
    rvc_ex_fun[c_sw] = c_sw_ex;
/********************************************************************/
                                //    000x  xxxx  xxxx  xx10  
    rvcinst[c_slli]             = CN(B0000,B0000,B0000,B0010);
    rvcinst_mask[c_slli]        = CN(B1110,B0000,B0000,B0011);
    rvc_ex_fun[c_slli] = c_slli_ex;
                                //    010x  xxxx  xxxx  xx10 
    rvcinst[c_lwsp]             = CN(B0100,B0000,B0000,B0010);
    rvcinst_mask[c_lwsp]        = CN(B1110,B0000,B0000,B0011);
    rvc_ex_fun[c_lwsp] = c_lwsp_ex;
                                //    1000  xxxx  x000  0010 
    rvcinst[c_jr]               = CN(B1000,B0000,B0000,B0010);
    rvcinst_mask[c_jr]          = CN(B1111,B0000,B0111,B1111);
    rvc_ex_fun[c_jr] = c_jr_ex;
                                //    1000  xxxx  xxxx  xx10
    rvcinst[c_mv]               = CN(B1000,B0000,B0000,B0010);
    rvcinst_mask[c_mv]          = CN(B1111,B0000,B0000,B0011);
    rvc_ex_fun[c_mv] = c_mv_ex;
                                //    1001  0000  0000  0010
    rvcinst[c_ebreak]           = CN(B1001,B0000,B0000,B0010);
    rvcinst_mask[c_ebreak]      = CN(B1111,B1111,B1111,B1111);
    rvc_ex_fun[c_ebreak] = c_ebreak_ex;
                                //    1001  xxxx  x000  0010  
    rvcinst[c_jalr]             = CN(B1001,B0000,B0000,B0010);
    rvcinst_mask[c_jalr]        = CN(B1111,B0000,B0111,B1111);
    rvc_ex_fun[c_jalr] = c_jalr_ex;
                                //    1001  xxxx  xxxx  xx10
    rvcinst[c_add]              = CN(B1001,B0000,B0000,B0010);
    rvcinst_mask[c_add]         = CN(B1111,B0000,B0000,B0011);
    rvc_ex_fun[c_add] = c_add_ex;
                                //    110x  xxxx  xxxx  xx10 
    rvcinst[c_swsp]             = CN(B1100,B0000,B0000,B0010);
    rvcinst_mask[c_swsp]        = CN(B1110,B0000,B0000,B0011);
    rvc_ex_fun[c_swsp] = c_swsp_ex;
}



void rvc_dec_ex(unsigned short int inst){
    int rvc_inst = 0xffffffff;
    for(int i = 0; i < RVC_NUM; i++){
        if(( inst & rvcinst_mask[i] ) == rvcinst[i]) {
            rvc_inst = i;
            break;
        }
    }
    if(rvc_inst == 0xffffffff){
        #ifdef DEBUG
        printf("* * * * * illegal inst * * * * *\n");
        #endif
    }
    else{
        rvc_ex_fun[rvc_inst](inst);
    }
}

