	beq	0	0	done
	add	0	0	1
	add	0	0	2
	add	0	0	3
done	lw	0	4	val
	noop
	noop
	noop
	halt
val	.fill	99
