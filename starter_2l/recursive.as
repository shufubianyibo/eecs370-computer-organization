	lw	0	1	n
	lw	0	2	r
	lw	0	4	Faddr
	jalr	4	7
	halt
start	beq	0	2	base
	beq	1	2	base
	lw	0	6	one
	sw	5	7	Stack
	add	5	6	5
	sw	5	1	Stack
	add	5	6	5
	sw	5	2	Stack
	add	5	6	5
	add	5	6	5
	lw	0	4	neg1
	add	1	4	1
	lw	0	6	Faddr
	jalr	6	7
	lw	0	6	neg1
	add	5	6	5
	sw	5	3	Stack
	add	5	6	5
	lw	5	2	Stack
	add	5	6	5
	lw	5	1	Stack
	lw	0	4	neg1
	add	1	4	1
	add	2	4	2
	lw	0	6	one
	add	5	6	5
	add	5	6	5
	add	5	6	5
	lw	0	6	Faddr
	jalr	6	7
	lw	0	6	neg1
	add	5	6	5
	lw	5	4	Stack
	add	3	4	3
	add	5	6	5
	add	5	6	5
	add	5	6	5
	lw	5	7	Stack
	jalr	7	6
base	lw	0	3	one
	jalr	7	6
n	.fill	7
r	.fill	3
neg1	.fill	-1
one	.fill	1
Faddr	.fill	start
