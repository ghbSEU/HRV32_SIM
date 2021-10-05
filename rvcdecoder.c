#include<stdio.h>
#include "rvcdecoder.h"

short int rvcinst[RVC_NUM];
short int rvcinst_mask[RVC_NUM];

void rvc_init(){
                                //   15-12 11-8   7-4   3-0
                                //    000x  0000  0xxxx xx01
    rvcinst[c_nop]              = CN(B0000,B0000,B0000,B0001);
    rvcinst_mask[c_nop]         = CN(B1110,B1111,B1000,B0011);
    // c_nop must before c_addi //    000x  xxxx  xxxx  xx01
    rvcinst[c_addi]             = CN(B0000,B0000,B0000,B0001);
    rvcinst_mask[c_addi]        = CN(B1110,B0000,B0000,B0011);
                                //    001x  xxxx  xxxx  xx01
    rvcinst[c_jal]              = CN(B0010,B0000,B0000,B0001);
    rvcinst_mask[c_jal]         = CN(B1110,B0000,B0000,B0011);
                                //    010x  xxxx  xxxx  xx01
    rvcinst[c_li]               = CN(B0100,B0000,B0000,B0001);
    rvcinst_mask[c_li]          = CN(B1110,B0000,B0000,B0011);
                                //    011x  0001  0xxx  xx01
    rvcinst[c_addi16sp]         = CN(B0110,B0001,B0000,B0001);
    rvcinst_mask[c_addi16sp]    = CN(B1110,B1111,B1000,B0011);
                                //    011x  xxxx  xxxx  xx01
    rvcinst[c_lui]              = CN(B0110,B0000,B0000,B0001);
    rvcinst_mask[c_lui]         = CN(B1110,B0000,B0000,B0011);
                                //    100x  00xx  xxxx  xx01
    rvcinst[c_srli]             = CN(B1000,B0000,B0000,B0001);
    rvcinst_mask[c_srli]        = CN(B1110,B1100,B0000,B0011);
                                //    100x  01xx  xxxx  xx01
    rvcinst[c_srai]             = CN(B1000,B0100,B0000,B0001);
    rvcinst_mask[c_srai]        = CN(B1110,B1100,B0000,B0011);
                                //    100x  10xx  xxxx  xx01
    rvcinst[c_andi]             = CN(B1000,B1000,B0000,B0001);
    rvcinst_mask[c_andi]        = CN(B1110,B1100,B0000,B0011);
                                //    1000  11xx  x00x  xx01
    rvcinst[c_sub]              = CN(B1000,B1100,B0000,B0001);
    rvcinst_mask[c_sub]         = CN(B1111,B1100,B0110,B0011);
                                //    1000  11xx  x01x  xx01
    rvcinst[c_xor]              = CN(B1000,B1100,B0010,B0001);
    rvcinst_mask[c_xor]         = CN(B1111,B1100,B0110,B0011);
                                //    1000  11xx  x10x  xx01
    rvcinst[c_or]               = CN(B0000,B0000,B0100,B0001);
    rvcinst_mask[c_or]          = CN(B1111,B1100,B0110,B0011);
                                //    1000  11xx  x11x  xx01
    rvcinst[c_and]              = CN(B0000,B0000,B0110,B0001);
    rvcinst_mask[c_and]         = CN(B1111,B1100,B0110,B0011);
                                //    101x  xxxx  xxxx  xx01
    rvcinst[c_j]                = CN(B1010,B0000,B0000,B0001);
    rvcinst_mask[c_j]           = CN(B1110,B0000,B0000,B0011);
                                //    110x  xxxx  xxxx  xx01
    rvcinst[c_beqz]             = CN(B1100,B0000,B0000,B0001);
    rvcinst_mask[c_beqz]        = CN(B1110,B0000,B0000,B0011);
                                //    111x  xxxx  xxxx  xx01
    rvcinst[c_bnez]             = CN(B1110,B0000,B0000,B0001);
    rvcinst_mask[c_bnez]        = CN(B1110,B0000,B0000,B0011);
/********************************************************************/
    rvcinst[c_illegal]          = CN(B0000,B0000,B0000,B0000);
    rvcinst_mask[c_illegal]     = CN(B1111,B1111,B1111,B1111);
    // c_illegal must before c_addi4spn
                                //    000x  xxxx  xxxx  xx00  
    rvcinst[c_addi4spn]         = CN(B0000,B0000,B0000,B0000);
    rvcinst_mask[c_addi4spn]    = CN(B1110,B0000,B0000,B0011);
                                //    010x  xxxx  xxxx  xx00  
    rvcinst[c_lw]               = CN(B0000,B0000,B0000,B0000);
    rvcinst_mask[c_lw]          = CN(B1110,B0000,B0000,B0011);
                                //    110x  xxxx  xxxx  xx00  
    rvcinst[c_sw]               = CN(B0000,B0000,B0000,B0000);
    rvcinst_mask[c_sw]          = CN(B1110,B0000,B0000,B0011);
/********************************************************************/
                                //    000x  xxxx  xxxx  xx10  
    rvcinst[c_slli]             = CN(B0000,B0000,B0000,B0010);
    rvcinst_mask[c_slli]        = CN(B1110,B0000,B0000,B0011);
                                //    010x  xxxx  xxxx  xx10 
    rvcinst[c_lwsp]             = CN(B0100,B0000,B0000,B0010);
    rvcinst_mask[c_lwsp]        = CN(B1110,B0000,B0000,B0011);
                                //    1000  xxxx  x000  0010 
    rvcinst[c_jr]               = CN(B1000,B0000,B0000,B0010);
    rvcinst_mask[c_jr]          = CN(B1111,B0000,B0111,B1111);
                                //    1000  xxxx  xxxx  xx10
    rvcinst[c_mv]               = CN(B1000,B0000,B0000,B0010);
    rvcinst_mask[c_mv]          = CN(B1111,B0000,B0000,B0011);
                                //    1001  0000  0000  0010
    rvcinst[c_ebreak]           = CN(B1001,B0000,B0000,B0010);
    rvcinst_mask[c_ebreak]      = CN(B1111,B1111,B1111,B1111);
                                //    1001  xxxx  x000  0010  
    rvcinst[c_jalr]             = CN(B1001,B0000,B0000,B0010);
    rvcinst_mask[c_jalr]        = CN(B1111,B0000,B0111,B1111);
                                //    1001  xxxx  xxxx  xx10
    rvcinst[c_add]              = CN(B1001,B0000,B0000,B0010);
    rvcinst_mask[c_add]         = CN(B1111,B0000,B0000,B0011);
                                //    110x  xxxx  xxxx  xx10 
    rvcinst[c_swsp]             = CN(B1100,B0000,B0000,B0010);
    rvcinst_mask[c_swsp]        = CN(B1110,B0000,B0000,B0011);
}