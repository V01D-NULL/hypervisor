#pragma once

#include <stddef.h>
#include <stdint.h>

size_t strlen(const char *str);
char itoc(int num);
char itoh(int num, bool upper);
char *strrev(char *src);
void itoa(size_t num, char *str, int base);
void itoa(size_t num, char *str, int base, bool upper);
int atoi(const char *str);
int strncmp(const char *str1, const char *str2, size_t n);
int strcmp(const char *str1, const char *str2);
