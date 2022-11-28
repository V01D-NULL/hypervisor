#pragma once

#include "vm/vmm.hpp"
#include <stdint.h>

namespace vmx
{
    void init(VMmonitor::vcpu &vcpu);
    bool supported();

    void create_vmcs(VMmonitor::vcpu &vcpu);

    void vmxon(uint64_t vmcs_addr);
    void vmptrld(uint64_t addr);

} // namespace vmx
