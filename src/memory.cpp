#include "memory.hpp"
#include <limine.h>

// Create buddy singleton
#include "buddy.hpp"
CREATE_SINGLETON(BuddyManager, buddy);

USED struct limine_hhdm_request hhdm_request {
    .id = LIMINE_HHDM_REQUEST, .revision = 0, .response = nullptr
};

uint64_t get_hhdm_offset()
{
    return hhdm_request.response->offset;
}

void *memset(void *dest, int val, size_t len)
{
    uint8_t *ptr = (uint8_t *)dest;

    while (len--)
    {
        *ptr++ = val;
    }

    return dest;
}

void *memcpy(void *dest, const void *src, size_t len)
{
    uint8_t *_dest = (uint8_t *)dest;
    uint8_t *_src = (uint8_t *)src;

    for (; len != 0; len--)
    {
        *_dest = *_src;

        _dest++;
        _src++;
    }

    return dest;
}