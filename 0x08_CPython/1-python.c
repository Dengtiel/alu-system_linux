#include <Python.h>
#include <stdio.h>

/**
 * print_python_list - prints basic info about a Python list
 * @p: pointer to a PyObject (must be a list)
 */
void print_python_list(PyObject *p)
{
	Py_ssize_t size, allocated, i;
	PyListObject *list;
	PyObject *item;

	list = (PyListObject *)p;
	size = Py_SIZE(p);
	allocated = list->allocated;

	printf("[*] Python list info\n");
	printf("[*] Size of the Python List = %ld\n", size);
	printf("[*] Allocated = %ld\n", allocated);

	for (i = 0; i < size; i++)
	{
		item = list->ob_item[i];
		printf("Element %ld: %s\n", i, Py_TYPE(item)->tp_name);
	}
}
