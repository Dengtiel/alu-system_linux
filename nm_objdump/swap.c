#include "hnm.h"

/**
 * swap16 - swap bytes of a 16-bit value
 * @val: value to swap
 *
 * Return: byte-swapped value
 */
uint16_t swap16(uint16_t val)
{
	return (((val & 0x00FF) << 8) |
		((val & 0xFF00) >> 8));
}

/**
 * swap32 - swap bytes of a 32-bit value
 * @val: value to swap
 *
 * Return: byte-swapped value
 */
uint32_t swap32(uint32_t val)
{
	return (((val & 0x000000FF) << 24) |
		((val & 0x0000FF00) << 8)  |
		((val & 0x00FF0000) >> 8)  |
		((val & 0xFF000000) >> 24));
}

/**
 * swap64 - swap bytes of a 64-bit value
 * @val: value to swap
 *
 * Return: byte-swapped value
 */
uint64_t swap64(uint64_t val)
{
	return (((val & 0x00000000000000FFULL) << 56) |
		((val & 0x000000000000FF00ULL) << 40) |
		((val & 0x0000000000FF0000ULL) << 24) |
		((val & 0x00000000FF000000ULL) << 8)  |
		((val & 0x000000FF00000000ULL) >> 8)  |
		((val & 0x0000FF0000000000ULL) >> 24) |
		((val & 0x00FF000000000000ULL) >> 40) |
		((val & 0xFF00000000000000ULL) >> 56));
}
