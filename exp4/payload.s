	.global pl
	.text
pl:
	mov $0xa, %eax
	jmp func
c:
	hlt

func:
	mov $0xa, %ebx
	jmp c
