	.code16
	.global pl
	.text
pl:
	mov $0x50, %dx
	mov $0x41, %al
	out %al, %dx
	mov $0xa, %eax

        hlt
