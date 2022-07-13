#pragma once

#include "compiler.hpp"

INLINE void halt()
{
    for (;;)
        asm("cli; hlt");
}