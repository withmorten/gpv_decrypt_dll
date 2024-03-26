#include <string.h>
#include <ctype.h>
#include <stdint.h>

typedef int32_t ssize_t;
#include "util.h"

ssize_t strscpy(char *dest, const char *src, size_t count)
{
	ssize_t res = 0;

	while (count)
	{
		char c = src[res];
		dest[res] = c;

		if (!c) return res;

		res++;
		count--;
	}

	if (res) dest[res - 1] = '\0';

	return -1;
}

ssize_t strsncpy(char *dest, const char *src, size_t count)
{
	ssize_t written = strscpy(dest, src, count);

	if (written < 0 || written == count - 1) return written;

	memset(dest + written + 1, 0, count - written - 1);

	return written;
}

char *strscat(char *dest, const char *src, size_t dest_size)
{
	if (!dest_size) return dest;

	size_t dest_content_len = strlen(dest);

	if ((dest_size - dest_content_len) <= 0) return dest;

	return strscpy(dest + dest_content_len, src, dest_size - dest_content_len) < 0 ? NULL : dest + dest_content_len;
}

ssize_t strscpychr(char *dest, const char *src, char end, size_t count)
{
	ssize_t res = 0;

	while (count)
	{
		char c = src[res];

		if (c == end)
		{
			dest[res] = '\0';
			return res;
		}

		dest[res] = c;

		if (!c) return res;

		res++;
		count--;
	}

	if (res) dest[res - 1] = '\0';

	return -1;
}

char *strtok_r(char *string, const char *control, char **nextoken)
{
	uint8_t *str;
	uint8_t *ctrl = (uint8_t *)control;

	uint8_t map[32];
	memset(map, 0, sizeof(map));

	do
	{
		map[*ctrl >> 3] |= (1 << (*ctrl & 7));
	}
	while (*ctrl++);

	if (string) str = (uint8_t *)string;
	else str = (uint8_t *)*nextoken;

	while ((map[*str >> 3] & (1 << (*str & 7))) && *str) str++;

	string = (char *)str;

	for (; *str; str++)
	{
		if (map[*str >> 3] & (1 << (*str & 7)))
		{
			*str++ = '\0';

			break;
		}
	}

	*nextoken = (char *)str;

	if ((uint8_t *)string == str) return NULL;
	else return string;
}

char *stristr(const char *str1, const char *str2)
{
	char *cp = (char *)str1;
	char *s1;
	char *s2;

	if (!*str2) return (char *)str1;

	while (*cp)
	{
		s1 = cp;
		s2 = (char *)str2;

		while (*s1 && *s2 && !(tolower(*s1) - tolower(*s2)))
		{
			s1++;
			s2++;
		}

		if (!*s2) return cp;

		cp++;
	}

	return NULL;
}
