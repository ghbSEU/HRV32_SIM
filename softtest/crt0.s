_start:
        # Load some constants and do some arithmetic
       li a0,0
       li a1,0
       li a2,0
       li a3,0
  
	li sp,0x10001000

    jal ra,main
loop:
	nop
	nop
	nop
	jal ra,loop
exit:
        # Similar to exit(0) in C.
        ecall

