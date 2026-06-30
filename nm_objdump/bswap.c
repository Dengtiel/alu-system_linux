#include "hobjdump.h"

/**
 * bswap16 - swap bytes of 16-bit value
 * @v: value to swap
 * Return: swapped value
 */
uint16_t bswap16(uint16_t v)
{
	return (((v & 0x00FF) << 8) | ((v & 0xFF00) >> 8));
}

/**
 * bswap32 - swap bytes of 32-bit value
 * @v: value to swap
 * Return: swapped value
 */
uint32_t bswap32(uint32_t v)
{
	return (((v & 0x000000FF) << 24) |
		((v & 0x0000FF00) << 8)  |
		((v & 0x00FF0000) >> 8)  |
		((v & 0xFF000000) >> 24));
}

/**
 * bswap64 - swap bytes of 64-bit value
 * @v: value to swap
 * Return: swapped value
 */
uint64_t bswap64(uint64_t v)
{
	return (((v & 0x00000000000000FFULL) << 56) |
		((v & 0x000000000000FF00ULL) << 40) |
		((v & 0x0000000000FF0000ULL) << 24) |
		((v & 0x00000000FF000000ULL) << 8)  |
		((v & 0x000000FF00000000ULL) >> 8)  |
		((v & 0x0000FF0000000000ULL) >> 24) |
		((v & 0x00FF000000000000ULL) >> 40) |
		((v & 0xFF00000000000000ULL) >> 56));
}
