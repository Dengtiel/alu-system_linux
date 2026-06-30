#include "hobjdump.h"

/**
 * get_shname32 - get section name from string table
 * @fd: file descriptor
 * @shdrs: section headers
 * @shstrndx: string table section index
 * @sh_name: offset into string table (already swapped)
 * @sw: swap flag
 * @buf: output buffer
 * @bufsz: buffer size
 */
static void get_shname32(int fd, Elf32_Shdr *shdrs,
		int shstrndx, uint32_t sh_name, int sw,
		char *buf, size_t bufsz)
{
	uint32_t off;
	ssize_t n;

	off = sw ? bswap32(shdrs[shstrndx].sh_offset)
		: shdrs[shstrndx].sh_offset;
	if (lseek(fd, off + sh_name, SEEK_SET) < 0)
	{
		buf[0] = '\0';
		return;
	}
	n = read(fd, buf, bufsz - 1);
	buf[n > 0 ? n : 0] = '\0';
}

/**
 * dump_section32 - dump one section's hex contents
 * @fd: file descriptor
 * @name: section name
 * @addr: section virtual address
 * @offset: section file offset
 * @size: section size in bytes
 */
static void dump_section32(int fd, const char *name,
		uint32_t addr, uint32_t offset, uint32_t size)
{
	unsigned char buf[16];
	uint32_t pos = 0;
	ssize_t n;
	int chunk, addrw;

	if (size == 0)
		return;
	addrw = hex_addr_width((unsigned long)(addr + size - 1));
	printf("Contents of section %s:\n", name);
	if (lseek(fd, offset, SEEK_SET) < 0)
		return;
	while (pos < size)
	{
		chunk = (int)(size - pos);
		if (chunk > 16)
			chunk = 16;
		n = read(fd, buf, chunk);
		if (n <= 0)
			break;
		print_hex_line((unsigned long)(addr + pos), buf,
				(int)n, 16, addrw);
		pos += (uint32_t)n;
	}
}

/**
 * should_print32 - decide if a section should be printed
 * @type: section type
 * @flags: section flags
 *
 * Return: 1 if section should be printed, 0 otherwise
 */
static int should_print32(uint32_t type, uint32_t flags)
{
	if (type == SHT_NULL || type == SHT_NOBITS)
		return (0);
	if (flags & SHF_ALLOC)
		return (1);
	if (type == SHT_PROGBITS || type == SHT_NOTE)
		return (1);
	return (0);
}

/**
 * print_sections32 - print all sections for 32-bit ELF
 * @fd: file descriptor
 * @e: ELF header
 * @shdrs: section headers array
 * @sw: swap flag
 *
 * Return: 0 on success
 */
int print_sections32(int fd, Elf32_Ehdr *e,
		Elf32_Shdr *shdrs, int sw)
{
	int i, shnum, shstrndx;
	uint32_t type, flags, addr, offset, size, sh_name;
	char name[256];

	shnum = sw ? (int)bswap16(e->e_shnum) : (int)e->e_shnum;
	shstrndx = sw ? (int)bswap16(e->e_shstrndx)
		: (int)e->e_shstrndx;
	for (i = 0; i < shnum; i++)
	{
		type = sw ? bswap32(shdrs[i].sh_type) : shdrs[i].sh_type;
		flags = sw ? bswap32(shdrs[i].sh_flags)
			: shdrs[i].sh_flags;
		addr = sw ? bswap32(shdrs[i].sh_addr) : shdrs[i].sh_addr;
		offset = sw ? bswap32(shdrs[i].sh_offset)
			: shdrs[i].sh_offset;
		size = sw ? bswap32(shdrs[i].sh_size) : shdrs[i].sh_size;
		sh_name = sw ? bswap32(shdrs[i].sh_name)
			: shdrs[i].sh_name;
		if (!should_print32(type, flags))
			continue;
		get_shname32(fd, shdrs, shstrndx, sh_name, sw,
			name, sizeof(name));
		dump_section32(fd, name, addr, offset, size);
	}
	return (0);
}

/**
 * print_header32 - print file header info for 32-bit ELF
 * @e: ELF header
 * @sw: swap flag
 */
void print_header32(Elf32_Ehdr *e, int sw)
{
	uint16_t type = sw ? bswap16(e->e_type) : e->e_type;
	uint32_t entry = sw ? bswap32(e->e_entry) : e->e_entry;
	uint32_t bfd_flags = 0;

	if (type == ET_EXEC)
		bfd_flags = 0x0112;
	else if (type == ET_DYN)
		bfd_flags = 0x0150;
	else if (type == ET_REL)
		bfd_flags = 0x0011;
	printf("architecture: %s, flags 0x%08x:\n",
		elf_arch32(e, sw), bfd_flags);
	print_flags(bfd_flags);
	printf("start address 0x%08x\n\n", entry);
}
