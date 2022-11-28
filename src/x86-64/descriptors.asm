global reload_segment_descriptors
reload_segment_descriptors:
	mov ax, 0x10
	mov ds, ax
	mov gs, ax
	mov es, ax
	mov fs, ax
	mov ss, ax
	
	pop rdi
	push 0x8
	push rdi

	retfq

global jump_usermode
extern test_user_function
jump_usermode:
;enable system call extensions that enables sysret and syscall
	; mov ecx, 0xc0000082
	; wrmsr
	; mov ecx, 0xc0000080
	; rdmsr
	; or eax, 1
	; wrmsr
	; mov ecx, 0xc0000081
	; rdmsr
	; mov edx, 0x00180008
	; wrmsr
 
	; IA32_STAR MSR
	; mov ecx, 0xC0000081
	; rdmsr
	; ; Load user 32-bit cs into STAR[63:48] and load kernel cs into STAR[47:32]
	; mov edx, 0x00180008
	; wrmsr

	mov rcx, test_user_function ; to be loaded into RIP
	mov r11, 0x202 ; to be loaded into EFLAGS
	o64 sysret ;use "o64 sysret" if you assemble with NASM


