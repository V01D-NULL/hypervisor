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

    template <typename T>
    INLINE T inb(uint16_t port)
    {
        T ret;
        asm volatile("inb %1, %0"
                     : "=a"(ret)
                     : "Nd"(port));
        return ret;
    }

    INLINE void wait()
    {
        inb<uint8_t>(0x80);
    }

} // namespace IO
