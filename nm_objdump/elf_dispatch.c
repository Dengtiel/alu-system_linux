#include "hnm.h"

/**
 * open_and_read_ident - open file and read ELF identity bytes
 * @filename: path to file
 * @prog: program name for error messages
 * @ident: buffer for ELF identity bytes
 *
 * Return: file descriptor or -1 on error
 */
static int open_and_read_ident(const char *filename,
		const char *prog, unsigned char *ident)
{
	int fd;
	ssize_t n;

	fd = open(filename, O_RDONLY);
	if (fd < 0)
	{
		fprintf(stderr, "%s: %s: No such file or directory\n",
			prog, filename);
		return (-1);
	}
	n = read(fd, ident, EI_NIDENT);
	if (n < EI_NIDENT || ident[0] != ELFMAG0 || ident[1] != ELFMAG1
		|| ident[2] != ELFMAG2 || ident[3] != ELFMAG3)
	{
		fprintf(stderr, "%s: %s: file format not recognized\n",
			prog, filename);
		close(fd);
		return (-1);
	}
	if (lseek(fd, 0, SEEK_SET) < 0)
	{
		close(fd);
		return (-1);
	}
	return (fd);
}

/**
 * process_elf_file - open and dispatch ELF file to correct handler
 * @filename: path to ELF file
 * @prog: program name for error messages
 *
 * Return: 0 on success, 1 on error
 */
int process_elf_file(const char *filename, const char *prog)
{
	int fd;
	unsigned char ident[EI_NIDENT];

	fd = open_and_read_ident(filename, prog, ident);
	if (fd < 0)
		return (1);
	if (ident[EI_CLASS] == ELFCLASS32)
		return (process_elf32(fd, ident, filename, prog));
	if (ident[EI_CLASS] == ELFCLASS64)
		return (process_elf64(fd, ident, filename, prog));
	fprintf(stderr, "%s: %s: file format not recognized\n",
		prog, filename);
	close(fd);
	return (1);
}

/**
 * process_elf32 - process a 32-bit ELF file
 * @fd: file descriptor
 * @ident: ELF identity bytes
 * @filename: file name for error messages
 * @prog: program name for error messages
 *
 * Return: 0 on success, 1 on error
 */
int process_elf32(int fd, unsigned char *ident,
		const char *filename, const char *prog)
{
	int ret;

	if (ident[EI_DATA] == ELFDATA2LSB)
		ret = read_elf32_le(fd, filename, prog);
	else if (ident[EI_DATA] == ELFDATA2MSB)
		ret = read_elf32_be(fd, filename, prog);
	else
	{
		fprintf(stderr, "%s: %s: unknown byte order\n",
			prog, filename);
		close(fd);
		return (1);
	}
	close(fd);
	return (ret);
}

/**
 * process_elf64 - process a 64-bit ELF file
 * @fd: file descriptor
 * @ident: ELF identity bytes
 * @filename: file name for error messages
 * @prog: program name for error messages
 *
 * Return: 0 on success, 1 on error
 */
int process_elf64(int fd, unsigned char *ident,
		const char *filename, const char *prog)
{
	int ret;

	if (ident[EI_DATA] == ELFDATA2LSB)
		ret = read_elf64_le(fd, filename, prog);
	else if (ident[EI_DATA] == ELFDATA2MSB)
		ret = read_elf64_be(fd, filename, prog);
	else
	{
		fprintf(stderr, "%s: %s: unknown byte order\n",
			prog, filename);
		close(fd);
		return (1);
	}
	close(fd);
	return (ret);
}
