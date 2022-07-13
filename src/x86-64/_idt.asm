bits 64
section .text

%define sizeof_gate_descriptor 16

extern interrupt_handler
interrupt_stub:
    cld
    push rax
    push rbx
    push rcx
    push rdx
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

	call interrupt_handler

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    add rsp, 16
    iretq

%macro define_isr_handler 1
global isr%1
isr%1:
    push dword 0 ; dummy error
    push %1      ; Int no.
    jmp interrupt_stub
%endmacro

%macro define_isr_handler_with_err 1
global isr%1
isr%1:
    push %1 ; Int no.
    jmp interrupt_stub
%endmacro

extern set64
%macro set_entry 2 ; Params: function/label, interrupt vector
    mov rdi, %1
    mov rsi, 0x08
    mov rdx, 0x00
    mov rcx, 0x8E
    mov r8, %2
    call set64
%endmacro

global setup_idt
setup_idt:
    %assign n 0
    %rep 256
        set_entry isr%+n, n
    %assign n n+1
    %endrep
    
    ; Setup the idtr
    mov rax, idt_handlers
    mov [idtr], dword (256 * sizeof_gate_descriptor) - 1 ; limit
    mov [idtr+2], rax ; offset
    lidt [idtr]
    ret

; CPU exceptions
%assign n 0
%rep 8
    define_isr_handler n
%assign n n+1
%endrep

define_isr_handler_with_err 8
define_isr_handler 9
define_isr_handler_with_err 10
define_isr_handler_with_err 11
define_isr_handler_with_err 12
define_isr_handler_with_err 13
define_isr_handler_with_err 14

%assign n 15
%rep 17
    define_isr_handler n
%assign n n+1
%endrep

; IRQ's
%assign n 32
%rep 16
    define_isr_handler n
%assign n n+1
%endrep

; User defined isrs
%assign n 48
%rep 208
    define_isr_handler n
%assign n n+1
%endrep


section .data
gate_descriptor:
    dw 0 ; offset1
    dw 0 ; Selector
    db 0 ; Ist
    db 0 ; Types & attributes
    dw 0 ; offset2
    dd 0 ; offset3
    dd 0 ; reserved

idtr:
    dw 0 ; Limit
    dq 0 ; Offset

section .bss
global idt_handlers
    idt_handlers: resb 256 * sizeof_gate_descriptor ; Equivalent of: IdtDescriptor idt_handlers[256], multiplied by 128 aka the sizeof(IdtDescriptor)

