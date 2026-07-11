#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>
#include "multithreading.h"

static pthread_mutex_t tprintf_mutex;

/**
 * tprintf_init - initialize the mutex (constructor)
 */
static void __attribute__((constructor)) tprintf_init(void)
{
	pthread_mutex_init(&tprintf_mutex, NULL);
}

/**
 * tprintf_destroy - destroy the mutex (destructor)
 */
static void __attribute__((destructor)) tprintf_destroy(void)
{
	pthread_mutex_destroy(&tprintf_mutex);
}

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
	pthread_mutex_lock(&tprintf_mutex);
	printf("[%lu] ", pthread_self());
	ret = vprintf(format, args);
	pthread_mutex_unlock(&tprintf_mutex);
	va_end(args);
	return (ret);
}
