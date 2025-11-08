#pragma once

#include <stddef.h>
#include <stdint.h>

extern "C" void *memset(void *dest, int val, size_t len);
extern "C" void *memcpy(void *dest, const void *src, size_t len);

// Get the Higher Half Direct Map offset
uint64_t get_hhdm_offset();

// Convert physical address to virtual address
inline void *phys_to_virt(uint64_t phys)
{
    return reinterpret_cast<void *>(phys + get_hhdm_offset());
}