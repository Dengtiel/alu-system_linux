#include "hobjdump.h"

/**
 * hex_addr_width - compute hex digit width for addresses up to max_addr
 * @max_addr: the maximum address in a section
 *
 * Return: number of hex digits needed (minimum 4)
 */
int hex_addr_width(unsigned long max_addr)
{
	int w = 1;
	unsigned long v = max_addr;

	while (v >= 16)
	{
		v >>= 4;
		w++;
	}
	if (w < 4)
		w = 4;
	return (w);
}

/**
 * print_hex_line - print one hex dump line in objdump -s format
 * @addr: address of first byte in line
 * @buf: buffer of bytes
 * @len: number of bytes (1-16)
 * @bytes_per_line: always 16 for objdump -s
 * @addrw: address field width in hex digits
 */
void print_hex_line(unsigned long addr, unsigned char *buf,
		int len, int bytes_per_line, int addrw)
{
	int i, j;

	printf(" %0*lx", addrw, addr);

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
