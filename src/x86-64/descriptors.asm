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