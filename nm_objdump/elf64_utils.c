#include "hnm.h"

/**
 * find_symtab64 - find symbol and string table section indices
 * @shdrs: array of section headers
 * @shnum: number of sections
 * @sym_idx: output symbol table index
 * @str_idx: output string table index
 * @swap: non-zero if byte-swapping needed
 */
void find_symtab64(Elf64_Shdr *shdrs, int shnum,
		int *sym_idx, int *str_idx, int swap)
{
	int i;
	uint32_t type, link;

	*sym_idx = -1;
	*str_idx = -1;
	for (i = 0; i < shnum; i++)
	{
		type = swap ? swap32(shdrs[i].sh_type) : shdrs[i].sh_type;
		if (type == SHT_SYMTAB)
		{
			*sym_idx = i;
			link = swap ? swap32(shdrs[i].sh_link)
				: shdrs[i].sh_link;
			*str_idx = (int)link;
			break;
		}
	}
}

/**
 * load_shdrs64 - read section headers from a 64-bit ELF
 * @fd: file descriptor
 * @ehdr: pointer to ELF header
 * @shnum: number of sections
 * @swap: non-zero if byte-swapping needed
 *
 * Return: allocated Elf64_Shdr array or NULL
 */
Elf64_Shdr *load_shdrs64(int fd, Elf64_Ehdr *ehdr,
		int shnum, int swap)
{
	Elf64_Shdr *shdrs;
	uint64_t shoff;
	ssize_t n;
	size_t shsize;

	shoff = swap ? swap64(ehdr->e_shoff) : ehdr->e_shoff;
	shsize = (size_t)shnum * sizeof(Elf64_Shdr);
	shdrs = malloc(shsize);
	if (!shdrs)
		return (NULL);
	if (lseek(fd, (off_t)shoff, SEEK_SET) < 0)
	{
		free(shdrs);
		return (NULL);
	}
	n = read(fd, shdrs, shsize);
	if (n < 0 || (size_t)n < shsize)
	{
		free(shdrs);
		return (NULL);
	}
	return (shdrs);
}

/**
 * build_entry64 - build one sym_entry_t from a 64-bit symbol
 * @fd: file descriptor
 * @sym: pointer to Elf64_Sym
 * @shdrs: array of section headers
 * @strtab_off: string table file offset
 * @shnum: number of sections
 * @swap: non-zero if byte-swapping needed
 *
 * Return: populated sym_entry_t
 */
sym_entry_t build_entry64(int fd, Elf64_Sym *sym,
		Elf64_Shdr *shdrs, uint64_t strtab_off,
		int shnum, int swap)
{
	sym_entry_t entry;
	uint32_t name_off;
	uint64_t val;
	char namebuf[256];
	ssize_t n;

	name_off = swap ? swap32(sym->st_name) : sym->st_name;
	if (lseek(fd, (off_t)(strtab_off + name_off), SEEK_SET) < 0)
		entry.name = strdup("");
	else
	{
		n = read(fd, namebuf, sizeof(namebuf) - 1);
		namebuf[n > 0 ? n : 0] = '\0';
		entry.name = strdup(namebuf);
	}
	val = swap ? swap64(sym->st_value) : sym->st_value;
	entry.value = (unsigned long)val;
	entry.type = get_sym_type64(sym, shdrs, shnum, swap);
	entry.has_value = (sym->st_shndx != SHN_UNDEF);
	return (entry);
}

/**
 * build_entries64 - build full sym_entry_t array for 64-bit ELF
 * @fd: file descriptor
 * @syms: array of Elf64_Sym
 * @shdrs: array of section headers
 * @symcount: number of symbols
 * @strtab_off: string table file offset
 * @shnum: number of sections
 * @swap: non-zero if byte-swapping needed
 *
 * Return: allocated sym_entry_t array or NULL
 */
sym_entry_t *build_entries64(int fd, Elf64_Sym *syms,
		Elf64_Shdr *shdrs, size_t symcount,
		uint64_t strtab_off, int shnum, int swap)
{
	sym_entry_t *entries;
	size_t i;

	entries = malloc(symcount * sizeof(sym_entry_t));
	if (!entries)
		return (NULL);
	for (i = 0; i < symcount; i++)
		entries[i] = build_entry64(fd, &syms[i], shdrs,
					strtab_off, shnum, swap);
	return (entries);
}
