#ifndef _MEM_H_
#define _MEM_H_

#define imem_size 1024*32
extern int imem[imem_size/4];			///// 32K/4 line
extern int instsize;
extern int dmem[dmem_size];
void txt2mem();
void printmem();

#endif