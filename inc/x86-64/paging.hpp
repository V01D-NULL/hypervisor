#pragma once
#include "buddy.hpp"
#include "compiler.hpp"
#include <stdint.h>

enum class AccessFlags : int {
    Readonly = 1,
    ReadWrite = 3,
    UserReadOnly = 5,
    UserReadWrite = 7
};

struct StructuredPte {
    uint8_t present;
    uint64_t address;
};

union Pte {
    uint64_t All;

    struct {
        uint8_t present : 1;
        uint8_t readwrite : 1;
        uint8_t supervisor : 1;
        uint8_t writethrough : 1;
        uint8_t cache_disabled : 1;
        uint8_t accessed : 1;
        uint8_t dirty : 1;
        uint8_t pagesize : 1;
        uint8_t global : 1;
        uint8_t ignore : 3;
        uint64_t address : 52;
    } fields PACKED;

    // A hack to use structured binding with a filter (only select a few elements)
    inline StructuredPte destructure() const
    {
        return {fields.present, fields.address};
    }

    inline void create(uint64_t addr, int flags)
    {
        auto check_flags = [&](int flags, int bit) {
            return static_cast<uint8_t>((flags >> bit) & 1);
        };

        this->fields = {
            .present = check_flags(flags, 0),
            .readwrite = check_flags(flags, 1),
            .supervisor = check_flags(flags, 2),
            .writethrough = 0,
            .cache_disabled = 0,
            .accessed = 0,
            .dirty = 0,
            .pagesize = 0,
            .global = 0,
            .ignore = 0,
            .address = addr >> page_shift // Page Frame Number
        };
    }
};

// Pml => Page map level
// (A vague term to help define Pml{1,2,3,4} or PDPT,PDP,PD,PT)
struct Pml {
    Pte entries[512] = {};

    // Retrieve the next pml, creating it if necessary.
    inline Pml *next(int depth, AccessFlags flags)
    {
        const auto &pte = this->entries[depth];
        auto [present, address] = pte.destructure();

        if (present)
        {
            return reinterpret_cast<Pml *>(address << 12);
        }

        uint64_t addr = reinterpret_cast<uint64_t>(buddy.alloc(page_size));
        if (!addr)
            return nullptr;

        this->entries[depth].create(addr, static_cast<int>(flags));
        return reinterpret_cast<Pml *>(addr);
    };
} PACKED;

class Paging
{
  public:
    void init();
    void invalidatePage(const void *page);
    void invalidatePage(const uint64_t page);
    void map(uint64_t virtual_addr, uint64_t physical_addr, AccessFlags access_flags, Pml *root, const int page_size);

  private:
    void traversePageTables(const uint64_t virtual_addr, const uint64_t physical_addr, AccessFlags access_flags, Pml *pml);

  private:
    INLINE int64_t getPmlOffset(const uint64_t virtual_addr, const int depth)
    {
        return (virtual_addr >> (12 + (9 * (depth - 1)))) & 0x1FF;
    }

    void writeCr3(Pml *pml) { asm volatile("mov %0, %%cr3\n" ::"r"(pml4) : "memory"); }
    int readCr3()
    {
        int pml;
        asm volatile("movq %%cr3, %0" : "=r"(pml));
        return pml;
    }

  private:
    Pml *pml4;
};

EXPOSE_SINGLETON(Paging, paging);
