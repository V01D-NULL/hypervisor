#pragma once

#include "compiler.hpp"
#include "page.hpp"
#include <stdint.h>

// Intel SDM Chapter 23.2 FORMAT OF THE VMCS REGION
struct vmx_vmcs {
  private:
    using T = uint32_t;

  public:
    MAKE_BITFIELD(T revision : 30; T shadow_vmcs : 1);
    uint32_t abort_indicator;
    uint8_t data[page_size - 8]; // VMCS is usually one page_size(4k) in size. Remove 8 bytes to account for: 4 bytes for the bitfield and 4 bytes for the abort indicator
} PACKED;

union vmx_basic_msr_result {
    uint64_t All;
    struct
    {
        uint32_t revision : 31;                       // Bit [00:30]
        uint32_t reserved0 : 1;                       // Bit [31]
        uint32_t region_size : 12;                    // Bit [32-43]
        uint32_t region_clear : 1;                    // Bit [44] (bit 44 is set if and only if bits 43:32 are clear).
        uint32_t reserved1 : 3;                       // Bit [45:47]
        uint32_t always_zero : 1;                     // Bit [48] If the bit is 0, these addresses are limited to the processor’s physical-address width. (Always 0 for for processors that support Intel 64 architecture.)
        uint32_t smm_dual_monitor : 1;                // Bit [49]
        uint32_t memory_type : 4;                     // Bit [50:53]
        uint32_t vm_exit_instruction_information : 1; // Bit [54]
        uint32_t may_clear_to_zero : 1;               // Bit [55] All bits set to 1 by default may be set to zero if this bit is 1.
        uint32_t reserved2 : 8;                       // Bit [56:63]
    } field;
};
