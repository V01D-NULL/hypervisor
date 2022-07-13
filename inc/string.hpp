#include "compiler.hpp"
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

namespace string
{
    INLINE size_t strlen(const char *str)
    {
        size_t n = 0;
        while (*str++)
            n++;
        return n;
    }

    INLINE char itoc(int num) { return '0' + num; }

    INLINE char itoh(int num, bool upper)
    {
        if (upper)
            return num - 10 + 'A';
        return num - 10 + 'a';
    }

    INLINE char *strrev(char *src)
    {
        static char temp;
        int src_string_index = 0;
        int last_char = strlen(src) - 1;

        for (; src_string_index < last_char; src_string_index++)
        {
            temp = src[src_string_index]; // Save current character
            src[src_string_index] =
                src[last_char];    // Swap out the current char with the last char
            src[last_char] = temp; // Swap out last character with the current character
            last_char--;
        }

        src[strlen(src) - 1 + 1] = '\0';

        return src;
    }

    INLINE void itoa(size_t num, char *str, int base)
    {
        size_t buffer_sz = 20;
        size_t counter = 0;
        size_t digit = 0;

        while (num != 0 && counter < buffer_sz)
        {
            digit = (num % base);
            if (digit > 9)
            {
                str[counter++] = itoh(digit, false);
            } else
            {
                str[counter++] = itoc(digit);
            }
            num /= base;
        }

        str[counter] = '\0';
        strrev(str);
    }

    INLINE void itoa(size_t num, char *str, int base, bool upper)
    {
        size_t buffer_sz = 20;
        size_t counter = 0;
        size_t digit = 0;

        if (!upper)
        {
            itoa(num, str, base);
            return;
        } else
        {
            while (num != 0 && counter < buffer_sz - 1)
            {
                digit = (num % base);
                if (digit > 9)
                {
                    str[counter++] = itoh(digit, true);
                } else
                {
                    str[counter++] = itoc(digit);
                }
                num /= base;
            }
        }

        str[counter] = '\0';
        strrev(str);
    }

    INLINE int atoi(const char *str)
    {
        int ret = 0;
        int i;

        for (i = 0; str[i]; i++)
        {
            ret = ret * 10 + str[i] - '0';
        }

        return ret;
    }

    INLINE int strncmp(const char *str1, const char *str2, size_t n)
    {
        for (size_t i = 0; i < n && str1; i++)
        {
            if (str1[i] != str2[i])
                return -1;
        }
        return 0;
    }

    INLINE int strcmp(const char *str1, const char *str2)
    {
        size_t n = strlen(str1);
        for (size_t i = 0; i < n; i++)
        {
            if (str1[i] != str2[i])
                return -1;
        }
        return 0;
    }
} // namespace string