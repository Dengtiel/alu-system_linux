#ifndef HOBJDUMP_H
#define HOBJDUMP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <sys/types.h>
#include <sys/stat.h>

uint16_t bswap16(uint16_t v);
uint32_t bswap32(uint32_t v);
uint64_t bswap64(uint64_t v);

int objdump_file(const char *filename, const char *prog);
int objdump_elf32(int fd, unsigned char *ident,
		const char *filename, const char *prog);
int objdump_elf64(int fd, unsigned char *ident,
		const char *filename, const char *prog);

void print_header32(Elf32_Ehdr *e, int sw);
void print_header64(Elf64_Ehdr *e, int sw);

int print_sections32(int fd, Elf32_Ehdr *e,
		Elf32_Shdr *shdrs, int sw);
int print_sections64(int fd, Elf64_Ehdr *e,
		Elf64_Shdr *shdrs, int sw);

const char *elf_arch32(Elf32_Ehdr *e, int sw);
const char *elf_arch64(Elf64_Ehdr *e, int sw);
const char *elf_fmt32(Elf32_Ehdr *e, int sw);
const char *elf_fmt64(Elf64_Ehdr *e, int sw);
void print_flags(uint32_t flags);
void print_hex_line(unsigned long addr, unsigned char *buf,
		int len, int bytes_per_line);

#endif /* HOBJDUMP_H */
