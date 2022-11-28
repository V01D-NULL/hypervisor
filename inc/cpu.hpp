#pragma once

#include "compiler.hpp"

#define cpuid(level, a, b, c, d)                      \
    asm volatile("cpuid"                              \
                 : "=a"(a), "=b"(b), "=c"(c), "=d"(d) \
                 : "0"(level))

INLINE void halt()
{
    for (;;)
        asm("cli; hlt");
}

INLINE uint64_t rd_cr4()
{
	uint64_t cr4{};
	asm volatile("mov %%cr4, %0" : "=r"(cr4));
	return cr4;
}

INLINE void wr_cr4(uint64_t value)
{
	asm("mov %0, %%cr4" :: "r"(value));
}