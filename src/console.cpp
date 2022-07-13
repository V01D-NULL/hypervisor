#include "console.hpp"
#include "bootloader_module.hpp"
#include "buddy.hpp"
#include "utility.hpp"
#include <limine.h>

USED struct limine_framebuffer_request fb {
    .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0, .response = nullptr
};

CREATE_SINGLETON(Console, console);

void Console::init()
{
    auto fb_response = move(fb.response);
    if (unlikely(!fb_response))
        return;

    struct framebuffer_t frm = {
        (uintptr_t)fb_response->framebuffers[0]->address, // Framebuffer address
        fb_response->framebuffers[0]->width,              // Framebuffer width
        fb_response->framebuffers[0]->height,             // Framebuffer height
        fb_response->framebuffers[0]->pitch               // Framebuffer pitch
    };

    extern uintptr_t _binary_src_ports_limine_terminal_port_fonts_vgafont_bin_start[];
    auto font_addr = (uintptr_t)_binary_src_ports_limine_terminal_port_fonts_vgafont_bin_start;

    struct font_t font = {
        font_addr, // Address of font file
        8,         // Font width
        16,        // Font height
        1,         // Character spacing
        0,         // Font scaling x
        0          // Font scaling y
    };

    struct style_t style = {
        DEFAULT_ANSI_COLOURS,        // Default terminal palette
        DEFAULT_ANSI_BRIGHT_COLOURS, // Default terminal bright palette
        0xA0000000,                  // Background colour
        0xFFFFFF,                    // Foreground colour
        0,                           // Terminal margin
        0                            // Terminal margin gradient
    };

    struct background_t back {
    };
    auto mod = find_bootloader_module("term_bg");
    if (mod)
    {
        struct image_t image;
        image_open(&image, (uint64_t)mod->address, mod->size);
        back = {
            &image,    // Background. Set to NULL to disable background
            STRETCHED, // STRETCHED, CENTERED or TILED
            0x00000000 // Terminal backdrop colour
        };
    }

    term_init(&term, nullptr, false); // if set to true memory is allocated for a vga terminal - we don't need that.
    term_vbe(&term, frm, font, style, back);
    initialized = true;
}

void Console::con_write(const char *str, size_t len)
{
    if (initialized)
        term_print(&term, str);

    e9_write(str, len);
}

void Console::e9_write(const char *str, size_t len)
{
    for (auto i = 0ul, n = len; i < n; i++)
        IO::out<char>(0xE9, str[i]);
}