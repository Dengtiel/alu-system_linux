#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
#include "multithreading.h"

/**
 * tprintf - prints formatted string preceded by calling thread ID
 * @format: format string
 *
 * Return: number of characters printed
 */
int tprintf(char const *format, ...)
{
	va_list args;
	int ret;

	va_start(args, format);
	printf("[%lu] ", pthread_self());
	ret = vprintf(format, args);
	va_end(args);
	return (ret);
}
