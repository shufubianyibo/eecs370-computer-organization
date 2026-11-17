	lw	0	1	ini
	lw	0	2	decr
loop	add	1	2	1
	beq	1	0	done
	beq	0	0	loop
done	halt
ini	.fill	5
decr	.fill	-1
