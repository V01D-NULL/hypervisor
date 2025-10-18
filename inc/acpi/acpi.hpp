#pragma once

#include "rsdp.hpp"
#include "rsdt.hpp"
#include "string.hpp"

class Acpi
{
  public:
    void init();

    template <typename T>
    T find_table(const char *name) const
    {
        auto header = has_xsdt ? xsdt->header : rsdt->header;
        int entry_count = (header.length - sizeof(header)) / (has_xsdt ? 8 : 4);

        for (int i = 0; i < entry_count; i++)
        {
            auto sdt = reinterpret_cast<Sdt *>(has_xsdt ? xsdt->next[i] : rsdt->next[i]);

            if (!string::strncmp(sdt->signature, name, 4) && sdt->validate_checksum())
            {
                return reinterpret_cast<T>(sdt);
            }
        }

        return nullptr;
    }

  private:
    void verify_checksum() const;

  private:
    Rsdp rsdp;
    bool has_xsdt{false};
    Xsdt *xsdt{nullptr};
    Rsdt *rsdt{nullptr};
};

EXPOSE_SINGLETON(Acpi, acpi);