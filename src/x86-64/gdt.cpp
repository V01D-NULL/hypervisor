#include "x86-64/gdt.hpp"
#include "trace.hpp"

CREATE_SINGLETON(Gdt, gdt);

void Gdt::init()
{
	// NULL, CS, DS
    set64(0, 0);
    set64(0x9A, 0x20);
    set64(0x92, 0x20);

    // clang-format off
	Gdtr gdtr {
		sizeof(segments) - 1,
		reinterpret_cast<uintptr_t>(segments)
	};
	// clang-format on
    
    lgdt(gdtr);
	reload_segments();
}

void Gdt::set64(uint8_t access, uint8_t granularity)
{
    // clang-format off
	segments[sel++] = {
		.access = access,
		.granularity = granularity,
	};
    // clang-format on
}