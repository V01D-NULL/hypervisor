#pragma once

#include "compiler.hpp"

class Gdt
{
    using Selector = int;

  private:
    struct PACKED Segment {
        uint16_t limit{0};
        uint16_t base_low{0};
        uint8_t base_mid{0};
        uint8_t access;
        uint8_t granularity;
        uint8_t base_high{0};
    } segments[5];
    Selector sel{0};

  public:
    struct Gdtr {
        uint16_t limit;
        uint64_t base;
    } PACKED;

    void init();
    void set64(uint8_t granularity, uint8_t access);

  private:
    INLINE void lgdt(Gdtr gdtr) { asm("lgdt %0" ::"m"(gdtr)); }
    NOINLINE void reload_segments() asm("reload_segment_descriptors");
};

EXPOSE_SINGLETON(Gdt, gdt);