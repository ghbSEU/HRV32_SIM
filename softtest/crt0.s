_start:
        # Load some constants and do some arithmetic
       li a0,0
       li a1,0
       li a2,0
       li a3,0
       li x5, 0x4000	#gpio crtl
	li x1, 0x4004	#gpio data
	li x2, 0x1+0x4		#gpio[0]/[1] output
	sw x2, 0(x5)
	li x3, 0x1		#gpio=01
	sw x3, 0(x1)
	li x2, 0x2+0x8		#gpio[0]/[1] input
	sw x2, 0(x5)
	li x6, 0x12345678
	li x6, 0x2345
	lw x3, 0(x1)		#read gpio
	li x4, 0x2
       jal t2,main
exit:
        # Similar to exit(0) in C.
        ecall

