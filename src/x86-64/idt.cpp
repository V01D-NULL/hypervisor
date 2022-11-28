#include "x86-64/idt.hpp"
#include "trace.hpp"

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
        .reserved = 0
    };
}

extern "C" void interrupt_handler(iframe frame)
{
	trace(TRACE_INTERRUPT, "CPU Exception: %d | err: %d", frame.isr_number, frame.error_code);
	trace(TRACE_INTERRUPT, "RIP = %p", frame.rip);
	
	// for (int i = 0; i < 100; i++)
	// 	put_pixel(i, 100, 0xFF0000);
		
	halt();
}