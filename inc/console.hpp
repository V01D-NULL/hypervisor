#pragma once

#include "c_string.hpp"
#include "cpu.hpp"
#include "format.hpp"
#include "io.hpp"
#include "limine-terminal-port/source/term.h"

class Console
{
  public:
    void init();

    void print(const char *fmt, ...)
    {
        char buff[512]{};
        va_list ap;
        va_start(ap, fmt);
        vsnprintf((char *)&buff, (size_t)-1, fmt, ap);
        va_end(ap);

        *(buff + strlen(buff)) = '\n';
        con_write(buff, strlen(buff));
    }

    void panic(const char *fmt, ...)
    {
        char buff[512]{};
        va_list ap;
        va_start(ap, fmt);
        vsnprintf((char *)&buff, (size_t)-1, fmt, ap);
        va_end(ap);

        con_write(buff, strlen(buff));
        halt();
    }

  private:
    struct term_t term;
    bool initialized{};
    static constexpr const char *color_codes[] = {
        "\033[31m", // Red
        "\033[32m", // Green
        "\033[33m", // Yellow
        "\033[35m", // Magenta
        "\033[36m", // Cyan
        "\033[37m", // White
        "\033[90m", // Bright Black (Gray)
        "\033[91m", // Bright Red
        "\033[92m", // Bright Green
        "\033[93m", // Bright Yellow
        "\033[94m", // Bright Blue
        "\033[95m", // Bright Magenta
        "\033[96m", // Bright Cyan
        "\033[97m"  // Bright White
    };

  private:
    void con_write(const char *str, size_t len);
    void e9_write(const char *str, size_t len);
};

EXPOSE_SINGLETON(Console, console);