#pragma once

#include "acpi/rsdt.hpp"
#include "compiler.hpp"
#include <stdint.h>

// HPET ACPI Table
struct Hpet {
    Sdt header;
    uint8_t hardware_rev_id;
    uint8_t comparator_count : 5;
    uint8_t counter_size : 1;
    uint8_t reserved : 1;
    uint8_t legacy_replacement : 1;
    uint16_t pci_vendor_id;
    uint8_t address_space_id;
    uint8_t register_bit_width;
    uint8_t register_bit_offset;
    uint8_t reserved2;
    uint64_t address;
    uint8_t hpet_number;
    uint16_t minimum_tick;
    uint8_t page_protection;
} PACKED;

// HPET Register Offsets
enum HpetReg : uint64_t {
    GENERAL_CAPABILITIES = 0x00,
    GENERAL_CONFIG = 0x10,
    GENERAL_INT_STATUS = 0x20,
    MAIN_COUNTER_VALUE = 0xF0,
    TIMER0_CONFIG = 0x100,
    TIMER0_COMPARATOR = 0x108,
};

class HpetTimer
{
  public:
    void init();

    uint64_t read_counter() const;
    void sleep_ns(uint64_t nanoseconds);

    // Get the counter frequency in Hz
    uint64_t get_frequency() const { return frequency; }

    // Get the period in femtoseconds (10^-15 seconds)
    uint64_t get_period_fs() const { return period_fs; }

  private:
    void write_register(uint64_t reg, uint64_t value);
    uint64_t read_register(uint64_t reg) const;

  private:
    uint64_t base_address{0};
    uint64_t period_fs{0}; // Period in femtoseconds
    uint64_t frequency{0}; // Frequency in Hz
};

EXPOSE_SINGLETON(HpetTimer, hpet);
