#pragma once

#include "compiler.hpp"
#include <stdint.h>

struct iframe {
    int64_t r15;
    int64_t r14;
    int64_t r13;
    int64_t r12;
    int64_t r11;
    int64_t r10;
    int64_t r9;
    int64_t r8;
    int64_t rsi;
    int64_t rdi;
    int64_t rdx;
    int64_t rcx;
    int64_t rbx;
    int64_t rax;
    int64_t isr_number;
    int64_t error_code;
    int64_t rip;
    int64_t cs;
    int64_t rflags;
    int64_t rsp;
    int64_t ss;
};

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