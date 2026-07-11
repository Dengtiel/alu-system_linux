#include <stdlib.h>
#include <stdio.h>
#include "multithreading.h"
#include "list.h"

/**
 * prime_factors - factorizes a number into prime factors
 * @s: string representation of the number
 *
 * Return: pointer to list of prime factors, or NULL on failure
 */
list_t *prime_factors(char const *s)
{
	unsigned long n, factor;
	unsigned long *val;
	list_t *list;

	n = strtoul(s, NULL, 10);
	list = list_create();
	if (!list)
		return (NULL);

	factor = 2;
	while (factor * factor <= n)
	{
		while (n % factor == 0)
		{
			val = malloc(sizeof(unsigned long));
			if (!val)
				return (list);
			*val = factor;
			list_add_tail(list, val);
			n /= factor;
		}
		factor++;
	}
	if (n > 1)
	{
		val = malloc(sizeof(unsigned long));
		if (!val)
			return (list);
		*val = n;
		list_add_tail(list, val);
	}
	return (list);
}
