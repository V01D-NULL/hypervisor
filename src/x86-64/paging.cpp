#include "x86-64/paging.hpp"

CREATE_SINGLETON(Paging, paging);

void Paging::init()
{
    trace(TRACE_CPU, "Copying page tables");
    uint64_t pml_boot = 0;
    pml4 = reinterpret_cast<Pml *>(buddy.alloc(page_size));

    asm volatile("movq %%cr3, %0" : "=r"(pml_boot));
    memcpy(static_cast<void *>(pml4), reinterpret_cast<const void *>(pml_boot), page_size);
    asm volatile("mov %0, %%cr3\n" ::"r"(pml4) : "memory");
    trace(TRACE_CPU, "Initialized paging");
}

void Paging::invalidatePage(const void *page)
{
    asm volatile("invlpg %0" ::"m"(page)
                 : "memory");
}

void Paging::invalidatePage(const uint64_t page)
{
    invalidatePage(reinterpret_cast<const void *>(page));
}

void Paging::traversePageTables(const uint64_t virtual_addr, const uint64_t physical_addr, AccessFlags access_flags, Pml *pml)
{
    int depth = 4;
    const int target_depth = 1;

    // Stop at pml1
    while (depth != target_depth)
    {
        pml = pml->next(getPmlOffset(virtual_addr, depth), access_flags);
        depth--;
    }

    // lowest_pml_index changes depending on the number of translation levels we use.
    // This can vary for 4K, 2M and 1G pages
    auto lowest_pml_index = getPmlOffset(virtual_addr, depth);
    pml->entries[lowest_pml_index].create(physical_addr, static_cast<int>(access_flags));
}

void Paging::map(uint64_t virtual_addr, uint64_t physical_addr, AccessFlags access_flags, Pml *root, const int page_size)
{
    traversePageTables(virtual_addr, physical_addr, access_flags, root);
    invalidatePage(virtual_addr);
}
