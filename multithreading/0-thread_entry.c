#include <stdio.h>
#include <pthread.h>
#include "multithreading.h"

/**
 * thread_entry - entry point for a new thread
 * @arg: address of string to print
 *
 * Return: NULL (never returns if called as thread entry)
 */
void *thread_entry(void *arg)
{
	printf("%s\n", (char *)arg);
	pthread_exit(NULL);
}
