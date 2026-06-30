#include "hobjdump.h"

/**
 * elf_fmt32 - get format string for 32-bit ELF
 * @e: ELF header
 * @sw: swap flag
 * Return: format string
 */
const char *elf_fmt32(Elf32_Ehdr *e, int sw)
{
	uint16_t mach = sw ? bswap16(e->e_machine) : e->e_machine;
	uint8_t data = e->e_ident[EI_DATA];

	if (mach == EM_386)
		return ("elf32-i386");
	if (mach == EM_MIPS)
		return (data == ELFDATA2MSB ? "elf32-bigmips"
			: "elf32-littlemips");
	if (mach == EM_PPC)
		return ("elf32-powerpc");
	if (mach == EM_ARM)
		return ("elf32-littlearm");
	if (data == ELFDATA2MSB)
		return ("elf32-big");
	return ("elf32-little");
}

/**
 * elf_fmt64 - get format string for 64-bit ELF
 * @e: ELF header
 * @sw: swap flag
 * Return: format string
 */
const char *elf_fmt64(Elf64_Ehdr *e, int sw)
{
	uint16_t mach = sw ? bswap16(e->e_machine) : e->e_machine;

	if (mach == EM_X86_64)
		return ("elf64-x86-64");
	if (mach == EM_AARCH64)
		return ("elf64-aarch64");
	if (mach == EM_PPC64)
		return ("elf64-powerpc");
	if (mach == EM_SPARCV9)
		return ("elf64-sparc");
	return ("elf64-unknown");
}

/**
 * elf_arch32 - get architecture string for 32-bit ELF
 * @e: ELF header
 * @sw: swap flag
 * Return: architecture string
 */
const char *elf_arch32(Elf32_Ehdr *e, int sw)
{
	uint16_t mach = sw ? bswap16(e->e_machine) : e->e_machine;

	if (mach == EM_386)
		return ("i386");
	if (mach == EM_MIPS)
		return ("mips");
	if (mach == EM_PPC)
		return ("powerpc:common");
	if (mach == EM_ARM)
		return ("arm");
	return ("UNKNOWN!");
}

/**
 * elf_arch64 - get architecture string for 64-bit ELF
 * @e: ELF header
 * @sw: swap flag
 * Return: architecture string
 */
const char *elf_arch64(Elf64_Ehdr *e, int sw)
{
	uint16_t mach = sw ? bswap16(e->e_machine) : e->e_machine;

	if (mach == EM_X86_64)
		return ("i386:x86-64");
	if (mach == EM_AARCH64)
		return ("aarch64");
	if (mach == EM_PPC64)
		return ("powerpc:common64");
	if (mach == EM_SPARCV9)
		return ("sparc:v9");
	return ("UNKNOWN!");
}

/**
 * print_flags - print BFD flag names from flags value
 * @flags: BFD flags value
 */
void print_flags(uint32_t flags)
{
	int first = 1;

	if (flags & 0x0001)
	{
		printf("%sHAS_RELOC", first ? "" : ", ");
		first = 0;
	}
	if (flags & 0x0002)
	{
		printf("%sEXEC_P", first ? "" : ", ");
		first = 0;
	}
	if (flags & 0x0004)
	{
		printf("%sHAS_LINENO", first ? "" : ", ");
		first = 0;
	}
	if (flags & 0x0010)
	{
		printf("%sHAS_SYMS", first ? "" : ", ");
		first = 0;
	}
	if (flags & 0x0020)
	{
		printf("%sHAS_LOCALS", first ? "" : ", ");
		first = 0;
	}
	if (flags & 0x0040)
	{
		printf("%sDYNAMIC", first ? "" : ", ");
		first = 0;
	}
	if (flags & 0x0100)
	{
		printf("%sD_PAGED", first ? "" : ", ");
		first = 0;
	}
	(void)first;
	printf("\n");
}
