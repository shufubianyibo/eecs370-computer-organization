	lw	0	1	val
	beq	1	0	fail
	add	1	1	2
	halt
fail	halt
val	.fill	5
