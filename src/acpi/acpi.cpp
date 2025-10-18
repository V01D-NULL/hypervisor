#include "acpi/acpi.hpp"
#include "acpi/rsdt.hpp"
#include "compiler.hpp"
#include "string.hpp"
#include "trace.hpp"
#include <limine.h>

CREATE_SINGLETON(Acpi, acpi);

USED struct limine_rsdp_request rsdp_request {
    .id = LIMINE_RSDP_REQUEST,
    .revision = 0,
    .response = nullptr,
};

void Acpi::init()
{
    verify_checksum();
    rsdp = *reinterpret_cast<Rsdp *>(rsdp_request.response->address);
    rsdt = reinterpret_cast<Rsdt *>(rsdp.rsdt_address);

    trace(TRACE_CPU, "acpi oem: %s", rsdp.oem_string);

    if (rsdp.revision >= 2)
    {
        has_xsdt = true;
        xsdt = reinterpret_cast<Xsdt *>(rsdp.xsdt_address);
    }
}

void Acpi::verify_checksum() const
{
    int checksum = 0;
    auto ptr = static_cast<uint8_t *>(rsdp_request.response->address);

    for (int i = 0; i < 20; i++)
        checksum += *ptr++;

    assert_truth(((checksum & 0xFF) == 0x00) && "Invalid rsdp checksum\n");
}