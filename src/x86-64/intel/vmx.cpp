#include "x86-64/intel/vmx.hpp"
#include "buddy.hpp"
#include "cpu.hpp"
#include "trace.hpp"
#include "x86-64/intel/vmx_registers.hpp"
#include "x86-64/msr.hpp"

static constexpr bool safety_checks{true};

void vmx::init(VMmonitor::vcpu &vcpu)
{
    if (!supported())
        panic("VMX is not supported or disabled (Check your BIOS settings).");

    wr_cr4(rd_cr4() | (1 << 13));
    trace(TRACE_VMX, "CR4.VMXE = 1");

    create_vmcs(vcpu);
	// vmxon(vcpu.vmxon_region_base);
}

bool vmx::supported()
{
    // Check if vmx is supported via cpuid
    auto vmx_support{0}, _{0};
    cpuid(1, _, _, vmx_support, _);
    auto vmx_present = (vmx_support >> 5) & 1;

    // Check if vmx is enabled via a msr (BIOS settings have an option to disable vmx via a msr)
    auto vmx_enabled = !(rdmsr(IA32_FEATURE_CONTROL) & 1) && !((rdmsr(IA32_FEATURE_CONTROL) >> 1) & 1);
    return vmx_present && vmx_enabled;
}

// Todo: Should this be 'ctx->vcpu->vmcs_addr' instead?
void vmx::vmxon(uint64_t vmcs_addr)
{
    asm("vmxon %0" ::"m"(vmcs_addr)
        : "memory");

    trace(TRACE_VMX, "Executed 'vmxon 0x%lx'", vmcs_addr);
}

void vmx::vmptrld(uint64_t addr)
{
}

void vmx::create_vmcs(VMmonitor::vcpu &vcpu)
{
    auto vmcs = reinterpret_cast<uint64_t>(buddy.must_alloc(4096));
    vcpu.vmcs_region.field.revision = vcpu.vmx_basic_msr.field.revision;
    vcpu.vmcs_region_base = vmcs;
    trace(TRACE_VMX, "Allocated VMCS region at: 0x%lx (region size: %d)", vmcs, vcpu.vmx_basic_msr.field.region_size);
    trace(TRACE_VMX, "VMCS Revision: %d", vcpu.vmx_basic_msr.field.revision);
}