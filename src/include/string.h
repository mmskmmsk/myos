#ifndef STRING_H
#define STRING_H

#include "stddef.h"

int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
size_t strlen(const char *s2);


// 文字列を連結
char *strcat(char *dest, const char *src);

#endif // STRING_H
