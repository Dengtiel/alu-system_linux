#include "hnm.h"

/**
 * get_section_type_char32 - get char based on section type (32-bit)
 * @shdr: section header pointer
 * @bind: symbol binding
 *
 * Return: character for symbol type
 */
static char get_section_type_char32(Elf32_Shdr *shdr, unsigned char bind)
{
	Elf32_Word flags = shdr->sh_flags;
	Elf32_Word type = shdr->sh_type;
	char c;

	if (type == SHT_PROGBITS && (flags & SHF_EXECINSTR))
		c = 't';
	else if (type == SHT_PROGBITS && (flags & SHF_WRITE))
		c = 'd';
	else if (type == SHT_PROGBITS)
		c = 'r';
	else if (type == SHT_NOBITS)
		c = 'b';
	else if (type == SHT_INIT_ARRAY || type == SHT_FINI_ARRAY)
		c = 't';
	else if (type == SHT_NOTE)
		c = 'r';
	else
		c = 'd';
	if (bind == STB_GLOBAL)
		c = (char)(c - 32);
	return (c);
}

/**
 * get_sym_type32 - determine nm-style type char for a 32-bit symbol
 * @sym: pointer to Elf32_Sym
 * @shdrs: array of section headers
 * @shnum: number of sections
 * @swap: non-zero if byte-swapping needed
 *
 * Return: character representing symbol type, 0 to skip
 */
char get_sym_type32(Elf32_Sym *sym, Elf32_Shdr *shdrs,
		int shnum, int swap)
{
	unsigned char bind, stype;
	uint16_t shndx;

	bind = ELF32_ST_BIND(sym->st_info);
	stype = ELF32_ST_TYPE(sym->st_info);
	shndx = swap ? swap16(sym->st_shndx) : sym->st_shndx;
	if (stype == STT_FILE || stype == STT_SECTION)
		return (0);
	if (shndx == SHN_UNDEF)
		return (bind == STB_WEAK ?
			(stype == STT_OBJECT ? 'v' : 'w') : 'U');
	if (shndx == SHN_ABS)
		return (bind == STB_LOCAL ? 'a' : 'A');
	if (shndx == SHN_COMMON)
		return ('C');
	if (bind == STB_WEAK)
		return (stype == STT_OBJECT ? 'V' : 'W');
	if (shndx >= (uint16_t)shnum)
		return ('?');
	return (get_section_type_char32(&shdrs[shndx], bind));
}

/**
 * get_section_type_char64 - get char based on section type (64-bit)
 * @shdr: section header pointer
 * @bind: symbol binding
 *
 * Return: character for symbol type
 */
static char get_section_type_char64(Elf64_Shdr *shdr, unsigned char bind)
{
	Elf64_Xword flags = shdr->sh_flags;
	Elf64_Word type = shdr->sh_type;
	char c;

	if (type == SHT_PROGBITS && (flags & SHF_EXECINSTR))
		c = 't';
	else if (type == SHT_PROGBITS && (flags & SHF_WRITE))
		c = 'd';
	else if (type == SHT_PROGBITS)
		c = 'r';
	else if (type == SHT_NOBITS)
		c = 'b';
	else if (type == SHT_INIT_ARRAY || type == SHT_FINI_ARRAY)
		c = 't';
	else if (type == SHT_NOTE)
		c = 'r';
	else
		c = 'd';
	if (bind == STB_GLOBAL)
		c = (char)(c - 32);
	return (c);
}

/**
 * get_sym_type64 - determine nm-style type char for a 64-bit symbol
 * @sym: pointer to Elf64_Sym
 * @shdrs: array of section headers
 * @shnum: number of sections
 * @swap: non-zero if byte-swapping needed
 *
 * Return: character representing symbol type, 0 to skip
 */
char get_sym_type64(Elf64_Sym *sym, Elf64_Shdr *shdrs,
		int shnum, int swap)
{
	unsigned char bind, stype;
	uint16_t shndx;

	bind = ELF64_ST_BIND(sym->st_info);
	stype = ELF64_ST_TYPE(sym->st_info);
	shndx = swap ? swap16(sym->st_shndx) : sym->st_shndx;
	if (stype == STT_FILE || stype == STT_SECTION)
		return (0);
	if (shndx == SHN_UNDEF)
		return (bind == STB_WEAK ?
			(stype == STT_OBJECT ? 'v' : 'w') : 'U');
	if (shndx == SHN_ABS)
		return (bind == STB_LOCAL ? 'a' : 'A');
	if (shndx == SHN_COMMON)
		return ('C');
	if (bind == STB_WEAK)
		return (stype == STT_OBJECT ? 'V' : 'W');
	if (shndx >= (uint16_t)shnum)
		return ('?');
	return (get_section_type_char64(&shdrs[shndx], bind));
}
