#pragma once

#include "compiler.hpp"

INLINE void halt()
{
    for (;;)
        asm volatile("hlt");
}

INLINE void halt_and_disable_interrupts()
{
    for (;;)
        asm volatile("cli; hlt");
}

INLINE void enable_interrupts()
{
    asm volatile("sti");
}

INLINE void disable_interrupts()
{
    asm volatile("cli");
}