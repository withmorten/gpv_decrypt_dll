#pragma once

#include <stdio.h>
#include <stdarg.h>

ssize_t strscpy(char *dest, const char *src, size_t count);
ssize_t strsncpy(char *dest, const char *src, size_t count);
char *strscat(char *dest, const char *src, size_t dest_size);

ssize_t strscpychr(char *dest, const char *src, char end, size_t count);

template<size_t count> int snprintf(char (&string)[count], const char *format, ...)
{
	int ret;
	va_list args;
	va_start(args, format);
	ret = vsnprintf(string, count, format, args);
	va_end(args);
	return ret;
}

template<size_t count> int vsnprintf(char(&string)[count], const char *format, va_list ap)
{
	return vsnprintf(string, count, format, ap);
}

template<size_t count> __forceinline ssize_t strscpy(char (&dest)[count], const char *src)
{
	return strscpy(dest, src, count);
}

template<size_t count> __forceinline ssize_t strsncpy(char (&dest)[count], const char *src)
{
	return strsncpy(dest, src, count);
}

template<size_t count> __forceinline char *strscat(char (&dest)[count], const char *src)
{
	return strscat(dest, src, count);
}

template<size_t count> __forceinline ssize_t strscpychr(char (&dest)[count], const char *src, char end)
{
	return strscpychr(dest, src, end, count);
}

char *strtok_r(char *string, const char *control, char **nextoken);

char *stristr(const char *str1, const char *str2);
