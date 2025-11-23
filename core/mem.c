#include <mem.h>

void *memcpy(void *dest, const void *src, size_t n)
{
	char *d = (char *)dest;
	const char *s = (const char *)src;

	for (size_t i = 0; i < n; i++) {
		d[i] = s[i];
	}

	return dest;
}

void *memset(void *dest, int val, size_t len)
{
	unsigned char *ptr = dest;
	while (len-- > 0) {
		*ptr++ = (unsigned char)val;
	}
	return dest;
}

void memzero(void *ptr, size_t size)
{
	unsigned char *p = (unsigned char *)ptr;
	while (size--) {
		*p++ = 0;
	}
}
void *memmove(void *dest, const void *src, size_t n)
{
	unsigned char *d = (unsigned char *)dest;
	const unsigned char *s = (const unsigned char *)src;

	if (d == s) {
		return dest;
	}

	if (d < s) {
		while (n--)
		{
			*d++ = *s++;
		}
	} else {
		d += n;
		s += n;
		while (n--) {
			*(--d) = *(--s);
		}
	}

	return dest;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
	const unsigned char *p1 = (const unsigned char *)s1;
	const unsigned char *p2 = (const unsigned char *)s2;

	for (size_t i = 0; i < n; i++) {
		if (p1[i] != p2[i]) {
			return p1[i] - p2[i];
		}
	}
	return 0;
}
