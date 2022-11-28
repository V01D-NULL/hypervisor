#pragma once

#include "memory.hpp"

// Clang has a trailing space at the end for whatever reason.
#define COMPILER_STRING __VERSION__ "\b"

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define INIT_PRIORITY(x) __attribute__((init_priority(x)))
#define INLINE inline __attribute__((always_inline))
#define NOINLINE __attribute__((noinline))

#define MAYBE_UNUSED __attribute__((maybe_unused))
#define USED __attribute__((used))
#define PACKED __attribute__((packed))
#define PERCPU __attribute__((section(".cpulocal")))
#define UNREACHABLE __builtin_unreachable()

#define CREATE_SINGLETON(Type, name) Type name
#define EXPOSE_SINGLETON(Type, name) extern Type name

#define ALIGN_UP(__number) (((__number) + 4096 - 1) & ~(4096 - 1))
#define ALIGN_DOWN(__number) ((__number) & ~(4096 - 1))

#define BIT(o) (1 << o)

#define MAKE_BITFIELD(...) \
    union {                \
        uint64_t All;      \
        struct {           \
            __VA_ARGS__;   \
        } field;           \
    }
