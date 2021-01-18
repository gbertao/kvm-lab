	.global pl
	.text
pl:
	mov $0xa, %rbx
	push %rbx
	pop %rax
	hlt
