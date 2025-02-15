global gdt_flush

gdt_flush:
    ; Get the pointer to the GDT structure from the stack
    mov eax, [esp + 4]       ; Move the GDT pointer passed on the stack into EAX
    lgdt [eax]               ; Load GDTR with the GDT pointer
    ret

global jump_usermode
extern test_user_function
jump_usermode:
	mov ax, (4 * 8) | 3 ; ring 3 data with bottom 2 bits set for ring 3
	mov ds, ax
	mov es, ax 
	mov fs, ax 
	mov gs, ax ; SS is handled by iret

	; set up the stack frame iret expects
	mov eax, esp
	push (4 * 8) | 3 ; data selector
	push eax ; current esp
	pushf ; eflags
	push (3 * 8) | 3 ; code selector (ring 3 code with bottom 2 bits set for ring 3)
	push test_user_function ; instruction address to return to
	iret

; C declaration: void flush_tss(void);
global flush_tss
flush_tss:
	mov ax, (5 * 8) | 0 ; fifth 8-byte selector, symbolically OR-ed with 0 to set the RPL (requested privilege level).
	ltr ax
	ret
