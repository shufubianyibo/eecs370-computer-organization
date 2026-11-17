lw      0       1       5       
	lw	0	1	data
	beq	0	1	target
	noop
target	halt
data	.fill	10
	.fill	data
