#include "acpi/acpi.hpp"
#include "acpi/rsdt.hpp"
#include "bootloader_module.hpp"
#include "buddy.hpp"
#include "compiler.hpp"
#include "cpu.hpp"
#include "page.hpp"
#include "trace.hpp"
#include "utility.hpp"
#include "x86-64/gdt.hpp"
#include "x86-64/idt.hpp"
#include "x86-64/paging.hpp"
#include "x86-64/pic.hpp"
#include <limine.h>

USED struct limine_memmap_request memmap {
    .id = LIMINE_MEMMAP_REQUEST, .revision = 0, .response = nullptr
};

extern "C" void _start(void)
{
    pic.remap();
    gdt.init();
    idt.init();

    buddy.init(memmap.response);
    pagelist.init(memmap.response);
    console.init();
    trace(TRACE_CPU, "%s %s [%s]", __DATE__, __TIME__, COMPILER_STRING);

    paging.init();
    acpi.init();

    struct Madt {
        Sdt sdt;
        uint32_t lapic_addr;
        uint32_t lapic_flags;
    } PACKED;

    auto madt = acpi.find_table<Madt *>("APIC");
    trace(TRACE_CPU, "%#lx %#lx %s", madt->lapic_addr, madt->lapic_flags, madt->sdt.signature);

    halt();
    UNREACHABLE;
}