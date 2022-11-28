#pragma once

#include "compiler.hpp"
#include "console.hpp"

#define trace(TMASK, ...)                                  \
    do                                                     \
    {                                                      \
        if (likely(trace_mask & TMASK))                    \
        {                                                  \
            console.auto_newline(false);                   \
            console.print("\033[37m" #TMASK "\033[39m: "); \
            console.auto_newline(true);                    \
            console.print(__VA_ARGS__);                    \
        }                                                  \
    } while (0)

#define assert_truth(condition)         \
    do                                  \
    {                                   \
        if (!(condition))               \
            console._panic(#condition); \
    } while (0)

#define panic(...) \
    console._panic(__VA_ARGS__);

enum TRACE_LEVEL {
    TRACE_CPU = BIT(0),       // Anything
    TRACE_HYPER = BIT(1),     // Hypervisor related
    TRACE_INTERRUPT = BIT(2), // Interrupt handler
    TRACE_BUDDY = BIT(3),     // Buddy memory allocation
    TRACE_SLAB = BIT(4),      // Slab memory allocation (tbd)
    TRACE_ERROR = BIT(5),     // Errors
    TRACE_VMX = BIT(6),       // Low-level VMX operations

    TRACE_MM = TRACE_BUDDY | TRACE_SLAB
};

// clang-format off
static constexpr auto trace_mask
{
    TRACE_CPU          |
    TRACE_HYPER        |
    TRACE_INTERRUPT    |
    TRACE_ERROR		   |
	TRACE_VMX
};
// clang-format on