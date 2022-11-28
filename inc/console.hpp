#pragma once

#include "cpu.hpp"
#include "format.hpp"
#include "io.hpp"
#include "limine-terminal-port/source/term.h"
#include "string.hpp"

class Console
{
  private:
    struct term_t term;
    bool initialized{}, automatic_newline{};

  public:
    void init();

    void print(const char *fmt, ...)
    {
        char buff[512]{};
        va_list ap;
        va_start(ap, fmt);
        vsnprintf((char *)&buff, (size_t)-1, fmt, ap);
        va_end(ap);

		if (automatic_newline)
        	*(buff + string::strlen(buff)) = '\n';
		
        con_write(buff, string::strlen(buff));
    }

    void _panic(const char *fmt, ...)
    {
        char buff[512]{};
        va_list ap;
        va_start(ap, fmt);
        vsnprintf((char *)&buff, (size_t)-1, fmt, ap);
        va_end(ap);

        *(buff + string::strlen(buff)) = '\n';
        con_write(buff, string::strlen(buff));
        halt();
    }

    void auto_newline(bool state = true) { automatic_newline = state; }

  private:
    void con_write(const char *str, size_t len);
    void e9_write(const char *str, size_t len);
};

EXPOSE_SINGLETON(Console, console);

void put_pixel(int x, int y, int color);