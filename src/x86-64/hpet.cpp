#include "x86-64/hpet.hpp"
#include "acpi/acpi.hpp"
#include "memory.hpp"
#include "trace.hpp"

CREATE_SINGLETON(HpetTimer, hpet);

void HpetTimer::init()
{
    auto hpet_table = acpi.find_table<Hpet *>("HPET");
    if (!hpet_table)
    {
        trace(TRACE_ERROR, "HPET table not found");
        return;
    }

    base_address = reinterpret_cast<uint64_t>(phys_to_virt(hpet_table->address));
    trace(TRACE_CPU, "HPET base address: %#lx (phys: %#lx)", base_address, hpet_table->address);

    // Read capabilities register to get the counter period
    uint64_t capabilities = read_register(HpetReg::GENERAL_CAPABILITIES);
    period_fs = capabilities >> 32; // Upper 32 bits contain period in femtoseconds

    // Calculate frequency: freq = 10^15 / period_fs
    frequency = 1000000000000000ULL / period_fs;

    trace(TRACE_CPU, "HPET period: %lu fs, frequency: %lu Hz", period_fs, frequency);

    // Enable the main counter
    uint64_t config = read_register(HpetReg::GENERAL_CONFIG);
    config |= 1; // Set enable bit
    write_register(HpetReg::GENERAL_CONFIG, config);

    trace(TRACE_CPU, "HPET enabled");
}

uint64_t HpetTimer::read_counter() const
{
    return read_register(HpetReg::MAIN_COUNTER_VALUE);
}

void HpetTimer::sleep_ns(uint64_t nanoseconds)
{
    // Convert nanoseconds to HPET ticks
    // ticks = (nanoseconds * 10^6) / period_fs
    uint64_t ticks = (nanoseconds * 1000000ULL) / period_fs;

    uint64_t start = read_counter();
    uint64_t end = start + ticks;

    // Handle counter wraparound
    if (end < start)
    {
        // Wait for wraparound
        while (read_counter() >= start)
            asm volatile("pause");
    }

    // Wait until target count is reached
    while (read_counter() < end)
        asm volatile("pause");
}

void HpetTimer::write_register(uint64_t reg, uint64_t value)
{
    *reinterpret_cast<volatile uint64_t *>(base_address + reg) = value;
}

uint64_t HpetTimer::read_register(uint64_t reg) const
{
    return *reinterpret_cast<volatile uint64_t *>(base_address + reg);
}
