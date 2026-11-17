	.fill	65536
	.fill	-65536
	.fill	2147483647
	.fill	-2147483648
	beq	0	0	target	// Offset = 2 - 0 - 1 = 1
	beq	0	0	self	// Offset = 1 - 1 - 1 = -1
self	halt
target	noop
