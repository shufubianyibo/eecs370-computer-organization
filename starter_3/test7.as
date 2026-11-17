	lw	0	1	val
	noop
	noop
	noop
	beq	0	0	done
	add	1	1	2
	add	1	1	3
	add	1	1	4
	halt
done	halt
val	.fill	5
