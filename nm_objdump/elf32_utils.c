#include "hnm.h"

/**
 * find_symtab32 - find symbol and string table section indices
 * @shdrs: array of section headers
 * @shnum: number of sections
 * @sym_idx: output symbol table index
 * @str_idx: output string table index
 * @swap: non-zero if byte-swapping needed
 */
void find_symtab32(Elf32_Shdr *shdrs, int shnum,
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
 * load_shdrs32 - read section headers from a 32-bit ELF
 * @fd: file descriptor
 * @ehdr: pointer to ELF header
 * @shnum: number of sections
 * @swap: non-zero if byte-swapping needed
 *
 * Return: allocated Elf32_Shdr array or NULL
 */
Elf32_Shdr *load_shdrs32(int fd, Elf32_Ehdr *ehdr,
		int shnum, int swap)
{
	Elf32_Shdr *shdrs;
	uint32_t shoff;
	ssize_t n;
	size_t shsize;

	shoff = swap ? swap32(ehdr->e_shoff) : ehdr->e_shoff;
	shsize = (size_t)shnum * sizeof(Elf32_Shdr);
	shdrs = malloc(shsize);
	if (!shdrs)
		return (NULL);
	if (lseek(fd, shoff, SEEK_SET) < 0)
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
