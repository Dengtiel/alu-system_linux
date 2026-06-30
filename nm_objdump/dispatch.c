#include "hobjdump.h"

/**
 * objdump_elf32 - process a 32-bit ELF file
 * @fd: file descriptor (at position 0)
 * @ident: ELF identity bytes
 * @filename: file name
 * @prog: program name
 *
 * Return: 0 on success, 1 on error
 */
int objdump_elf32(int fd, unsigned char *ident,
		const char *filename, const char *prog)
{
	Elf32_Ehdr ehdr;
	Elf32_Shdr *shdrs = NULL;
	int sw, shnum;
	size_t shsize;
	ssize_t n;

	sw = (ident[EI_DATA] == ELFDATA2MSB);
	n = read(fd, &ehdr, sizeof(Elf32_Ehdr));
	if (n < (ssize_t)sizeof(Elf32_Ehdr))
	{
		fprintf(stderr, "%s: %s: unexpected end of file\n",
			prog, filename);
		close(fd);
		return (1);
	}
	printf("\n%s:     file format %s\n", filename,
		elf_fmt32(&ehdr, sw));
	print_header32(&ehdr, sw);
	shnum = sw ? (int)bswap16(ehdr.e_shnum) : (int)ehdr.e_shnum;
	shsize = (size_t)shnum * sizeof(Elf32_Shdr);
	shdrs = malloc(shsize);
	if (!shdrs)
	{
		close(fd);
		return (1);
	}
	if (lseek(fd, sw ? bswap32(ehdr.e_shoff) : ehdr.e_shoff,
		SEEK_SET) < 0 ||
		read(fd, shdrs, shsize) < (ssize_t)shsize)
	{
		free(shdrs);
		close(fd);
		return (1);
	}
	print_sections32(fd, &ehdr, shdrs, sw);
	free(shdrs);
	close(fd);
	return (0);
}

/**
 * objdump_elf64 - process a 64-bit ELF file
 * @fd: file descriptor (at position 0)
 * @ident: ELF identity bytes
 * @filename: file name
 * @prog: program name
 *
 * Return: 0 on success, 1 on error
 */
int objdump_elf64(int fd, unsigned char *ident,
		const char *filename, const char *prog)
{
	Elf64_Ehdr ehdr;
	Elf64_Shdr *shdrs = NULL;
	int sw, shnum;
	size_t shsize;
	ssize_t n;

	sw = (ident[EI_DATA] == ELFDATA2MSB);
	n = read(fd, &ehdr, sizeof(Elf64_Ehdr));
	if (n < (ssize_t)sizeof(Elf64_Ehdr))
	{
		fprintf(stderr, "%s: %s: unexpected end of file\n",
			prog, filename);
		close(fd);
		return (1);
	}
	printf("\n%s:     file format %s\n", filename,
		elf_fmt64(&ehdr, sw));
	print_header64(&ehdr, sw);
	shnum = sw ? (int)bswap16(ehdr.e_shnum) : (int)ehdr.e_shnum;
	shsize = (size_t)shnum * sizeof(Elf64_Shdr);
	shdrs = malloc(shsize);
	if (!shdrs)
	{
		close(fd);
		return (1);
	}
	if (lseek(fd, (off_t)(sw ? bswap64(ehdr.e_shoff) : ehdr.e_shoff),
		SEEK_SET) < 0 ||
		read(fd, shdrs, shsize) < (ssize_t)shsize)
	{
		free(shdrs);
		close(fd);
		return (1);
	}
	print_sections64(fd, &ehdr, shdrs, sw);
	free(shdrs);
	close(fd);
	return (0);
}

/**
 * objdump_file - open and dispatch ELF file
 * @filename: path to file
 * @prog: program name for errors
 *
 * Return: 0 on success, 1 on error
 */
int objdump_file(const char *filename, const char *prog)
{
	int fd;
	unsigned char ident[EI_NIDENT];
	ssize_t n;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "%s: %s: No such file or directory\n",
			prog, filename);
		return (1);
	}
	n = read(fd, ident, EI_NIDENT);
	if (n < EI_NIDENT || ident[0] != ELFMAG0 || ident[1] != ELFMAG1
		|| ident[2] != ELFMAG2 || ident[3] != ELFMAG3)
	{
		fprintf(stderr, "%s: %s: file format not recognized\n",
			prog, filename);
		close(fd);
		return (1);
	}
	if (lseek(fd, 0, SEEK_SET) < 0)
	{
		close(fd);
		return (1);
	}
	if (ident[EI_CLASS] == ELFCLASS32)
		return (objdump_elf32(fd, ident, filename, prog));
	if (ident[EI_CLASS] == ELFCLASS64)
		return (objdump_elf64(fd, ident, filename, prog));
	fprintf(stderr, "%s: %s: file format not recognized\n",
		prog, filename);
	close(fd);
	return (1);
}
