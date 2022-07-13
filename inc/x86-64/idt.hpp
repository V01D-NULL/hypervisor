#pragma once

#include "compiler.hpp"
#include <stdint.h>

class Idt
{
  public:
    struct IdtDescriptor {
        uint16_t offset0;
        uint16_t selector;
        char ist;
        char types_attr;
        uint16_t offset1;
        uint32_t offset2;
        uint32_t reserved;
    } PACKED;

	NOINLINE void init() asm("setup_idt");
};

EXPOSE_SINGLETON(Idt, idt);