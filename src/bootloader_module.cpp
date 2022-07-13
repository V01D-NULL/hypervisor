#include "bootloader_module.hpp"
#include "compiler.hpp"
#include "io.hpp"
#include "utility.hpp"
#include "string.hpp"

USED struct limine_module_request modules {
    .id = LIMINE_MODULE_REQUEST, .revision = 0, .response = nullptr
};

limine_file *find_bootloader_module(const char *module_name)
{
    if (modules.response->module_count == 0)
        return nullptr;

    for (int i = 0; i < modules.response->module_count; i++)
        if (!string::strcmp(module_name, modules.response->modules[i]->cmdline))
            return move(modules.response->modules[i]);

    return nullptr;
}