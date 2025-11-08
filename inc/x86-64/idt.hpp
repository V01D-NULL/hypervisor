#pragma once

#include "compiler.hpp"
#include "trace.hpp"
#include "vector.hpp"
#include <stdint.h>

class Idt
{
  public:
    struct IntFrame;
    using Handler = void (*)(IntFrame *);

    struct IdtDescriptor {
        uint16_t offset0;
        uint16_t selector;
        char ist;
        char types_attr;
        uint16_t offset1;
        uint32_t offset2;
        uint32_t reserved;
    } PACKED;

    struct IntFrame {
        uint64_t r15;
        uint64_t r14;
        uint64_t r13;
        uint64_t r12;
        uint64_t r11;
        uint64_t r10;
        uint64_t r9;
        uint64_t r8;
        uint64_t rsi;
        uint64_t rdi;
        uint64_t rbp;
        uint64_t rdx;
        uint64_t rcx;
        uint64_t rbx;
        uint64_t rax;
        uint64_t isr_number;
        uint64_t error_code;
        uint64_t rip;
        uint64_t cs;
        uint64_t rflags;
        uint64_t rsp;
        uint64_t ss;
    } PACKED;

    NOINLINE void init() asm("setup_idt");

    void init_handlers()
    {
        interrupt_routine_handlers.resize(255, &default_handler);
    }

    void set_handler(int vector, Handler handler)
    {
        interrupt_routine_handlers.at(vector) = handler;
    }

    Handler get_handler(int vector) const
    {
        return interrupt_routine_handlers.at(vector);
    }

  private:
    static void default_handler(Idt::IntFrame *frame);

  private:
    Vector<Handler, 255> interrupt_routine_handlers;
};

EXPOSE_SINGLETON(Idt, idt);