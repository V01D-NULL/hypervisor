#include "x86-64/paging.hpp"
#include "buddy.hpp"
#include "page.hpp"

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
        hypervisor_pagemap = reinterpret_cast<Pml *>(buddy.alloc(4096));
		for (size_t i = 0; i < 0x40000000UL*4; i += page_size)
            map_fast(hypervisor_pagemap, i, i, 3);

		for (size_t i = 0; i < 0x40000000UL * 4; i += page_size)
            map_fast(hypervisor_pagemap, i + 0xffff800000000000, i, 3);

        for (size_t i = 0; i < 0x40000000UL * 2; i += page_size)
            map_fast(hypervisor_pagemap, i + 0xffffffff80000000, i, 1);

		trace(TRACE_CPU, "%p", hypervisor_pagemap);
		wrcr3(reinterpret_cast<uint64_t>(hypervisor_pagemap));
		for(;;)
			;
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
		// trace(TRACE_CPU, "%lx", addr);
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

#include <stddef.h>
#include "x86-64/paging.hpp"
#include "trace.hpp"
#include "buddy.hpp"

constexpr size_t GB = 0x40000000UL;

VirtualMemoryManager::VirtualMemoryManager(bool initial_mapping, limine_memmap_response *mmap) {
    if (initial_mapping)
        this->configure_initial_kernel_mapping(mmap);
}

void VirtualMemoryManager::configure_initial_kernel_mapping(limine_memmap_response *mmap) {
    auto pml4 = buddy.alloc(4096);
    if (pml4 == nullptr) console.panic("Failed to allocate memory for the kernel pml4");
    this->kernel_pml4 = static_cast<pte_t *>(pml4);
	trace(TRACE_CPU, "kernel_pml4: %p", this->kernel_pml4);

    // for (size_t n = 4096; n < GB * 4; n += 4096) {
    //     this->map(n, n, 0x3, kernel_pml4);
    // }
    
    // for (auto i = 0ul; i < mmap->entry_count; i++) {
    //     auto entry = mmap->entries[i];
	// 	if (entry->type != LIMINE_MEMMAP_RESERVED)
	// 	{
			// auto base = 0x7ff00000;
			// auto top =  0x80000000;
			// trace(TRACE_CPU, "reserved memory [%lx-%lx]", base, top);
			// for (size_t n = base; n < top; n+=4096)
			// {
			// 	// trace(TRACE_CPU, "mapping 0x%lx :: 0x%lx", n, 0xffff800000000000 + n);
			// 	this->map(n, 0xffff800000000000 + n, 3, kernel_pml4);
			// }
		// }
    //     if (entry.type == STIVALE2_MMAP_USABLE) {
    //         auto base = entry.base;
    //         auto top = base + entry.length;
    //         for (; base != top; base += 4096) {
    //             this->map(base, base, 0x3, kernel_pml4);
    //         }
    //     }
    // }

    // for (size_t n = 0; n < 4 * GB; n += 4096) {
    //     this->map(n, n + 0xffff800000000000, 0x3, kernel_pml4);
    // }

    // for (size_t base = 0; base < 2 * GB; base += 4096) {
    //     this->map(base, base + 0xffffffff80000000, 0x3, kernel_pml4);
    // }

	for (size_t i = 0x1000; i <= 0x180000000; i += page_size)
		this->map(i, i, 3, kernel_pml4);

	for (size_t i = 0; i <= 0x180000000; i += page_size)
		this->map(i, i + 0xffff800000000000, 3, kernel_pml4);

	for (size_t i = 0; i <= 0xf000; i+= page_size)
		this->map(i, i + 0xffffffff80000000, 1, kernel_pml4);

	for (size_t i = 0xf000; i <= 0x85012000; i += page_size)
		this->map(i, i + 0xffffffff80000000, 3, kernel_pml4);

    asm volatile("mov %0, %%cr3\n" ::"r"(this->kernel_pml4) : "memory");
	for(;;)
		;
	trace(TRACE_CPU, "vmm: Initialized");
    // info_logger << "vmm: Initialized" << logger::endl;
}

walk_t VirtualMemoryManager::walk(virt_t virtual_addr, pte_t *pml_ptr, uint64_t access_flags) {
    auto idx4 = this->get_index(virtual_addr, 4);
    auto idx3 = this->get_index(virtual_addr, 3);
    auto idx2 = this->get_index(virtual_addr, 2);
    auto idx1 = this->get_index(virtual_addr, 1);

    if (!(pml_ptr[idx4] & 1)) {
        auto ptr = buddy.alloc(4096);
        if (ptr == nullptr) console.panic("OOM");
        this->kernel_pml4[idx4] = reinterpret_cast<pte_t>(ptr);
        this->kernel_pml4[idx4] |= access_flags;
    }
    auto pml3 = (pte_t *)(this->kernel_pml4[idx4] & ~(511));

    if (!(pml3[idx3] & 1)) {
        auto ptr = buddy.alloc(4096);
        if (ptr == nullptr) console.panic("OOM");
        pml3[idx3] = reinterpret_cast<pte_t>(ptr);
        pml3[idx3] |= access_flags;
    }
    auto pml2 = (pte_t *)(pml3[idx3] & ~(511));

    if ((pml2[idx2] & 1) == 0) {
        auto ptr = buddy.alloc(4096);
        if (ptr == nullptr) console.panic("OOM");
        pml2[idx2] = reinterpret_cast<pte_t>(ptr);
        pml2[idx2] |= access_flags;
    }
    return {
        .idx = idx1,
        .pml1 = (pte_t *)(pml2[idx2] & ~(511))
    };
}

void VirtualMemoryManager::map(phys_t physical_addr, virt_t virtual_addr, uint64_t access_flags, pte_t *pml_ptr) {
    auto table_walk_result = walk(virtual_addr, pml_ptr, access_flags);
    table_walk_result.pml1[table_walk_result.idx] = (physical_addr | access_flags);
    asm volatile("invlpg %0" ::"m"(virtual_addr)
                 : "memory");
}