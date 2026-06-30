#include "hobjdump.h"

/**
 * get_shname64 - get section name from string table
 * @fd: file descriptor
 * @shdrs: section headers
 * @shstrndx: string table section index
 * @sh_name: offset into string table (already swapped)
 * @sw: swap flag
 * @buf: output buffer
 * @bufsz: buffer size
 */
static void get_shname64(int fd, Elf64_Shdr *shdrs,
		int shstrndx, uint32_t sh_name, int sw,
		char *buf, size_t bufsz)
{
	uint64_t off;
	ssize_t n;

	off = sw ? bswap64(shdrs[shstrndx].sh_offset)
		: shdrs[shstrndx].sh_offset;
	if (lseek(fd, (off_t)(off + sh_name), SEEK_SET) < 0)
	{
		buf[0] = '\0';
		return;
	}
	n = read(fd, buf, bufsz - 1);
	buf[n > 0 ? n : 0] = '\0';
}

/**
 * dump_section64 - dump one section's hex contents
 * @fd: file descriptor
 * @name: section name
 * @addr: section virtual address
 * @offset: section file offset
 * @size: section size in bytes
 */
static void dump_section64(int fd, const char *name,
		uint64_t addr, uint64_t offset, uint64_t size)
{
	unsigned char buf[16];
	uint64_t pos = 0;
	ssize_t n;
	int chunk, addrw;

	if (size == 0)
		return;
	addrw = hex_addr_width((unsigned long)(addr + size - 1));
	printf("Contents of section %s:\n", name);
	if (lseek(fd, (off_t)offset, SEEK_SET) < 0)
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
		pos += (uint64_t)n;
	}
}

/**
 * should_print64 - decide if a section should be printed
 * @type: section type
 * @flags: section flags
 *
 * Return: 1 if section should be printed, 0 otherwise
 */
static int should_print64(uint32_t type, uint64_t flags)
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
 * print_sections64 - print all sections for 64-bit ELF
 * @fd: file descriptor
 * @e: ELF header
 * @shdrs: section headers array
 * @sw: swap flag
 *
 * Return: 0 on success
 */
int print_sections64(int fd, Elf64_Ehdr *e,
		Elf64_Shdr *shdrs, int sw)
{
	int i, shnum, shstrndx;
	uint32_t type, sh_name;
	uint64_t flags, addr, offset, size;
	char name[256];

	shnum = sw ? (int)bswap16(e->e_shnum) : (int)e->e_shnum;
	shstrndx = sw ? (int)bswap16(e->e_shstrndx)
		: (int)e->e_shstrndx;
	for (i = 0; i < shnum; i++)
	{
		type = sw ? bswap32(shdrs[i].sh_type) : shdrs[i].sh_type;
		flags = sw ? bswap64(shdrs[i].sh_flags)
			: shdrs[i].sh_flags;
		addr = sw ? bswap64(shdrs[i].sh_addr) : shdrs[i].sh_addr;
		offset = sw ? bswap64(shdrs[i].sh_offset)
			: shdrs[i].sh_offset;
		size = sw ? bswap64(shdrs[i].sh_size) : shdrs[i].sh_size;
		sh_name = sw ? bswap32(shdrs[i].sh_name)
			: shdrs[i].sh_name;
		if (!should_print64(type, flags))
			continue;
		get_shname64(fd, shdrs, shstrndx, sh_name, sw,
			name, sizeof(name));
		dump_section64(fd, name, addr, offset, size);
	}
	return (0);
}

/**
 * print_header64 - print file header info for 64-bit ELF
 * @e: ELF header
 * @sw: swap flag
 */
void print_header64(Elf64_Ehdr *e, int sw)
{
	uint16_t type = sw ? bswap16(e->e_type) : e->e_type;
	uint64_t entry = sw ? bswap64(e->e_entry) : e->e_entry;
	uint32_t bfd_flags = 0;

	if (type == ET_EXEC)
		bfd_flags = 0x0112;
	else if (type == ET_DYN)
		bfd_flags = 0x0150;
	else if (type == ET_REL)
		bfd_flags = 0x0011;
	printf("architecture: %s, flags 0x%08x:\n",
		elf_arch64(e, sw), bfd_flags);
	print_flags(bfd_flags);
	printf("start address 0x%016lx\n\n", entry);
}
