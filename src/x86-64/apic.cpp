#include "x86-64/apic.hpp"
#include "acpi/acpi.hpp"
#include "compiler.hpp"
#include "cpu.hpp"
#include "memory.hpp"
#include "trace.hpp"
#include "x86-64/hpet.hpp"
#include "x86-64/pic.hpp"

CREATE_SINGLETON(Apic, apic);

void Apic::init()
{
    auto madt = acpi.find_table<Madt *>("APIC");
    if (!madt)
    {
        trace(TRACE_ERROR, "Failed to find MADT table");
        return;
    }

    uint64_t phys_lapic = madt->lapic_address;
    lapic_base = reinterpret_cast<uint64_t>(phys_to_virt(phys_lapic));

    trace(TRACE_CPU, "Local APIC base address: %#lx (phys: %#lx)", lapic_base, phys_lapic);

    // Parse MADT entries
    uint32_t entries_length = madt->header.length - sizeof(Madt);
    uint8_t *entry_ptr = madt->entries;
    uint8_t *entries_end = entry_ptr + entries_length;

    while (entry_ptr < entries_end)
    {
        auto header = reinterpret_cast<MadtEntryHeader *>(entry_ptr);

        switch (header->type)
        {
        case MADT_PROCESSOR_LOCAL_APIC: {
            auto local_apic = reinterpret_cast<MadtLocalApic *>(entry_ptr);
            trace(TRACE_CPU, "Local APIC - Processor ID: %u, APIC ID: %u, Flags: %#x",
                  local_apic->processor_id, local_apic->apic_id, local_apic->flags);
            break;
        }
        case MADT_IO_APIC: {
            auto io_apic = reinterpret_cast<MadtIoApic *>(entry_ptr);
            trace(TRACE_CPU, "I/O APIC - ID: %u, Address: %#x, GSI Base: %u",
                  io_apic->io_apic_id, io_apic->io_apic_address,
                  io_apic->global_system_interrupt_base);
            break;
        }
        }

        entry_ptr += header->length;
    }

    enable();
}

void Apic::enable()
{
    pic.disable();

    // Enable APIC by setting spurious interrupt vector register
    // Bit 8 enables the APIC
    write_register(LapicReg::SPURIOUS, 0x100 | 0xFF);

    trace(TRACE_CPU, "Local APIC enabled");
}

void Apic::eoi()
{
    write_register(LapicReg::EOI, 0);
}

void Apic::timer_init(uint8_t vector, ApicTimerMode mode, ApicTimerDivide divide, uint64_t interval_usec)
{
    current_divide = divide;
    write_register(LapicReg::TIMER_DIVIDE, static_cast<uint32_t>(divide));

    uint32_t lvt_timer = vector | static_cast<uint32_t>(mode);
    write_register(LapicReg::TIMER, lvt_timer);

    trace(TRACE_CPU, "APIC timer initialized - Vector: %u, Mode: %#x, Divide: %#x",
          vector, static_cast<uint32_t>(mode), static_cast<uint32_t>(divide));

    // If interval is specified and timer is calibrated, calculate and set the initial count
    if (interval_usec > 0 && timer_frequency > 0)
    {
        uint32_t divide_value = 0;
        switch (divide)
        {
        case ApicTimerDivide::BY_1:
            divide_value = 1;
            break;
        case ApicTimerDivide::BY_2:
            divide_value = 2;
            break;
        case ApicTimerDivide::BY_4:
            divide_value = 4;
            break;
        case ApicTimerDivide::BY_8:
            divide_value = 8;
            break;
        case ApicTimerDivide::BY_16:
            divide_value = 16;
            break;
        case ApicTimerDivide::BY_32:
            divide_value = 32;
            break;
        case ApicTimerDivide::BY_64:
            divide_value = 64;
            break;
        case ApicTimerDivide::BY_128:
            divide_value = 128;
            break;
        }

        // Calculate ticks: (frequency / divide) * (microseconds / 1000000)
        uint64_t effective_freq = timer_frequency / divide_value;
        uint32_t ticks = (effective_freq * interval_usec) / 1000000;

        trace(TRACE_CPU, "Calculated %u ticks for %lu usec interval", ticks, interval_usec);
        timer_start(ticks);
    }
}

void Apic::timer_start(uint32_t initial_count)
{
    write_register(LapicReg::TIMER_INITIAL_COUNT, initial_count);
    trace(TRACE_CPU, "APIC timer started with initial count: %u", initial_count);
}

void Apic::timer_stop()
{
    write_register(LapicReg::TIMER_INITIAL_COUNT, 0);
    trace(TRACE_CPU, "APIC timer stopped");
}

uint32_t Apic::timer_get_current_count()
{
    return read_register(LapicReg::TIMER_CURRENT_COUNT);
}

void Apic::timer_calibrate()
{
    trace(TRACE_CPU, "Calibrating APIC timer using HPET...");

    // Temporarily set divide to 1 for maximum precision
    ApicTimerDivide saved_divide = current_divide;
    write_register(LapicReg::TIMER_DIVIDE, static_cast<uint32_t>(ApicTimerDivide::BY_1));
    write_register(LapicReg::TIMER_INITIAL_COUNT, 0xFFFFFFFF);

    // Read starting APIC counter
    uint32_t start_apic = read_register(LapicReg::TIMER_CURRENT_COUNT);

    // Sleep for 10 milliseconds
    hpet.sleep_ns(10000000); // 10ms = 10,000,000 nanoseconds

    // Read ending APIC counter
    uint32_t end_apic = read_register(LapicReg::TIMER_CURRENT_COUNT);

    // Stop the timer
    write_register(LapicReg::TIMER_INITIAL_COUNT, 0);

    // Calculate ticks elapsed (counter counts down)
    uint32_t ticks_elapsed = start_apic - end_apic;

    // Calculate frequency: ticks per 10ms = (ticks * 100) per second
    timer_frequency = static_cast<uint64_t>(ticks_elapsed) * 100;

    trace(TRACE_CPU, "APIC timer frequency: %lu Hz (bus frequency with divide=1)", timer_frequency);

    // Restore the divide setting
    write_register(LapicReg::TIMER_DIVIDE, static_cast<uint32_t>(saved_divide));
}

void Apic::write_register(uint32_t reg, uint32_t value)
{
    *reinterpret_cast<volatile uint32_t *>(lapic_base + reg) = value;
}

uint32_t Apic::read_register(uint32_t reg)
{
    return *reinterpret_cast<volatile uint32_t *>(lapic_base + reg);
}
