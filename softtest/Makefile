TOOLCHAIN := /home/ghb/riscvtool/riscv-gnu-toolchain/bin/riscv32-unknown-elf-

CC    = ${TOOLCHAIN}gcc
AS    = ${TOOLCHAIN}as
LINK  = ${TOOLCHAIN}ld
OBJDUMP = ${TOOLCHAIN}objdump
OBJCOPY = ${TOOLCHAIN}objcopy

SSRC = $(wildcard *.S)
sSRC = $(wildcard *.s)
CSRC = $(wildcard *.c)



CRTOBJ = $(SSRC:%.S=%.o)

OBJECTS = $(sSRC:%.s=%.o) $(CSRC:%.c=%.o)

OBJDUMPFLAGS = -T

HEXFLAGS	= -O ihex



%.o : %.s
	${AS} -march=rv32imc -mabi=ilp32 -o $@ $^
	${AS} -march=rv32imc -mabi=ilp32 crt0.s -o crt0.o


EXelf : ${OBJECTS} link.ld
	${LINK} ${OBJDUMPFLAGS} link.ld  ${CRTOBJ} ${OBJECTS} -o examples.elf 


EXhex : EXelf
	${OBJCOPY} ${HEXFLAGS} examples.elf examples.hex 

txt :
	/home/ghb/riscvtool/riscv-gnu-toolchain/bin/riscv32-unknown-elf-objdump -S examples.elf
	python hex2hex2.py
	cat examples.hex2
#	cat examples.hex2 > /mnt/hgfs/share/mem.dat
	

.PHONY :clean 
clean:
	rm *.o examples.elf examples.hex examples.hex2
