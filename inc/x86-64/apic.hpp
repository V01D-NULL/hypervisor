#pragma once

#include "acpi/rsdt.hpp"
#include "compiler.hpp"
#include <stdint.h>

// Multiple APIC Description Table (MADT)
struct Madt {
    Sdt header;
    uint32_t lapic_address;
    uint32_t flags;
    uint8_t entries[];
} PACKED;

// MADT Entry Types
enum MadtEntryType : uint8_t {
    MADT_PROCESSOR_LOCAL_APIC = 0,
    MADT_IO_APIC = 1,
    MADT_INTERRUPT_SOURCE_OVERRIDE = 2,
    MADT_NMI = 3,
    MADT_LOCAL_APIC_NMI = 4,
    MADT_LOCAL_APIC_ADDRESS_OVERRIDE = 5,
    MADT_PROCESSOR_LOCAL_X2APIC = 9,
};

// MADT Entry Header
struct MadtEntryHeader {
    uint8_t type;
    uint8_t length;
} PACKED;

// Local APIC Entry
struct MadtLocalApic {
    MadtEntryHeader header;
    uint8_t processor_id;
    uint8_t apic_id;
    uint32_t flags;
} PACKED;

// I/O APIC Entry
struct MadtIoApic {
    MadtEntryHeader header;
    uint8_t io_apic_id;
    uint8_t reserved;
    uint32_t io_apic_address;
    uint32_t global_system_interrupt_base;
} PACKED;

// Local APIC Registers
enum LapicReg : uint32_t {
    ID = 0x20,
    VERSION = 0x30,
    TPR = 0x80,
    APR = 0x90,
    PPR = 0xA0,
    EOI = 0xB0,
    RRD = 0xC0,
    LDR = 0xD0,
    DFR = 0xE0,
    SPURIOUS = 0xF0,
    ISR = 0x100,
    TMR = 0x180,
    IRR = 0x200,
    ESR = 0x280,
    ICR_LOW = 0x300,
    ICR_HIGH = 0x310,
    TIMER = 0x320,
    THERMAL = 0x330,
    PERF = 0x340,
    LINT0 = 0x350,
    LINT1 = 0x360,
    ERROR = 0x370,
    TIMER_INITIAL_COUNT = 0x380,
    TIMER_CURRENT_COUNT = 0x390,
    TIMER_DIVIDE = 0x3E0
};

enum class ApicTimerDivide : uint32_t {
    BY_2 = 0b0000,
    BY_4 = 0b0001,
    BY_8 = 0b0010,
    BY_16 = 0b0011,
    BY_32 = 0b1000,
    BY_64 = 0b1001,
    BY_128 = 0b1010,
    BY_1 = 0b1011,
};

enum class ApicTimerMode : uint32_t {
    ONE_SHOT = 0b00 << 17,
    PERIODIC = 0b01 << 17,
    TSC_DEADLINE = 0b10 << 17,
};

class Apic
{
  public:
    void init();
    void enable();
    void eoi();

    void timer_init(uint8_t vector, ApicTimerMode mode, ApicTimerDivide divide, uint64_t interval_usec = 0);
    void timer_start(uint32_t initial_count);
    void timer_stop();
    uint32_t timer_get_current_count();
    void timer_calibrate();

    // Get timer frequency in Hz
    uint64_t get_timer_frequency() const { return timer_frequency; }
    uint64_t get_base() const { return lapic_base; }

  private:
    void write_register(uint32_t reg, uint32_t value);
    uint32_t read_register(uint32_t reg);

  private:
    uint64_t lapic_base{0};
    uint64_t timer_frequency{0};
    ApicTimerDivide current_divide{ApicTimerDivide::BY_1};
};

EXPOSE_SINGLETON(Apic, apic);
