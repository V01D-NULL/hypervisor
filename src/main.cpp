#include "buddy.hpp"
#include "compiler.hpp"
#include "cpu.hpp"
#include "page.hpp"
#include "trace.hpp"
#include "utility.hpp"
#include "x86-64/gdt.hpp"
#include "x86-64/idt.hpp"
#include "x86-64/paging.hpp"
#include <limine.h>

USED struct limine_memmap_request memmap {
    .id = LIMINE_MEMMAP_REQUEST, .revision = 0, .response = nullptr
};

extern "C" void _start(void)
{
    gdt.init();
    idt.init();

    buddy.init(move(memmap.response));
    pagelist.init(move(memmap.response));
    console.init();

    auto ptr = buddy.alloc(8192);
    trace(TRACE_CPU, "ptr = %lx", ptr);
    buddy.free(ptr);

    ptr = buddy.alloc(8192);
    trace(TRACE_CPU, "ptr = %lx", ptr);

	paging::init();
	// VirtualMemoryManager vmm{true};

    halt();
    UNREACHABLE;
}