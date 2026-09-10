bits 16

section .entry progbits alloc exec nowrite

extern _cstart_
global entry

entry:
	; setting up the stack
	cli
	mov ax, ds
	mov ss, ax
	mov sp, 0
	mov bp, sp
	sti

	; expect boot drive to dl, send it as argument to main function
	xor dh, dh
	push dx
	call _cstart_
	add sp, 2

	cli
	hlt