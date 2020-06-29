#pragma once

#include <platcompiler.h>
#include <stdint.h>

#define ELF_MAGIC_BYTE0							(0x7F)
#define ELF_MAGIC_BYTE1							('E')
#define ELF_MAGIC_BYTE2							('L')
#define ELF_MAGIC_BYTE3							('F')

#define ELFHDR_OSABI_SYSTEMV					(0x00)
#define ELFHDR_OSABI_HPUX						(0x01)
#define ELFHDR_OSABI_NETBSD						(0x02)
#define ELFHDR_OSABI_LINUX						(0x03)
#define ELFHDR_OSABI_GNUHURD					(0x04)
#define ELFHDR_OSABI_SOLARIS					(0x06)
#define ELFHDR_OSABI_AIX						(0x07)
#define ELFHDR_OSABI_IRIX						(0x08)
#define ELFHDR_OSABI_FREEBSD					(0x09)
#define ELFHDR_OSABI_TRU64						(0x0A)
#define ELFHDR_OSABI_NOVEL_MODESTO				(0x0B)
#define ELFHDR_OSABI_OPENBSD					(0x0C)
#define ELFHDR_OSABI_OPENVMS					(0x0D)
#define ELFHDR_OSABI_NONSTOP_KERNEL				(0x0E)
#define ELFHDR_OSABI_AROS						(0x0F)
#define ELFHDR_OSABI_FENIXOS					(0x10)
#define ELFHDR_OSABI_CLOUDABI					(0x11)
#define ELFHDR_OSABI_OPENVOS					(0x12)

#define ELFHDR_ET_NONE							(0x0000)
#define ELFHDR_ET_REL							(0x0001)
#define ELFHDR_ET_EXEC							(0x0002)
#define ELFHDR_ET_DYN							(0x0003)
#define ELFHDR_ET_CORE							(0x0004)
#define ELFHDR_ET_LOOS							(0xFE00)
#define ELFHDR_ET_HIOS							(0xFEFF)
#define ELFHDR_ET_LOPROC						(0xFF00)
#define ELFHDR_ET_HIPROC						(0xFFFF)

#define ELFHDR_MACHINE_NONE						(0x0000)
#define ELFHDR_MACHINE_SPARC					(0x0002)
#define ELFHDR_MACHINE_X86						(0x0003)
#define ELFHDR_MACHINE_MIPS						(0x0008)
#define ELFHDR_MACHINE_POWERPC					(0x0014)
#define ELFHDR_MACHINE_POWERPC64				(0x0015)
#define ELFHDR_MACHINE_S390						(0x0016)
#define ELFHDR_MACHINE_ARM						(0x0028)
#define ELFHDR_MACHINE_SUPERH					(0x002A)
#define ELFHDR_MACHINE_IA64						(0x0032)
#define ELFHDR_MACHINE_AMD64					(0x003E)
#define ELFHDR_MACHINE_TMS320C600				(0x008C)
#define ELFHDR_MACHINE_AARCH64					(0x00B7)
#define ELFHDR_MACHINE_RISCV					(0x00F3)

#define ELFPGM_PT_NULL							(0x0000)
#define ELFPGM_PT_LOAD							(0x00000001)
#define ELFPGM_PT_DYNAMIC						(0x00000002)
#define ELFPGM_PT_INTERP						(0x00000003)
#define ELFPGM_PT_NOTE							(0x00000004)
#define ELFPGM_PT_SHLIB							(0x00000005)
#define ELFPGM_PT_PHDR							(0x00000006)
#define ELFPGM_PT_TLS							(0x00000007)
#define ELFPGM_PT_LOOS							(0x60000000)
#define ELFPGM_PT_HIOS							(0x6FFFFFFF)
#define ELFPGM_PT_LOPROC						(0x70000000)
#define ELFPGM_PT_HIPROC						(0x7FFFFFFF)

#define ELFSECT_SHT_NULL						(0x00000000)
#define ELFSECT_SHT_PROGBITS					(0x00000001)
#define ELFSECT_SHT_SYMTAB						(0x00000002)
#define ELFSECT_SHT_STRTAB						(0x00000003)
#define ELFSECT_SHT_RELA						(0x00000004)
#define ELFSECT_SHT_HASH						(0x00000005)
#define ELFSECT_SHT_DYNAMIC						(0x00000006)
#define ELFSECT_SHT_NOTE						(0x00000007)
#define ELFSECT_SHT_NOBITS						(0x00000008)
#define ELFSECT_SHT_REL							(0x00000009)
#define ELFSECT_SHT_SHLIB						(0x0000000A)
#define ELFSECT_SHT_DYNSYM						(0x0000000B)
#define ELFSECT_SHT_INIT_ARRAY					(0x0000000E)
#define ELFSECT_SHT_FINI_ARRAY					(0x0000000F)
#define ELFSECT_SHT_PREINIT_ARRAY				(0x00000010)
#define ELFSECT_SHT_GROUP						(0x00000011)
#define ELFSECT_SHT_SYMTAB_SHNDX				(0x00000012)
#define ELFSECT_SHT_NUM							(0x00000013)
#define ELFSECT_SHT_LOOS						(0x60000000)
#define ELFSECT_SHT_ARM_EXIDX					(0x70000001)
#define ELFSECT_SHT_ARM_ATTRIBUTES				(0x70000003)

#define ELFSYM_STB_LOCAL						(0x00)
#define ELFSYM_STB_GLOBAL						(0x01)
#define ELFSYM_STB_WEAK							(0x02)
#define ELFSYM_STB_LOOS							(0x0A)
#define ELFSYM_STB_HIOS							(0x0C)
#define ELFSYM_STB_LOPROC						(0x0D)
#define ELFSYM_STB_HIPROC						(0x0F)

#define ELFSYM_STT_NOTYPE						(0x00)
#define ELFSYM_STT_OBJECT						(0x01)
#define ELFSYM_STT_FUNC							(0x02)
#define ELFSYM_STT_SECTION						(0x03)
#define ELFSYM_STT_FILE							(0x04)
#define ELFSYM_STT_COMMON						(0x05)
#define ELFSYM_STT_TLS							(0x06)
#define ELFSYM_STT_LOOS							(0x0A)
#define ELFSYM_STT_HIOS							(0x0C)
#define ELFSYM_STT_LOPROC						(0x0D)
#define ELFSYM_STT_HIPROC						(0x0F)

#define ELFSYM_ST_BIND(i)						((i)>>4)
#define ELFSYM_ST_TYPE(i)						((i)&0xf)
#define ELFSYM_ST_INFO(b,t)						(((b)<<4)+((t)&0xf))

#define ELFSYM_ST_VISIBILITY(o)					((o)&0x3)

#pragma pack(push)
#pragma pack(1)
struct elfheader
{
	uint8_t e_ident_[9];
	uint8_t e_padding1[7];
	uint16_t e_type_;
	uint16_t e_machine_;
	uint32_t e_version_;
	uint32_t e_entry_;
	uint32_t e_phoff_;
	uint32_t e_shoff_;
	uint32_t e_flags_;
	uint16_t e_ehsize_;
	uint16_t e_phentsize_;
	uint16_t e_phnum_;
	uint16_t e_shentsize_;
	uint16_t e_shnum_;
	uint16_t e_shstrndx_;
};

struct elfpgmheader
{
	uint32_t p_type_;
	uint32_t p_offset_;
	uint32_t p_vaddr_;
	uint32_t p_paddr_;
	uint32_t p_filesz_;
	uint32_t p_memsz_;
	uint32_t p_flags_;
	uint32_t p_align_;
};

struct elfsectheader
{
	uint32_t sh_name_;
	uint32_t sh_type_;
	uint32_t sh_flags_;
	uint32_t sh_addr_;
	uint32_t sh_offset_;
	uint32_t sh_size_;
	uint32_t sh_link_;
	uint32_t sh_info_;
	uint32_t sh_addralign_;
	uint32_t sh_entsize_;
};

struct elfheader64
{
	uint8_t ident_[9];
	uint8_t padding1[7];
	uint16_t type_;
	uint16_t machine_;
	uint32_t version_;
	uint64_t entry_;
	uint64_t phoff_;
	uint64_t shoff_;
	uint32_t flags_;
	uint16_t ehsize_;
	uint16_t phentsize_;
	uint16_t phnum_;
	uint16_t shentsize_;
	uint16_t shnum_;
	uint16_t shstrndx_;
};

struct elfpgmheader64
{
	uint32_t type_;
	uint32_t flags_;
	uint64_t offset_;
	uint64_t vaddr_;
	uint64_t paddr_;
	uint64_t filesz_;
	uint64_t memsz_;
	uint64_t align_;
};

struct elfsectheader64
{
	uint32_t sh_name_;
	uint32_t sh_type_;
	uint64_t sh_flags_;
	uint64_t sh_addr_;
	uint64_t sh_offset_;
	uint64_t sh_size_;
	uint32_t sh_link_;
	uint32_t sh_info_;
	uint64_t sh_addralign_;
	uint64_t sh_entsize_;
};

struct elfsymbol
{
	uint32_t st_name_;
	uint32_t st_value_;
	uint32_t st_size_;
	uint8_t st_info_;
	uint8_t st_other_;
	uint16_t st_shndx_;
};

#pragma pack(pop)