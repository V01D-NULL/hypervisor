#pragma once

#include "atomic.hpp"
#include "compiler.hpp"
#include "list.hpp"
#include "trace.hpp"
#include "utility.hpp"
#include <limine.h>

static constexpr auto page_shift = 12;
static constexpr auto page_size = 4096;
extern uintptr_t GLOB_PAGE_ARRAY[], GLOB_PAGE_ARRAY_SIZE[];
static const auto page_array_sz = reinterpret_cast<uintptr_t>(GLOB_PAGE_ARRAY_SIZE);

enum class RawPageFlags : int {
    None = 0,
    Unusable = 1,
    Slab = 2
};

struct RawPage {
    RawPageFlags flags;
    int order;
    int buddy_index;
    atomic_uint refcount;

    bool is_buddy_page(int min_order) const { return order >= min_order; }
    void reset(bool reset_refcount = true)
    {
        flags = RawPageFlags::None;
        order = 0;
        if (likely(reset_refcount))
            refcount = 0;
    }
} PACKED;

class Pagelist
{
    using Index = int;
    using AddressType = uint64_t;

  public:
    void init(struct limine_memmap_response *memmap_response, size_t size)
    {
        // for (size_t i = 0; i < memmap_response->entry_count; i++)
		for (size_t i = 0; i < size; i += page_size)
        {
            // struct limine_memmap_entry *e = memmap_response->entries[i];

            // // Ensure the array size is not exceeded
            // auto const projected_index = largest_index + (e->length / 4096);
            // assert_truth(projected_index <= page_array_sz);

            // for (size_t j = 0; j <= e->length; j += 4096, largest_index++)
            // {
                // clang-format off
                auto const page = RawPage
         		{
                    // .flags = (e->type != LIMINE_MEMMAP_USABLE) ? RawPageFlags::Unusable : RawPageFlags::None
                };
                pages[largest_index] = move(page);
                // clang-format on
            // }
        }

        trace(TRACE_CPU, "RawPage size: %ld bytes", sizeof(RawPage));
        trace(TRACE_CPU, "Pagelist overhead: %ld Bytes", largest_index * sizeof(RawPage));

        // largest_index is incremented one too many times.
        --largest_index;
    }

    INLINE auto get_page(RawPage *p) const { return AddressType(pages - p); }
    INLINE auto max_page() const { return &pages[largest_index]; }
    INLINE auto min_page() const { return &pages[0]; }

    INLINE auto phys_to_page(uint64_t addr) const
    {
        return &pages[(addr >> page_shift) - 1];
    }

    INLINE auto page_to_phys(RawPage *p) const { return get_page(p) << page_shift; }

    auto operator[](Index i) const
    {
        auto const &page = phys_to_page(i);
        return array_operator{.page = page, .address = page_to_phys(page)};
    }

  private:
    struct array_operator {
        RawPage *page;
        AddressType address;
    };

    RawPage *pages = (struct RawPage *)GLOB_PAGE_ARRAY;
    Index largest_index{}; // largest index into the 'pages' array
};

static Pagelist pagelist;
