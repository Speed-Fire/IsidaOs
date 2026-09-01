org 0x7C00
bits 16

%define ENDL 0x0D, 0x0A

start:
	jmp main

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




main:

	; setup data segments
	mov ax, 0 ; can't write to ds/es directly
	mov ds, ax
	mov es, ax

	; setup stack
	mov ss, ax
	mov sp, 0x7C00

	mov si, msg_hello
	call print

	hlt
	
.halt:
	jmp .halt
	
msg_hello: db 'Hello, my niggers', ENDL, 0

times 510-($-$$) db 0
dw 0AA55h