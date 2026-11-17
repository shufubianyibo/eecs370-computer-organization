	lw	0	4	Faddr
	jalr	4	7
	halt
Myfunc	add	1	1	3
	jalr	7	6
Faddr	.fill	Myfunc
