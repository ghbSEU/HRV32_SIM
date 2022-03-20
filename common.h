#ifndef _COMMON_H_
#define _COMMON_H_

#include "config.h"

#define B000 0
#define B001 1
#define B010 2
#define B011 3
#define B100 4
#define B101 5
#define B110 6
#define B111 7
#define B0000000 0x00
#define B0000001 0x01
#define B0000010 0x02
#define B0000011 0x03
#define B0000100 0x04
#define B0000101 0x05
#define B0000110 0x06
#define B0000111 0x07
#define B0001000 0x08
#define B0001001 0x09
#define B0001010 0x0A
#define B0001011 0x0B
#define B0001100 0x0C
#define B0001101 0x0D
#define B0001110 0x0E
#define B0001111 0x0F
#define B0010000 0x10
#define B0010001 0x11
#define B0010010 0x12
#define B0010011 0x13
#define B0010100 0x14
#define B0010101 0x15
#define B0010110 0x16
#define B0010111 0x17
#define B0011000 0x18
#define B0011001 0x19
#define B0011010 0x1A
#define B0011011 0x1B
#define B0011100 0x1C
#define B0011101 0x1D
#define B0011110 0x1E
#define B0011111 0x1F
#define B0100000 0x20
#define B0100001 0x21
#define B0100010 0x22
#define B0100011 0x23
#define B0100100 0x24
#define B0100101 0x25
#define B0100110 0x26
#define B0100111 0x27
#define B0101000 0x28
#define B0101001 0x29
#define B0101010 0x2A
#define B0101011 0x2B
#define B0101100 0x2C
#define B0101101 0x2D
#define B0101110 0x2E
#define B0101111 0x2F
#define B0110000 0x30
#define B0110001 0x31
#define B0110010 0x32
#define B0110011 0x33
#define B0110100 0x34
#define B0110101 0x35
#define B0110110 0x36
#define B0110111 0x37
#define B0111000 0x38
#define B0111001 0x39
#define B0111010 0x3A
#define B0111011 0x3B
#define B0111100 0x3C
#define B0111101 0x3D
#define B0111110 0x3E
#define B0111111 0x3F
#define B1000000 0x40
#define B1000001 0x41
#define B1000010 0x42
#define B1000011 0x43
#define B1000100 0x44
#define B1000101 0x45
#define B1000110 0x46
#define B1000111 0x47
#define B1001000 0x48
#define B1001001 0x49
#define B1001010 0x4A
#define B1001011 0x4B
#define B1001100 0x4C
#define B1001101 0x4D
#define B1001110 0x4E
#define B1001111 0x4F
#define B1010000 0x50
#define B1010001 0x51
#define B1010010 0x52
#define B1010011 0x53
#define B1010100 0x54
#define B1010101 0x55
#define B1010110 0x56
#define B1010111 0x57
#define B1011000 0x58
#define B1011001 0x59
#define B1011010 0x5A
#define B1011011 0x5B
#define B1011100 0x5C
#define B1011101 0x5D
#define B1011110 0x5E
#define B1011111 0x5F
#define B1100000 0x60
#define B1100001 0x61
#define B1100010 0x62
#define B1100011 0x63
#define B1100100 0x64
#define B1100101 0x65
#define B1100110 0x66
#define B1100111 0x67
#define B1101000 0x68
#define B1101001 0x69
#define B1101010 0x6A
#define B1101011 0x6B
#define B1101100 0x6C
#define B1101101 0x6D
#define B1101110 0x6E
#define B1101111 0x6F
#define B1110000 0x70
#define B1110001 0x71
#define B1110010 0x72
#define B1110011 0x73
#define B1110100 0x74
#define B1110101 0x75
#define B1110110 0x76
#define B1110111 0x77
#define B1111000 0x78
#define B1111001 0x79
#define B1111010 0x7A
#define B1111011 0x7B
#define B1111100 0x7C
#define B1111101 0x7D
#define B1111110 0x7E
#define B1111111 0x7F


extern int wdmemaddr;
extern int wdmemdata;
extern int wdmemflag;

extern int dmem[dmem_size];
extern int imem[imem_size/4];
void decoder_excute(int inst);
void cpulog();
extern char *reg_name[];
extern unsigned int pc_reg;
extern unsigned int pc_reg_next;
extern unsigned int rv_reg[32];
extern unsigned int rv_reg_3[32];
extern int inst_is_rvc;
void addline2txt(char *data,char *filename);
union inst{
		unsigned int inst;
		struct{
			unsigned L_is_rvc:2;
			unsigned Lother:14;
   			unsigned H_is_rvc:2;
			unsigned Hother:14;
		}LHtype;
		struct{
			unsigned opcode:7;
			unsigned rd:5;
			unsigned fun3:3;
			unsigned rs1:5;
			unsigned rs2:5;
			unsigned fun7:7;
		}COMRVtype;
		struct{
			unsigned opcode:7;
			unsigned rd:5;
			unsigned fun3:3;
			unsigned rs1:5;
			unsigned rs2:5;
			unsigned fun7:7;
		}Rtype;
        struct{
			unsigned opcode:7;
			unsigned rd:5;
			unsigned fun3:3;
			unsigned rs1:5;
			unsigned imm11_0:12;
		}Itype;
        struct{
			unsigned opcode:7;
			unsigned imm4_0:5;
			unsigned fun3:3;
			unsigned rs1:5;
			unsigned rs2:5;
			unsigned imm11_5:7;
		}Stype;
        struct{
			unsigned opcode:7;
			unsigned imm11:1;
            unsigned imm4_1:4;
			unsigned fun3:3;
			unsigned rs1:5;
			unsigned rs2:5;
			unsigned imm10_5:6;
            unsigned imm12:1;
		}Btype;
        struct{
			unsigned opcode:7;
			unsigned rd:5;
			unsigned imm31_12:20;
		}Utype;
		struct{
			unsigned opcode:7;
			unsigned rd:5;
			unsigned imm19_12:8;
			unsigned imm11:1;
			unsigned imm10_1:10;
			unsigned imm20:1;
		}Jtype;
	};


signed int sext(unsigned int data,int length);

#define INSTLOG() addline2txt(instout,"/home/ghb/HRV32/HRV32_SIM/instlog.txt")


#endif