#pragma once

#include "buddy.hpp"
#include "compiler.hpp"
#include "x86-64/intel/vmx_registers.hpp"
#include <stddef.h>
#include <stdint.h>

class VMmonitor
{
  public:
    // guest cpu state (context)
    struct context {
        uint64_t cr2;
        // debug registers
        // msrs
        // special registers (GDTR, IDTR, LDTR, ..)
        uint64_t rflags;
        // general purpose registers (rax-r15)
        // segments
    } PACKED;

    // Represents a virtual cpu. Used by the hypervisor only.
    struct vcpu {
        uint32_t vmxon_region;
        uint64_t vmxon_region_base;

        vmx_vmcs vmcs_region;
        uint64_t vmcs_region_base;

        vmx_basic_msr_result vmx_basic_msr;
        struct context guest_cpu_ctx;
    };

    // Represents an entire guest
    struct guest {
        int num_vcpus{0};
        struct vcpu *vcpus{nullptr};
        uint8_t *stack{nullptr};
    };

  public:
    void init();

  private:
    struct guest *create_guest_context();

  private:
    guest *root{nullptr};
};

EXPOSE_SINGLETON(VMmonitor, vmmon);