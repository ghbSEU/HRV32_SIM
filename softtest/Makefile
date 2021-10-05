TOOLCHAIN := /home/ghb/gap/gap_riscv_toolchain_ubuntu_18/bin/riscv32-unknown-elf-

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
	${AS} -march=rv32i -o $@ $^
	${AS} -march=rv32i  crt0.s -o crt0.o


EXelf : ${OBJECTS} link.ld
	${LINK} ${OBJDUMPFLAGS} link.ld ${CRTOBJ} ${OBJECTS} -o examples.elf 


EXhex : EXelf
	${OBJCOPY} ${HEXFLAGS} examples.elf examples.hex 

.PHONY :clean 
clean:
	rm test.o test.elf examples.hex examples.hex2
