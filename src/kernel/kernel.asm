org 0x0
bits 16

%define ENDL 0x0D, 0x0A

start:

	mov si, msg_hello
	call print

.halt:
	cli
	hlt


;
; Prints a string to the screen.
; Params:
;	- ds:si points to string
print:
	push si
	push ax

.loop:
	lodsb
	or al, al ; check if null terminator

	mov ah, 0x0E
	mov bh, 0
	int 0x10

	jz .done
	jmp .loop

.done:
	pop ax
	pop si
	ret
	
	
msg_hello: db 'Hello, my niggers', ENDL, 0