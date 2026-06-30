#include <Python.h>
#include <stdio.h>

/**
 * print_python_bytes - prints basic info about a Python bytes object
 * @p: pointer to a PyObject
 */
void print_python_bytes(PyObject *p)
{
	PyBytesObject *b;
	Py_ssize_t size, i, limit;

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

	list = (PyListObject *)p;
	var = (PyVarObject *)p;
	size = var->ob_size;
	allocated = list->allocated;

	printf("[*] Python list info\n");
	printf("[*] Size of the Python List = %ld\n", size);
	printf("[*] Allocated = %ld\n", allocated);

	for (i = 0; i < size; i++)
	{
		item = list->ob_item[i];
		printf("Element %ld: %s\n", i,
			((PyObject *)(item))->ob_type->tp_name);
		if (PyBytes_Check(item))
			print_python_bytes(item);
	}
}
