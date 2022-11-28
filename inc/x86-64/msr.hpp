#pragma once

#include "compiler.hpp"

enum {
    IA32_FEATURE_CONTROL = 0x3a,
	IA32_VMX_BASIC = 0x480
};

INLINE uint64_t rdmsr(uint32_t msr)
{
    uint32_t eax{0}, edx{0};
    asm volatile("rdmsr"
                 : "=a"(eax), "=d"(edx)
                 : "c"(msr)
                 : "memory");
    return ((uint64_t)eax << 32) | edx;
}