#include "../include/string.h"

int strcmp(const char *s1, const char *s2) {
	while (*s1 && (*s1 == *s2)) {
		s1++; s2++;
	}
	return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
	while (n && *s1 && (*s1 == *s2)) {
		s1++; s2++; n--;
	}
	if (n == 0) return 0;
	return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

size_t strlen(const char *s) {
	size_t len = 0;
	while (s[len]) len++;
	return len;
}

char *strcat(char *dest, const char *src) {
	char *d = dest;
	while (*d) d++; // destの末尾に移動
	while (*src) {
		*d++ = *src++; // srcの文字をコピー
	}
	*d = '\0'; // null終端
	return dest;
}

