org 0x7C00
bits 16

%define ENDL 0x0D, 0x0A

;
;	FAT12 header
;
jmp short start
nop

bdb_oem:						db 'MSWIN4.1'		        ; 8 bytes
bdb_bytes_per_sector:			dw 512				        
bdb_sectors_per_cluster:		db 1				        
bdb_reserved_sectors:			dw 1				        
bdb_fat_count:					db 2				        
bdb_dir_entries_count:			dw 0E0h				        
bdb_total_sectors:				dw 2880				        ; 2880 * 512 = 1.44MB
bdb_media_descriptor_type:		db 0F0h				        ; F0 = 3.5 floppy disk
bdb_sectors_per_flat:			dw 9				        ; 9 sectors/fat
bdb_sectors_per_track:			dw 18				        
bdb_heads:						dw 2				        
bdb_hidden_sectors:				dd 0				        
bdb_large_sector_count:			dd 0				        
													        
; extended boot record								        
ebr_drive_number:				db 0				        ; 0x00 floppy, 0x80 hdd ...
								db 0				        ; reserved
ebr_signature:					db 29h
ebr_volume_id:					db 12h, 34h, 56h, 78h		; serial number
ebr_volume_label:				db 'NANOBYTE OS'			; 11 bytes, padded with spaces
ebr_system_id:					db 'FAT12'					; 8 bytes

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

	; attempt to read sth from disk
	; BIOS should set DL to drive number
	mov [ebr_drive_number], dl
	
	mov ax, 1								; LBA=1, second sector from disk
	mov cl, 1								; 1 sector to read
	mov bx, 0x07E00							; data should be after the bootloader
	call disk_read

	mov si, msg_hello
	call print

	cli
	hlt
	
;
;	Error handlers
;

floppy_error:
	mov si, msg_read_failed
	call print
	jmp wait_key_and_reboot

wait_key_and_reboot:
	mov ah, 0
	int 16h					; wait for keypress
	jmp 0FFFFh:0			; jump to the beginning of BIOS. Should reboot

.halt:
	cli						; disable interrupts, so we can't get out of halt
	hlt
	

;
;	Disk routines
;

;
; Convert LBA address to CHS address
; Params:
;	- ax: LBA address
; Returns:
;	- cx [bits 0-5]: sector number
;	- cx [bits 6-15]: cylinder
;	- dh: head
;
lba_to_chs:

	push ax
	push dx

	xor dx, dx							; dx = 0
	div word [bdb_sectors_per_track]	; ax = LBA / sectors_per_track
										; dx = LBA % sectors_per_track
	inc dx								; dx = (LBA % sectors_per_track) + 1 = sector
	mov cx, dx							; cx = sector

	xor dx, dx							; dx = 0
	div word [bdb_heads]				; ax = (LBA / sectors_per_track) / heads = cylinder
										; ax = (LBA / sectors_per_track) % heads = head
	mov dh, dl							; dh = head
	mov ch, al							; ch = cylinder (lower 8 bits)
	shl ah, 6							
	or  cl, ah							; put upper 2 bits of cylinder to cl

	pop ax
	mov dl, al							; restore dl
	pop ax
	ret


;
; Reads sectors from disk
; Params:
;	- ax: LBA address
;	- cl: number of sectors to read
;	- dl: drive number
;	- es:bx: memory address where to store read data
;
disk_read:
	push ax								; save registers
	push bx
	push dx
	push cx
	push di

	push cx								; temporarily store value of cx (number of sectors to read)
	call lba_to_chs						; converting LBA address to CHS
	pop ax								; ax = number of sectors to read

	mov ah, 02h
	mov di, 3							; retry count

.retry:
	pusha								; save all registers. BIOS can modify any of them
	stc									; set carry flag, some BIOSes don't set it
	int 13h								; if carry flag is cleared -> success
	jnc .done							; jump if carry is not set

	; read failed
	popa
	call disk_reset

	dec di
	test di, di
	jnz .retry

.fail:
	; all attemts used
	jmp floppy_error

.done:
	popa

	pop di								; restore registers
	pop cx
	pop dx
	pop bx
	pop ax
	ret


;
; Resets disk controller
; Params:
;	- dl: drive number
;
disk_reset:
	pusha
	mov ah, 0
	stc
	int 13h
	jc floppy_error
	popa 
	ret




msg_hello:					db 'Hello, my niggers', ENDL, 0
msg_read_failed:			db 'Read from disk failed', ENDL, 0

times 510-($-$$) db 0
dw 0AA55h