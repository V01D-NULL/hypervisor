#pragma once
#include "compiler.hpp"
#include <stdint.h>

namespace IO
{
    template <typename T>
    INLINE void out(uint16_t port, T value)
    {
        asm volatile("out %0, %1" ::"a"(value), "Nd"(port));
    }
} // namespace IO
