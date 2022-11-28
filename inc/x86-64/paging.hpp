#pragma once

#include "compiler.hpp"
#include <limine.h>
#include <stdint.h>

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