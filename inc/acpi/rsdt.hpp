#pragma once
#include "compiler.hpp"

struct Sdt {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char oem_string[6];
    char oem_table_id[8];
    uint32_t oem_revision;
    char creator_id[4];
    uint32_t creator_revision;

    inline bool validate_checksum() const
    {
        uint8_t sum{0};

        for (size_t i = 0; i < length; i++)
            sum += ((uint8_t *)this)[i];

        return sum == 0;
    }
} PACKED;

struct Rsdt {
    Sdt header;
    uint32_t next[];
} PACKED;

struct Xsdt {
    Sdt header;
    uint64_t next[];
} PACKED;