#include <stdlib.h>
#include "list.h"
#include "multithreading.h"

/**
 * prime_factors - Factorizes a number into prime factors
 * @s: String representation of the positive number
 *
 * Return: Pointer to a list of prime factors, or NULL on failure
 */
list_t *prime_factors(char const *s)
{
	list_t *factors;
	unsigned long number;
	unsigned long divisor;
	unsigned long values[sizeof(unsigned long) * 8];
	unsigned long *value;
	size_t count;
	size_t i;

	if (s == NULL)
		return (NULL);

	number = strtoul(s, NULL, 10);
	factors = list_create();
	if (factors == NULL)
		return (NULL);

	count = 0;

	while (number % 2 == 0)
	{
		values[count++] = 2;
		number /= 2;
	}

	for (divisor = 3; divisor <= number / divisor; divisor += 2)
	{
		while (number % divisor == 0)
		{
			values[count++] = divisor;
			number /= divisor;
		}
	}

	if (number > 1)
		values[count++] = number;

	for (i = count; i > 0; i--)
	{
		value = malloc(sizeof(*value));
		if (value == NULL)
		{
			list_destroy(factors, free);
			free(factors);
			return (NULL);
		}

		*value = values[i - 1];

		if (list_add(factors, value) == NULL)
		{
			free(value);
			list_destroy(factors, free);
			free(factors);
			return (NULL);
		}
	}

	return (factors);
}
