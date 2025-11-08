#include "x86-64/idt.hpp"
#include "trace.hpp"
#include "x86-64/apic.hpp"

CREATE_SINGLETON(Idt, idt);

extern "C" Idt::IdtDescriptor idt_handlers[256];

/* Called from assembly for the sake of simplicity, readability and maintainability */
extern "C" void set64(uint64_t offset, uint16_t selector, char ist, char types_attr, int vector)
{
    idt_handlers[vector] = {
        .offset0 = (uint16_t)(offset & 0xffff),
        .selector = selector,
        .ist = ist,
        .types_attr = types_attr,
        .offset1 = (uint16_t)((offset >> 16)),
        .offset2 = (uint32_t)((offset >> 32)),
        .reserved = 0};
}

extern "C" void interrupt_handler(Idt::IntFrame *frame)
{
    Idt::Handler handler = idt.get_handler(frame->isr_number);
    handler(frame);
}

void Idt::default_handler(Idt::IntFrame *frame)
{
    trace(TRACE_INTERRUPT, "Unhandled ISR=%#lx RIP=%#lx CS=%#lx RFLAGS=%#lx ERR=%#lx",
          frame->isr_number, frame->rip, frame->cs, frame->rflags, frame->error_code);

    trace(TRACE_INTERRUPT, "RAX=%#018lx RBX=%#018lx RCX=%#018lx RDX=%#018lx\nRSI=%#018lx RDI=%#018lx RBP=%#018lx RSP=%#018lx\nR8 =%#018lx R9 =%#018lx R10=%#018lx R11=%#018lx\nR12=%#018lx R13=%#018lx R14=%#018lx R15=%#018lx",
          frame->rax, frame->rbx, frame->rcx, frame->rdx,
          frame->rsi, frame->rdi, frame->rbp, frame->rsp,
          frame->r8, frame->r9, frame->r10, frame->r11,
          frame->r12, frame->r13, frame->r14, frame->r15);

    while (true)
        halt_and_disable_interrupts();
}
