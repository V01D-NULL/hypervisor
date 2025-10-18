#pragma once

#include "compiler.hpp"

struct Rsdp {
    // Version 1.0
    char signature[8];
    uint8_t checksum;
    char oem_string[6];
    uint8_t revision;
    uint32_t rsdt_address;

    // Version 2.0+
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t ext_checksum;
    uint8_t reserved[3];
} PACKED;