#ifndef _RVCDECODER_H_
#define _RVCDECODER_H_

#define RVC_NUM 28
enum rvcinst{c_nop=0,
             c_addi,
             c_jal,
             c_li,
             c_addi16sp,
             c_lui,
             c_srli,
             c_srai,
             c_andi,
             c_sub,
             c_xor,
             c_or,
             c_and,
             c_j,
             c_beqz,
             c_bnez,
             c_illegal,
             c_addi4spn,
             c_lw,
             c_sw,
             c_slli,
             c_lwsp,
             c_jr,
             c_mv,
             c_ebreak,
             c_jalr,
             c_add,
             c_swsp};

#define B0000 0x0
#define B0001 0x1
#define B0010 0x2
#define B0011 0x3
#define B0100 0x4
#define B0101 0x5
#define B0110 0x6
#define B0111 0x7
#define B1000 0x8
#define B1001 0x9
#define B1010 0xA
#define B1011 0xB
#define B1100 0xC
#define B1101 0xD
#define B1110 0xE
#define B1111 0xF

#define CN(A,B,C,D) A*16*16*16+B*16*16+C*16+D


#endif