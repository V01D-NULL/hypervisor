#include "x86-64/paging.hpp"
#include "buddy.hpp"
#include "page.hpp"
#include <limine.h>

USED struct limine_kernel_address_request kernel_address {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST, .revision = 0, .response = nullptr
};

constexpr size_t GB = 0x40000000UL;

namespace paging
{
    // clang-format off
	INLINE void invlpg(uint64_t vaddr) { asm volatile("invlpg %0" ::"m"(vaddr) : "memory"); }
	INLINE void wrcr3(uint64_t pml) { asm volatile("mov %0, %%cr3\n" ::"r"(pml) : "memory"); }
    // clang-format on

    Pml *next(struct Pml *entry, size_t level, int flags);
    pte paging_create_entry(uint64_t paddr, int flags);
    uint64_t index_of(uint64_t vaddr, int offset);

    static Pml *hypervisor_pagemap{nullptr};

    void init()
    {
        trace(TRACE_CPU, "kern_load_addr: %lp", kernel_address.response->virtual_base);
        hypervisor_pagemap = reinterpret_cast<Pml *>(buddy.must_alloc(4096));

        for (size_t i = 0x1000; i <= buddy.get_highest_address(); i += page_size)
            map_fast(hypervisor_pagemap, i, i, 7);

        for (size_t i = 0; i <= GB * 4; i += page_size)
            map_fast(hypervisor_pagemap, i + 0xffff800000000000, i, 7);

        size_t e = 0;
        for (size_t i = kernel_address.response->physical_base; i < kernel_address.response->physical_base + 0x40000000UL; i += page_size, e += page_size)
            map_fast(hypervisor_pagemap, e + kernel_address.response->virtual_base, i, 7);

        wrcr3(reinterpret_cast<uint64_t>(hypervisor_pagemap));
        trace(TRACE_CPU, "Hypervisor pml4: 0x%lx", hypervisor_pagemap);
    }

    void map(struct Pml *pml4, size_t vaddr, size_t paddr, int flags)
    {
        Pml *pml3, *pml2, *pml1 = NULL;
        pml3 = next(pml4, index_of(vaddr, 4), flags);
        pml2 = next(pml3, index_of(vaddr, 3), flags);
        pml1 = next(pml2, index_of(vaddr, 2), flags);

        struct pte *L1 = &pml1->page_tables[index_of(vaddr, 1)];
        if (L1->address != 0)
            return;

        *L1 = paging_create_entry(paddr, flags);
        invlpg(vaddr);
    }

    void map_fast(struct Pml *pml4, size_t vaddr, size_t paddr, int flags)
    {
        struct Pml *pml3, *pml2, *pml1 = nullptr;
        pml3 = next(pml4, index_of(vaddr, 4), flags);
        pml2 = next(pml3, index_of(vaddr, 3), flags);
        pml1 = next(pml2, index_of(vaddr, 2), flags);

        pml1->page_tables[index_of(vaddr, 1)] = paging_create_entry(paddr, flags);
        invlpg(vaddr);
    }

    uint64_t index_of(uint64_t vaddr, int offset)
    {
        return vaddr >> (12 + 9 * (offset - 1)) & 0x1FF;
    }

    Pml *next(struct Pml *entry, size_t level, int flags)
    {
        auto pte = entry->page_tables[level];
        if (pte.present)
        {
            return (struct Pml *)(pte.address << page_shift);
        }

        uint64_t addr = (uint64_t)buddy.alloc(4096);
        assert_truth(addr != 0 && "Failed to allocate memory for a pagetable!");

        entry->page_tables[level] = paging_create_entry(addr, flags);
        return (struct Pml *)addr;
    }

    bool check_flag(int flags, int bit) { return (flags >> bit) & 1; }
    pte paging_create_entry(uint64_t paddr, int flags)
    {
        return (struct pte){
            .present = check_flag(flags, 0),
            .readwrite = check_flag(flags, 1),
            .supervisor = check_flag(flags, 2),
            .writethrough = 0,
            .cache_disabled = 0,
            .accessed = 0,
            .dirty = 0,
            .pagesize = 0, // PS/PAT bit
            .global = 0,   // Todo: set this
            .avail = 0,
            .address = paddr >> page_shift // PFN
        };
    }
} // namespace paging