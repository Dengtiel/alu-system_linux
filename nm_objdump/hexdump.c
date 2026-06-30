#include "hobjdump.h"

/**
 * print_hex_line - print one hex dump line in objdump -s format
 * @addr: address of first byte in line
 * @buf: buffer of bytes
 * @len: number of bytes (1-16)
 * @bytes_per_line: always 16 for objdump -s
 */
void print_hex_line(unsigned long addr, unsigned char *buf,
		int len, int bytes_per_line)
{
	int i, j;

	printf(" %04lx", addr);
	for (i = 0; i < bytes_per_line; i++)
	{
		if (i % 4 == 0)
			printf(" ");
		if (i < len)
			printf("%02x", buf[i]);
		else
			printf("  ");
	}
	printf("  ");
	for (j = 0; j < len; j++)
	{
		if (buf[j] >= 0x20 && buf[j] < 0x7f)
			printf("%c", buf[j]);
		else
			printf(".");
	}
	for (j = len; j < bytes_per_line; j++)
		printf(" ");
	printf("\n");
}
