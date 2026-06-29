#ifndef HNM_H
#define HNM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef struct sym_entry
{
	char *name;
	unsigned long value;
	char type;
	int has_value;
} sym_entry_t;

int process_elf_file(const char *filename);
int process_elf32(int fd, unsigned char *ident, const char *filename);
int process_elf64(int fd, unsigned char *ident, const char *filename);
int read_elf32_le(int fd, const char *filename);
int read_elf32_be(int fd, const char *filename);
int read_elf64_le(int fd, const char *filename);
int read_elf64_be(int fd, const char *filename);
char get_sym_type32(Elf32_Sym *sym, Elf32_Shdr *shdrs,
		int shnum, int swap);
char get_sym_type64(Elf64_Sym *sym, Elf64_Shdr *shdrs,
		int shnum, int swap);
uint16_t swap16(uint16_t val);
uint32_t swap32(uint32_t val);
uint64_t swap64(uint64_t val);
void print_symbols(sym_entry_t *syms, size_t count, int is64);
void free_symbols(sym_entry_t *syms, size_t count);

#endif /* HNM_H */
