#include <Python.h>
#include <stdio.h>

/**
 * print_python_int - prints a Python integer
 * @p: pointer to a PyObject
 */
void print_python_int(PyObject *p)
{
	PyLongObject *l;
	Py_ssize_t ndigits, i;
	unsigned long int result = 0;
	unsigned long int digit;
	int negative;

	setbuf(stdout, NULL);

	if (!PyLong_Check(p))
	{
		printf("Invalid Int Object\n");
		return;
	}

	l = (PyLongObject *)p;
	ndigits = Py_SIZE(l);
	negative = (ndigits < 0);
	if (negative)
		ndigits = -ndigits;

	if (ndigits == 0)
	{
		printf("0\n");
		return;
	}

	/* Check for overflow: build value from digits */
	result = 0;
	for (i = ndigits - 1; i >= 0; i--)
	{
		digit = l->ob_digit[i];
		/* Check if shifting result would overflow */
		if (result > (unsigned long int)(-1) >> PyLong_SHIFT)
		{
			printf("C unsigned long int overflow\n");
			return;
		}
		result = (result << PyLong_SHIFT) | digit;
	}

	if (negative)
		printf("-%lu\n", result);
	else
		printf("%lu\n", result);
}
