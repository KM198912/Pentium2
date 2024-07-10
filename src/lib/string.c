#include <lib/string.h>
void *memcpy(void *dest, const void *src, size_t n) {
    char *d = dest;
    const char *s = src;

    while (n--) {
        *d++ = *s++;
    }

    return dest;
}

void *memset(void *s, int c, size_t n) {
    unsigned char *p = s;
    unsigned char uc = c;

    while (n-- > 0) {
        *p++ = uc;
    }

    return s;
}