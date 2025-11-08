#include "acpi/acpi.hpp"
#include "acpi/rsdt.hpp"
#include "bootloader_module.hpp"
#include "buddy.hpp"
#include "compiler.hpp"
#include "cpu.hpp"
#include "page.hpp"
#include "trace.hpp"
#include "utility.hpp"
#include "vector.hpp"
#include "x86-64/apic.hpp"
#include "x86-64/gdt.hpp"
#include "x86-64/hpet.hpp"
#include "x86-64/idt.hpp"
#include "x86-64/paging.hpp"
#include "x86-64/pic.hpp"
#include <limine.h>

USED struct limine_memmap_request memmap {
    .id = LIMINE_MEMMAP_REQUEST, .revision = 0, .response = nullptr
};

void timer_interrupt(Idt::IntFrame *frame)
{
    trace(TRACE_INTERRUPT, "Timer interrupt received", frame->rip);
    apic.eoi();
}

extern "C" void _start(void)
{
    pic.remap();
    gdt.init();
    idt.init();
    idt.init_handlers();

    buddy.init(memmap.response);
    pagelist.init(memmap.response);
    console.init();
    trace(TRACE_CPU, "%s %s [%s]", __DATE__, __TIME__, COMPILER_STRING);

    paging.init();
    acpi.init();
    hpet.init();
    apic.init();
    apic.timer_calibrate();
    apic.timer_init(32, ApicTimerMode::PERIODIC, ApicTimerDivide::BY_16, 20);

    idt.set_handler(32, timer_interrupt);

    trace(TRACE_CPU, "Enabling interrupts...");
    enable_interrupts();

    halt();
    UNREACHABLE;
}