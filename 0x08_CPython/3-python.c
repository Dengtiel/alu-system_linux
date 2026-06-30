#include <Python.h>
#include <stdio.h>
#include <string.h>

/**
 * format_float - format a double like Python 3.4 repr (16 sig digits)
 * @val: the double value
 * @buf: output buffer (must be at least 64 bytes)
 */
static void format_float(double val, char *buf)
{
	char *p;

	PyOS_snprintf(buf, 64, "%.16g", val);
	/* Add .0 if no decimal point or exponent */
	if (strchr(buf, '.') == NULL && strchr(buf, 'e') == NULL
		&& strchr(buf, 'E') == NULL && strchr(buf, 'n') == NULL
		&& strchr(buf, 'N') == NULL)
	{
		p = buf + strlen(buf);
		*p++ = '.';
		*p++ = '0';
		*p = '\0';
	}
}

/**
 * print_python_float - prints basic info about a Python float object
 * @p: pointer to a PyObject
 */
void print_python_float(PyObject *p)
{
	PyFloatObject *f;
	char buf[64];

	setbuf(stdout, NULL);
	printf("[.] float object info\n");

	if (!PyFloat_Check(p))
	{
		printf("  [ERROR] Invalid Float Object\n");
		return;
	}

	f = (PyFloatObject *)p;
	format_float(f->ob_fval, buf);
	printf("  value: %s\n", buf);
}

/**
 * print_python_bytes - prints basic info about a Python bytes object
 * @p: pointer to a PyObject
 */
void print_python_bytes(PyObject *p)
{
	PyBytesObject *b;
	Py_ssize_t size, i, limit;

	setbuf(stdout, NULL);
	printf("[.] bytes object info\n");

	if (!PyBytes_Check(p))
	{
		printf("  [ERROR] Invalid Bytes Object\n");
		return;
	}

	b = (PyBytesObject *)p;
	size = ((PyVarObject *)(p))->ob_size;

	printf("  size: %ld\n", size);
	printf("  trying string: %s\n", b->ob_sval);

	limit = size + 1;
	if (limit > 10)
		limit = 10;

	printf("  first %ld bytes:", limit);
	for (i = 0; i < limit; i++)
		printf(" %02x", (unsigned char)b->ob_sval[i]);
	printf("\n");
}

/**
 * print_python_list - prints basic info about a Python list object
 * @p: pointer to a PyObject
 */
void print_python_list(PyObject *p)
{
	PyListObject *list;
	PyVarObject *var;
	PyObject *item;
	Py_ssize_t size, allocated, i;

	setbuf(stdout, NULL);
	printf("[*] Python list info\n");

	if (!PyList_Check(p))
	{
		printf("  [ERROR] Invalid List Object\n");
		return;
	}

	list = (PyListObject *)p;
	var = (PyVarObject *)p;
	size = var->ob_size;
	allocated = list->allocated;

	printf("[*] Size of the Python List = %ld\n", size);
	printf("[*] Allocated = %ld\n", allocated);

	for (i = 0; i < size; i++)
	{
		item = list->ob_item[i];
		printf("Element %ld: %s\n", i,
			((PyObject *)(item))->ob_type->tp_name);
		if (PyBytes_Check(item))
			print_python_bytes(item);
		else if (PyFloat_Check(item))
			print_python_float(item);
	}
}
