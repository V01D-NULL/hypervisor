#include "vm/vmm.hpp"
#include "utility.hpp"
#include "x86-64/intel/vmx.hpp"
#include "x86-64/msr.hpp"

CREATE_SINGLETON(VMmonitor, vmmon);

// This is just shorter and easier to read without having to expose the name 'vmm'
using vmm = VMmonitor;

void vmm::init()
{
    root = move(create_guest_context());
    vmx::init(root->vcpus[0]);
}

vmm::guest *vmm::create_guest_context()
{
    // Todo: Write a slab allocator for this
    auto ctx = (guest *)buddy.must_alloc(sizeof(context));
    ctx->stack = (uint8_t *)(buddy.must_alloc(page_size * 4));
    ctx->num_vcpus = 1; // No SMP for now
    ctx->vcpus = (vcpu *)buddy.must_alloc(sizeof(vcpu) * ctx->num_vcpus);
    ctx->vcpus[0].vmx_basic_msr.All = rdmsr(IA32_VMX_BASIC);

    trace(TRACE_HYPER, "\nAllocated guest at: 0x%lx\n  stack:      0x%lx (%d)\n  num-vcpus:  %ld\n  vcpus-base: 0x%lx\n", ctx, ctx->stack, (page_size * 4), ctx->num_vcpus, ctx->vcpus);
    return ctx;
}