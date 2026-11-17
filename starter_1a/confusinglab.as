	noop
add	.fill	5
	lw	0	1	add	// Should load from addr 1
r1	.fill	99
	lw	0	1	r1	// Should load from addr 3
// Forward Fill
	lw	0	1	ptr
ptr	.fill	final			// Should contain addr 8
final	halt
