#include "buddy.hpp"
#include "compiler.hpp"
#include "cpu.hpp"
#include "page.hpp"
#include "trace.hpp"
#include "utility.hpp"
#include "vm/vmm.hpp"
#include "x86-64/gdt.hpp"
#include "x86-64/idt.hpp"
#include "x86-64/paging.hpp"
#include <limine.h>

USED struct limine_memmap_request memmap {
    .id = LIMINE_MEMMAP_REQUEST, .revision = 0, .response = nullptr
};

extern "C" {
	void test_user_function() { for(;;);}
	void jump_usermode();
}

enum
{
	IA32_APIC_BASE = 0x1B,
	IA32_EFER = 0xC0000080,
	IA32_KERNEL_GS_BASE = 0xC0000102,
	IA32_TSC_DEADLINE = 0x6E0,
	GS_BASE = 0xC0000101,
	PAT_MSR = 0x277,
	STAR = 0xC0000081,
	LSTAR = 0xC0000082,
	CSTAR = 0xC0000083,
	IA32_FMASK = 0xC0000084
};

inline void wrmsr(uint32_t msr, uint64_t in)
{
	uint32_t eax = in;
	uint32_t edx = in >> 32;

	trace(TRACE_CPU, "eax: %lx | edx: %lx | in: %lx | msr: %lx\n", eax, edx, in, msr);
	asm volatile("wrmsr" ::"a"(eax), "d"(edx), "c"(msr)
				 : "memory");
}

inline uint64_t rdmsr(uint32_t msr)
{
	uint32_t eax, edx = 0;
	asm volatile("rdmsr"
				 : "=a"(eax), "=d"(edx)
				 : "c"(msr)
				 : "memory");
	return ((uint64_t)edx << 32) | eax;
}

#define SEL_USER_CODE32 0x1b
#define SEL_USER_DATA   0x23
#define SEL_KERN_CODE   0x8

void x86_syscall_handler() {
	trace(TRACE_CPU, "LOL");
	panic("");
}

extern "C" void _start(void)
{
	pagelist.init(move(memmap.response));
    buddy.init(move(memmap.response));
    gdt.init();
	// for (int i = 0; i < 100; i++)
	// 	put_pixel(i, 10, 0xFFFFFF);
    
	idt.init();
    console.init();

	trace(TRACE_CPU, "IA32_EFER: %lx\n", rdmsr(IA32_EFER));
	wrmsr(IA32_EFER, rdmsr(IA32_EFER) | (1 << 0)); // Enable MSR IA32_EFER.SCE (System call extensions)
	wrmsr(STAR, (static_cast<uint64_t>(SEL_USER_CODE32) << 48 | static_cast<uint64_t>(SEL_KERN_CODE) << 32));
	wrmsr(LSTAR, (uint64_t)x86_syscall_handler);

	trace(TRACE_CPU, "STAR: 0x%p\n", rdmsr(STAR));

    trace(TRACE_CPU, "Booting hypervisor: %s %s [%s]", __DATE__, __TIME__, COMPILER_STRING);
    paging::init();
	jump_usermode();

    vmmon.init();

    halt();
    UNREACHABLE;
}
