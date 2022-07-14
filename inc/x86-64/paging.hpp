#pragma once

#include "compiler.hpp"
#include <stdint.h>
#include <limine.h>

namespace paging
{
    struct pte {
        uint8_t present : 1;
        uint8_t readwrite : 1;
        uint8_t supervisor : 1;
        uint8_t writethrough : 1;
        uint8_t cache_disabled : 1;
        uint8_t accessed : 1;
        uint8_t dirty : 1;
        uint8_t pagesize : 1;
        uint8_t global : 1;
        uint8_t avail : 3;
        uint64_t address : 52;
    } PACKED;

    struct Pml {
        struct pte page_tables[512];
    };

	void init();
	void map(struct Pml *pml4, size_t vaddr, size_t paddr, int flags);
	void map_fast(struct Pml *pml4, size_t vaddr, size_t paddr, int flags);
} // namespace paging

using pte_t = uint64_t;
using virt_t = uint64_t;
using phys_t = uint64_t;

constexpr size_t page_offset = 12;  //Lower 12 bits of a virtual address denote the offset in the page frame - We don't need that

struct walk_t {
    int64_t idx;
    pte_t *pml1;
};

class VirtualMemoryManager {
public:
    VirtualMemoryManager(bool initial_mapping = false, limine_memmap_response *mmap = nullptr);

public:
    void map(phys_t physical_addr, virt_t virtual_addr, uint64_t access_flags, pte_t *pml_ptr);
    inline pte_t *get_kernel_pml4() {
        return this->kernel_pml4;
    }

private:
    pte_t *kernel_pml4;

private:
    int64_t get_index(virt_t virtual_addr, const int idx) {
        // 9 bits per index - 9 * idx = index 'idx' in virtual_address, skip the first 12 bits
        return (virtual_addr >> (page_offset + (9 * (idx - 1)))) & 0x1FF;  // We subtract 1 from idx so that we don't have to input idx 0-3, but rather 1-4
    }

private:
    void configure_initial_kernel_mapping(limine_memmap_response *mmap);
    walk_t walk(virt_t virtual_addr, pte_t *pml_ptr, uint64_t access_flags);
};