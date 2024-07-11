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

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

void reverse(char *str, int length) {
    int start = 0;
    int end = length - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

char* itoa(int num, char* str, int base) {
    int i = 0;
    int is_negative = 0;

    // Handle 0 explicitly, otherwise empty string is printed
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // Handle negative numbers for bases other than 10
    if (num < 0 && base != 10) {
        return NULL;  // Unsupported for non-decimal bases
    }

    // Handle negative numbers for base 10
    if (num < 0 && base == 10) {
        is_negative = 1;
        num = -num;
    }

    // Process individual digits
    while (num != 0) {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // Add negative sign for base 10
    if (is_negative && base == 10) {
        str[i++] = '-';
    }

    str[i] = '\0';  // Null terminate string

    // Reverse the string
    reverse(str, i);

    return str;
}
