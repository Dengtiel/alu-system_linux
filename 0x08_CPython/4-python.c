#include <Python.h>
#include <stdio.h>

/**
 * print_python_string - prints basic info about a Python string object
 * @p: pointer to a PyObject
 */
void print_python_string(PyObject *p)
{
	PyUnicodeObject *s;
	Py_ssize_t length;
	const char *kind;

	setbuf(stdout, NULL);
	printf("[.] string object info\n");

	if (!PyUnicode_Check(p))
	{
		printf("  [ERROR] Invalid String Object\n");
		return;
	}

	s = (PyUnicodeObject *)p;
	length = ((PyASCIIObject *)s)->length;

	if (PyUnicode_IS_COMPACT_ASCII(p))
		kind = "compact ascii";
	else
		kind = "compact unicode object";

	printf("  type: %s\n", kind);
	printf("  length: %ld\n", length);
	printf("  value: %s\n", PyUnicode_AsUTF8(p));
}
