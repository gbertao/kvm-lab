	.global pl
	.text
pl:
	call func
	hlt
func:
	push %rbp
	mov %rsp, %rbp

	mov $0x0a, %rax

	leave
	ret
