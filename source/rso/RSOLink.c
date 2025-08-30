#include <revolution/rso.h>

/*******************************************************************************
 * headers
 */

#include <string.h>

#include <__ppc_eabi_linker.h>
#include <elf.h>

#include <macros.h>

#include <revolution/types.h>

#if 0
#include <revolution/os/OSReport.h>
#include <revolution/os/OSCache.h>
#else
#include <context_rvl.h>
#endif

/*******************************************************************************
 * macros
 */

#undef NULL
#define NULL					((void *)(0))

#define JUMP_CODE_INST_COUNT	6
#define JUMP_CODE_SIZE			(sizeof(u32) * JUMP_CODE_INST_COUNT)

// Binding macros

#define BIND_POINTER(target_, base_)				\
	do												\
		*(u32 *)&(target_) += (u32)(base_);			\
	while (false)

#define UNBIND_POINTER(target_, base_)				\
	do												\
		*(u32 *)&(target_) -= (u32)(base_);			\
	while (false)

// Module to section info (TODO: what is this?)

struct RSOSectionInfo;
struct mtsi {char _[12]; struct RSOSectionInfo *at_0x0c;};
#define MODULE_TO_SECTION_INFO(module_)	\
	(((struct mtsi *)(module_))->at_0x0c)

// Linked list macros

#define ADD_LINK_TO_LIST_TAIL(list_, link_)			\
	do												\
	{												\
		RSOObjectInfo *__prev = (list_)->tail;		\
													\
		if (__prev == nullptr)						\
			(list_)->head = (link_);				\
		else										\
			__prev->link.next = (link_);			\
													\
		(link_)->link.prev = __prev;				\
		(link_)->link.next = nullptr;				\
													\
		(list_)->tail = (link_);					\
	} while (false)

#define REMOVE_LINK_FROM_LIST(list_, link_)			\
	do												\
	{												\
		RSOObjectInfo *__next = (link_)->link.next;	\
		RSOObjectInfo *__prev = (link_)->link.prev;	\
													\
		if (__next == nullptr)						\
			(list_)->tail = __prev;					\
		else										\
			__next->link.prev = __prev;				\
													\
		if (__prev == nullptr)						\
			(list_)->head = __next;					\
		else										\
			__prev->link.next = __next;				\
	} while (false)

// Relocation macros

// clang-format off
#define MAKE_RELOC_ADDR24(base_, reloc_)		(((base_) & ~0x03fffffc) | ((reloc_) & 0x03fffffc))
// Note: these ADDR16 macros return uint16_ts
#define MAKE_RELOC_ADDR16(base_, reloc_)		((reloc_) & 0xffff)
#define MAKE_RELOC_ADDR16_LO(base_, reloc_)		((reloc_) & 0xffff)
#define MAKE_RELOC_ADDR16_HI(base_, reloc_)		(((reloc_) >> 16) & 0xffff)
#define MAKE_RELOC_ADDR16_HA(base_, reloc_)		((((reloc_) >> 16) + ((reloc_) & 0x8000 ? 1 : 0)) & 0xffff)
#define MAKE_RELOC_ADDR14(base_, reloc_)		(((base_) & ~0x0000fffc) | ((reloc_) & 0x0000fffc))
#define MAKE_RELOC_REL24(base_, reloc_)			(((base_) & ~0x03fffffc) | ((reloc_) & 0x03fffffc))
#define MAKE_RELOC_REL14(base_, reloc_)			(((base_) & ~0x0000fffc) | ((reloc_) & 0x0000fffc))

// Embedded-specific relocations
// Note: MAKE_RELOC_EMB_SDA21_REG  operates on the second uint8_t  quarter of a uint32_t (0x..xx....)
#define MAKE_RELOC_EMB_SDA21_REG(hi_, reg_)		(((hi_) & ~0x1f) | (reg_))
// Note: MAKE_RELOC_EMB_SDA21_ADDR operates on the second uint16_t half    of a uint32_t (0x....xxxx)
#define MAKE_RELOC_EMB_SDA21_ADDR(lo_, addr_)	((addr_) & 0xffff)
// clang-format on

/*******************************************************************************
 * types
 */

typedef int ConvertStatus;
enum ConvertStatus_et
{
	ConvertFailure	= -1,
	ConvertNone		= 0,
	ConvertSuccess	= 1,
};

// [SC5PGN]/build/libs/Debug/slamWiiD.a:RSOLink.o(1)::.debug_info::0x988 (original object)
typedef struct RSOObjectList
{
	RSOObjectInfo	*head;	// size 0x04, offset 0x00
	RSOObjectInfo	*tail;	// size 0x04, offset 0x04
} RSOObjectList; // size 0x08

// [SC5PGN]/build/libs/Debug/slamWiiD.a:RSOLink.o(1)::.debug_info::0x552 (original object)
typedef struct RSORel
{
	u32	offset;	// size 0x04, offset 0x00
	u32	info;	// size 0x04, offset 0x04
	u32	addend;	// size 0x04, offset 0x08
} RSORel; // size 0x0c

// [SC5PGN]/build/libs/Debug/slamWiiD.a:RSOLink.o(1)::.debug_info::0x270 (original object)
typedef struct RSOSectionInfo
{
	u32 offset;	// size 0x04, offset 0x00
	u32 size;	// size 0x04, offset 0x04
} RSOSectionInfo; // size 0x08

/*******************************************************************************
 * local function declarations
 */

static BOOL LocateObject(void *newModule, void *bss,
                         RSOFixedLevel i_fixed_level);

static void RSOResolveImportSymbol(RSOObjectHeader *rsoImp, int index,
                                   void *addr);
static void RSOUnresolveImportSymbol(RSOObjectHeader *rsoImp, int index);
static void RSOUnresolveImportSymbolAll(RSOObjectHeader *rso);

static int FindExportIndex(RSOObjectHeader *rso, char *name);

static void RSORelocate(RSORel *rel, int index, u32 offset);
static void RSORelocateImportSymbol(RSOObjectHeader *rso,
                                    RSOImportTable *impTab, int impIndex);
static void RSORelocateSmallDataSection(RSOObjectHeader *rsoImp, int impIndex,
                                        RSOObjectHeader *rsoExp);

static BOOL LinkList(void *i_newRso, void *i_bss, RSOFixedLevel i_fixed_level);

static BOOL checkNeedFarCode(RSOObjectHeader *rso, RSOImportTable *impTab,
                             int impIndex, u32 addr);
static void makeCode(u32 addr, u32 *i_buff);
static ConvertStatus cnvFarCode(RSOObjectHeader *rso, RSOImportTable *impTab,
                                int impIndex, u32 addr, u32 *i_buff);
static ConvertStatus cnvJumpCode(RSOObjectHeader *rso, RSOImportTable *impTab,
                                 int impIndex, u32 addr, u32 *i_buff);

/*******************************************************************************
 * variables
 */

// .sbss
static RSOObjectList __RSOObjectInfoList;

/*******************************************************************************
 * functions
 */

ATTR_WEAK ATTR_NOINLINE void RSONotifyModuleLoaded(
	RSOObjectHeader *moduleHeader ATTR_UNUSED)
{
	/* ... */
}

ATTR_WEAK ATTR_NOINLINE void RSONotifyModuleUnloaded(
	RSOObjectHeader *moduleHeader ATTR_UNUSED)
{
	/* ... */
}

ATTR_WEAK ATTR_NOINLINE void RSONotifyPreRSOLink(
	RSOObjectHeader *rsoImp ATTR_UNUSED, RSOObjectHeader *rsoExp ATTR_UNUSED)
{
	/* ... */
}

ATTR_WEAK ATTR_NOINLINE void RSONotifyPostRSOLink(
	RSOObjectHeader *rsoImp ATTR_UNUSED, RSOObjectHeader *rsoExp ATTR_UNUSED)
{
	/* ... */
}

ATTR_WEAK ATTR_NOINLINE void RSONotifyPreRSOLinkFar(
	RSOObjectHeader *i_rsoImp ATTR_UNUSED,
	RSOObjectHeader *i_rsoExp ATTR_UNUSED, void *i_buff ATTR_UNUSED)
{
	/* ... */
}

ATTR_WEAK ATTR_NOINLINE void RSONotifyPostRSOLinkFar(
	RSOObjectHeader *i_rsoImp ATTR_UNUSED,
	RSOObjectHeader *i_rsoExp ATTR_UNUSED, void *i_buff ATTR_UNUSED)
{
	/* ... */
}

static BOOL LocateObject(void *newModule, void *bss,
                         RSOFixedLevel i_fixed_level)
{
	u32 i;
	RSOSectionInfo *si;
	RSOObjectHeader *moduleHeader = newModule;
	RSORel *intRel;
	RSORel *extRel;
	RSOSectionInfo *sectionInfo;
	u32 a_max;

	moduleHeader->bssSection = 0;

	OSAssert_Line(201, moduleHeader->info.version <= RSO_VERSION);

	BIND_POINTER(moduleHeader->info.sectionInfoOffset, moduleHeader);
	BIND_POINTER(moduleHeader->info.nameOffset, moduleHeader);
	BIND_POINTER(moduleHeader->internalRelOffset, moduleHeader);
	BIND_POINTER(moduleHeader->externalRelOffset, moduleHeader);
	BIND_POINTER(moduleHeader->expHeader.tableOffset, moduleHeader);
	BIND_POINTER(moduleHeader->expHeader.stringOffset, moduleHeader);
	BIND_POINTER(moduleHeader->impHeader.tableOffset, moduleHeader);
	BIND_POINTER(moduleHeader->impHeader.stringOffset, moduleHeader);

	for (i = RSO_SECTION_INIT; i < moduleHeader->info.numSections; ++i)
	{
		si = MODULE_TO_SECTION_INFO(newModule) + i;

		if (si->offset)
		{
			BIND_POINTER(si->offset, moduleHeader);
		}
		else if (si->size)
		{
			OSAssert_Line(228, moduleHeader->bssSection == 0);

			moduleHeader->bssSection = i;
			si->offset = (u32)bss;
		}
	}

	if (moduleHeader->prologSection)
	{
		BIND_POINTER(
			moduleHeader->prolog,
			MODULE_TO_SECTION_INFO(newModule)[moduleHeader->prologSection]
				.offset);
	}

	if (moduleHeader->epilogSection)
	{
		BIND_POINTER(
			moduleHeader->epilog,
			MODULE_TO_SECTION_INFO(newModule)[moduleHeader->epilogSection]
				.offset);
	}

	if (moduleHeader->unresolvedSection)
	{
		BIND_POINTER(
			moduleHeader->unresolved,
			MODULE_TO_SECTION_INFO(newModule)[moduleHeader->unresolvedSection]
				.offset);
	}

	a_max = moduleHeader->internalRelSize / sizeof *intRel;

	for (i = 0; i < a_max; ++i)
	{
		intRel = (RSORel *)moduleHeader->internalRelOffset + i;

		BIND_POINTER(intRel->offset, moduleHeader);

		sectionInfo = (RSOSectionInfo *)moduleHeader->info.sectionInfoOffset
		            + ELF32_R_SYM(intRel->info);

		RSORelocate(intRel, 0, sectionInfo->offset);
	}

	if (i_fixed_level >= RSO_FL_INTERNAL)
		moduleHeader->internalRelSize = 0;

	a_max = moduleHeader->externalRelSize / sizeof *extRel;

	for (i = 0; i < a_max; ++i)
	{
		extRel = (RSORel *)moduleHeader->externalRelOffset + i;

		BIND_POINTER(extRel->offset, moduleHeader);
	}

	RSOUnresolveImportSymbolAll(moduleHeader);

	if (i_fixed_level <= RSO_FL_INTERNAL)
		memset(bss, 0, moduleHeader->bssSize);

	RSONotifyModuleLoaded(moduleHeader);

	return true;
}

BOOL RSOLocateObject(void *newModule, void *bss)
{
	return LocateObject(newModule, bss, RSO_FL_NON);
}

BOOL RSOLocateObjectFixed(void *newModule, void *bss)
{
	return LocateObject(newModule, bss, RSO_FL_INTERNAL);
}

BOOL RSOStaticLocateObject(void *newModule)
{
	RSOObjectHeader *moduleHeader = newModule;
	u32 i;
	u32 a_max;
	RSOSectionInfo *si;
	RSORel *extRel;

	moduleHeader->bssSection = 0;

	BIND_POINTER(moduleHeader->info.sectionInfoOffset, moduleHeader);
	BIND_POINTER(moduleHeader->info.nameOffset, moduleHeader);
	BIND_POINTER(moduleHeader->internalRelOffset, moduleHeader);
	BIND_POINTER(moduleHeader->externalRelOffset, moduleHeader);
	BIND_POINTER(moduleHeader->expHeader.tableOffset, moduleHeader);
	BIND_POINTER(moduleHeader->expHeader.stringOffset, moduleHeader);
	BIND_POINTER(moduleHeader->impHeader.tableOffset, moduleHeader);
	BIND_POINTER(moduleHeader->impHeader.stringOffset, moduleHeader);

	for (i = RSO_SECTION_INIT; i < moduleHeader->info.numSections; ++i)
	{
		si = MODULE_TO_SECTION_INFO(newModule) + i;

		switch (i)
		{
		case RSO_SECTION_INIT:
			si->offset = (u32)_f_init;
			break;

		case RSO_SECTION_TEXT:
			si->offset = (u32)_f_text;
			break;

		case RSO_SECTION_CTORS:
			if (si->size)
			{
				OSReport("Warrning! .ctors section[%d]! size=%x\n", i,
				         si->size);
			}

			si->offset = (u32)nullptr;
			break;

		case RSO_SECTION_DTORS:
			if (si->size)
			{
				OSReport("Warrning! .dtors section[%d]! size=%x\n", i,
				         si->size);
			}

			si->offset = (u32)nullptr;
			break;

		case RSO_SECTION_RODATA:
			si->offset = (u32)_f_rodata;
			break;

		case RSO_SECTION_DATA:
			si->offset = (u32)_f_data;
			break;

		case RSO_SECTION_BSS:
			si->offset = (u32)_f_bss;
			break;

		case RSO_SECTION_SDATA:
			si->offset = (u32)_f_sdata;
			break;

		case RSO_SECTION_SBSS:
			si->offset = (u32)_f_sbss;
			break;

		case RSO_SECTION_SDATA2:
			si->offset = (u32)_f_sdata2;
			break;

		case RSO_SECTION_SBSS2:
			si->offset = (u32)_f_sbss2;
			break;

		case RSO_SECTION_EMB_SDATA0:
			si->offset = (u32)0;
			break;

		case RSO_SECTION_EMB_SBSS0:
			si->offset = (u32)0;
			break;

		default:
			if (si->size)
			{
				OSReport("Warrning! unknown section[%d]! size=%x\n", i,
				         si->size);
			}

			si->offset = (u32)nullptr;
			break;
		}
	}

	a_max = moduleHeader->externalRelSize / sizeof *extRel;

	for (i = 0; i < a_max; ++i)
	{
		extRel = (RSORel *)moduleHeader->externalRelOffset + i;

		BIND_POINTER(extRel->offset, moduleHeader);
	}

	return true;
}

BOOL RSOUnLocateObject(void *oldModule)
{
	RSOObjectHeader *moduleHeader = oldModule;
	int i;
	int a_max;
	RSORel *extRel;
	RSORel *intRel;
	RSOSectionInfo *si;

	RSONotifyModuleUnloaded(moduleHeader);

	a_max = moduleHeader->externalRelSize / sizeof *extRel;

	for (i = 0; i < a_max; ++i)
	{
		extRel = (RSORel *)moduleHeader->externalRelOffset + i;

		UNBIND_POINTER(extRel->offset, moduleHeader);
	}

	a_max = moduleHeader->internalRelSize / sizeof *intRel;

	for (i = 0; i < a_max; ++i)
	{
		intRel = (RSORel *)moduleHeader->internalRelOffset + i;

		UNBIND_POINTER(intRel->offset, moduleHeader);
	}

	if (moduleHeader->prologSection)
	{
		UNBIND_POINTER(
			moduleHeader->prolog,
			MODULE_TO_SECTION_INFO(oldModule)[moduleHeader->prologSection]
				.offset);
	}

	if (moduleHeader->epilogSection)
	{
		UNBIND_POINTER(
			moduleHeader->epilog,
			MODULE_TO_SECTION_INFO(oldModule)[moduleHeader->epilogSection]
				.offset);
	}

	if (moduleHeader->unresolvedSection)
	{
		UNBIND_POINTER(
			moduleHeader->unresolved,
			MODULE_TO_SECTION_INFO(oldModule)[moduleHeader->unresolvedSection]
				.offset);
	}

	for (i = RSO_SECTION_INIT; i < moduleHeader->info.numSections; ++i)
	{
		si = MODULE_TO_SECTION_INFO(oldModule) + i;

		if (i == moduleHeader->bssSection)
		{
			OSAssert_Line(489, si->size != 0);

			moduleHeader->bssSection = 0;
			si->offset = 0;
		}
		else if (si->offset)
		{
			UNBIND_POINTER(si->offset, moduleHeader);
		}
	}

	UNBIND_POINTER(moduleHeader->info.sectionInfoOffset, moduleHeader);
	UNBIND_POINTER(moduleHeader->info.nameOffset, moduleHeader);
	UNBIND_POINTER(moduleHeader->internalRelOffset, moduleHeader);
	UNBIND_POINTER(moduleHeader->externalRelOffset, moduleHeader);
	UNBIND_POINTER(moduleHeader->expHeader.tableOffset, moduleHeader);
	UNBIND_POINTER(moduleHeader->expHeader.stringOffset, moduleHeader);
	UNBIND_POINTER(moduleHeader->impHeader.tableOffset, moduleHeader);
	UNBIND_POINTER(moduleHeader->impHeader.stringOffset, moduleHeader);

	return true;
}

RSOImportTable *RSOGetImport(RSOSymbolHeader *imp)
{
	return (RSOImportTable *)imp->tableOffset;
}

RSOExportTable *RSOGetExport(RSOSymbolHeader *exp)
{
	return (RSOExportTable *)exp->tableOffset;
}

int RSOLink(RSOObjectHeader *rsoImp, RSOObjectHeader *rsoExp)
{
	RSOSymbolHeader *imp = &rsoImp->impHeader;
	int i;
	int count = 0;
	char *impName;
	u32 *addr;
	int s_max = RSOGetNumImportSymbols(imp);
	RSOImportTable *impTab = (RSOImportTable *)imp->tableOffset;

	RSONotifyPreRSOLink(rsoImp, rsoExp);

	/* explicitly post-increment */
	for (i = 0; i < s_max; i++, impTab++)
	{
		impName = (char *)(impTab->strOffset + imp->stringOffset);
		if (impName == nullptr)
			continue;

		RSORelocateSmallDataSection(rsoImp, i, rsoExp);

		addr = RSOFindExportSymbolAddr(rsoExp, impName);
		if (!addr)
			continue;

		RSOResolveImportSymbol(rsoImp, i, addr);

		++count;
	}

	RSONotifyPostRSOLink(rsoImp, rsoExp);

	return count;
}

void RSOUnLink(RSOObjectHeader *rsoImp, RSOObjectHeader *rsoExp)
{
	RSOSymbolHeader *imp = &rsoImp->impHeader;
	int i;
	char *impName;
	u32 *addr;
	int s_max = RSOGetNumImportSymbols((RSOSymbolHeader *)imp);

	RSONotifyPreRSOLink(rsoImp, rsoExp);

	for (i = 0; i < s_max; ++i)
	{
		impName = RSOGetImportSymbolName((RSOSymbolHeader *)imp, (u32)i);
		if (impName == nullptr)
			continue;

		addr = RSOFindExportSymbolAddr(rsoExp, impName);
		if (!addr)
			continue;

		RSOUnresolveImportSymbol(rsoImp, i);
	}

	RSONotifyPostRSOLink(rsoImp, rsoExp);
}

RSOHash RSOGetHash(char *symbolname)
{
	RSOHash h = 0x00000000;
	RSOHash g;

	while (*symbolname)
	{
		h = (h << 4) + *symbolname++;

		g = h & 0xf0000000;

		if (g)
			h ^= g >> 24;

		h &= ~g;
	}

	return h;
}

int RSOGetNumImportSymbols(RSOSymbolHeader *imp)
{
	return imp->tableSize / sizeof(RSOImportTable);
}

int RSOGetNumImportSymbolsUnresolved(RSOObjectHeader *rso)
{
	int count = 0;
	int i;
	int s_max = RSOGetNumImportSymbols(&rso->impHeader);

	for (i = 0; i < s_max; ++i)
	{
		if (!RSOIsImportSymbolResolved(rso, i))
			++count;
	}

	return count;
}

char *RSOGetImportSymbolName(RSOSymbolHeader *imp, int index)
{
	RSOImportTable *impTab = (RSOImportTable *)imp->tableOffset + index;

	return (char *)(impTab->strOffset + imp->stringOffset);
}

BOOL RSOIsImportSymbolResolved(RSOObjectHeader *rso, int index)
{
	RSOImportTable *impTab = RSOGetImport(&rso->impHeader) + index;

	if (impTab->value == rso->unresolved)
		return false;

	return true;
}

BOOL RSOIsImportSymbolResolvedAll(RSOObjectHeader *rso)
{
	int i;
	int s_max = RSOGetNumImportSymbols(&rso->impHeader);

	for (i = 0; i < s_max; ++i)
	{
		if (!RSOIsImportSymbolResolved(rso, i))
			return false;
	}

	return true;
}

static void RSOResolveImportSymbol(RSOObjectHeader *rsoImp, int index,
                                   void *addr)
{
	RSOImportTable *impTab = RSOGetImport(&rsoImp->impHeader) + index;

	impTab->value = (u32)addr;

	RSORelocateImportSymbol(rsoImp, impTab, index);
}

static void RSOUnresolveImportSymbol(RSOObjectHeader *rsoImp, int index)
{
	RSOImportTable *impTab = RSOGetImport(&rsoImp->impHeader) + index;

	impTab->value = rsoImp->unresolved;

	RSORelocateImportSymbol(rsoImp, impTab, index);
}

static void RSOUnresolveImportSymbolAll(RSOObjectHeader *rso)
{
	int i;
	int s_max = RSOGetNumImportSymbols(&rso->impHeader);

	for (i = 0; i < s_max; ++i)
		RSOUnresolveImportSymbol(rso, i);
}

int RSOGetNumExportSymbols(RSOSymbolHeader *exp)
{
	return exp->tableSize / sizeof(RSOExportTable);
}

char *RSOGetExportSymbolName(RSOSymbolHeader *exp, int index)
{
	RSOExportTable *expTab = (RSOExportTable *)exp->tableOffset + index;

	return (char *)(expTab->strOffset + exp->stringOffset);
}

void *RSOGetExportSymbolAddr(RSOObjectHeader *rso, int index)
{
	RSOExportTable *expTab =
		(RSOExportTable *)rso->expHeader.tableOffset + index;
	RSOSectionInfo *expSec =
		(RSOSectionInfo *)rso->info.sectionInfoOffset + expTab->section;

	return (void *)(expSec->offset + expTab->value);
}

static int FindExportIndex(RSOObjectHeader *rso, char *name)
{
	RSOHash a_hash = RSOGetHash(name);
	int i;
	char *expName;
	int s_max = RSOGetNumExportSymbols(&rso->expHeader);
	RSOExportTable *expTab = (RSOExportTable *)rso->expHeader.tableOffset;

	// a
	RSOExportTable *a_expTab;
	int a_top = 0;
	int a_last = s_max - 1;
	int a_idx = -1;

	if (s_max <= 0)
		return -1;

	while (a_idx == -1)
	{
		i = (a_top + a_last) >> 1;
		a_expTab = expTab + i;

		if (a_hash > a_expTab->hash)
		{
			if (a_top == i)
				a_idx = a_last;
			else
				a_top = i;
		}
		else if (a_hash < a_expTab->hash)
		{
			if (a_top == i)
				a_idx = a_top;
			else
				a_last = i;
		}
		else
		{
			a_idx = i;
		}
	}

	a_expTab = expTab + a_idx;

	if (a_hash != a_expTab->hash)
		return -1;

	expName = RSOGetExportSymbolName(&rso->expHeader, a_idx);

	if (strcmp(name, expName) == 0)
		return a_idx;

	for (i = a_idx + 1; i <= a_last; i++)
	{
		a_expTab = expTab + i;

		if (a_hash == a_expTab->hash)
		{
			expName = RSOGetExportSymbolName((RSOSymbolHeader *)&rso->expHeader,
			                                 (u32)i);

			if (strcmp(name, expName) == 0)
				return i;
		}
		else
		{
			i = a_last + 1;
		}
	}

	for (i = a_idx - 1; i >= a_top; --i)
	{
		a_expTab = expTab + i;

		if (a_hash == a_expTab->hash)
		{
			expName = RSOGetExportSymbolName((RSOSymbolHeader *)&rso->expHeader,
			                                 (u32)i);

			if (strcmp(name, expName) == 0)
				return i;
		}
		else
		{
			return -1;
		}
	}

	return -1;
}

void *RSOFindExportSymbolAddr(RSOObjectHeader *rso, char *name)
{
	int a_idx = FindExportIndex(rso, name);

	if (a_idx == -1)
		return nullptr;

	return RSOGetExportSymbolAddr(rso, a_idx);
}

RSOExportTable *RSOFindExportSymbol(RSOObjectHeader *rso, char *name)
{
	int a_idx = FindExportIndex(rso, name);

	if (a_idx == -1)
		return nullptr;

	return (RSOExportTable *)rso->expHeader.tableOffset + a_idx;
}

static void RSORelocate(RSORel *rel, int index, u32 offset)
{
	u32 *p;
	u32 x;
	u32 y;
	RSORel *targetRel = rel + index;

	p = (u32 *)targetRel->offset;

	switch (ELF32_R_TYPE(rel->info))
	{
	case R_PPC_NONE:
		break;

	case R_PPC_ADDR32:
		x = offset + rel->addend;
		*p = x;
		break;

	case R_PPC_ADDR24:
		x = offset + rel->addend;
		*p = MAKE_RELOC_ADDR24(*p, x);
		break;

	case R_PPC_ADDR16:
		x = offset + rel->addend;
		*(u16 *)p = MAKE_RELOC_ADDR16(*p, x);
		break;

	case R_PPC_ADDR16_LO:
		x = offset + rel->addend;
		*(u16 *)p = MAKE_RELOC_ADDR16_LO(*p, x);
		break;

	case R_PPC_ADDR16_HI:
		x = offset + rel->addend;
		*(u16 *)p = MAKE_RELOC_ADDR16_HI(*p, x);
		break;

	case R_PPC_ADDR16_HA:
		x = offset + rel->addend;
		*(u16 *)p = MAKE_RELOC_ADDR16_HA(*p, x);
		break;

	case R_PPC_ADDR14:
	case R_PPC_ADDR14_BRTAKEN:
	case R_PPC_ADDR14_BRNTAKEN:
		x = offset + rel->addend;
		*p = MAKE_RELOC_ADDR14(*p, x);
		break;

	case R_PPC_REL24:
		x = offset + rel->addend - (u32)p;
		*p = MAKE_RELOC_REL24(*p, x);

		x = offset + rel->addend;
		y = MAKE_RELOC_REL24((u32)p, ((u32)p & 0x03fffffc) + (*p & 0x03fffffc));

		if (x != 0 && x != y)
			*p = MAKE_RELOC_REL24(*p, 0x03fffffc);

		break;

	case R_PPC_REL14:
	case R_PPC_REL14_BRTAKEN:
	case R_PPC_REL14_BRNTAKEN:
		x = offset + rel->addend - (u32)p;
		*p = MAKE_RELOC_REL14(*p, x);
		break;

	case R_PPC_EMB_SDA21:
		/* handled in RSORelocateSmallDataSection */
		break;

	default:
		// NOTE: Not ELF32_R_TYPE
		OSReport("OSLink: unknown relocation type %3d\n", (u8)rel->info);
		break;
	}

	DCFlushRange(p, ROUND_UP(sizeof *p, 32));
	ICInvalidateRange(p, ROUND_UP(sizeof *p, 32));
}

static void RSORelocateImportSymbol(RSOObjectHeader *rso,
                                    RSOImportTable *impTab, int impIndex)
{
	RSORel *rel;

	for (rel = (RSORel *)(rso->externalRelOffset + impTab->relOffset);
	     ELF32_R_SYM(rel->info) == impIndex; ++rel)
	{
		RSORelocate(rel, 0, impTab->value);
	}
}

static void RSORelocateSmallDataSection(RSOObjectHeader *rsoImp, int impIndex,
                                        RSOObjectHeader *rsoExp)
{
	u8 *p;
	u32 x;
	u32 base;
	char *impName;
	RSOImportTable *impTab;
	RSOExportTable *expTab;
	RSOExportTable *baseTab;
	RSORel *rel;

	RSOSectionInfo *si;

	impTab = RSOGetImport((RSOSymbolHeader *)&rsoImp->impHeader) + impIndex;
	impName = RSOGetImportSymbolName(&rsoImp->impHeader, impIndex);
	rel = (RSORel *)(rsoImp->externalRelOffset + impTab->relOffset);

	while (ELF32_R_SYM(rel->info) == impIndex)
	{
		if (ELF32_R_TYPE(rel->info) == R_PPC_EMB_SDA21)
		{
			// why not just (u8 *)(((rel->offset) & ~0x03) | 0x01)?
			p = (u8 *)rel->offset;

			switch (rel->offset & 0x03)
			{
			case 0x00:
				p += 1;
				break;

			case 0x01:
				/* want the address of the second quarter of the uint32_t */
				break;

			case 0x02:
				p -= 1;
				break;

			case 0x03:
				p -= 2;
				break;
			}

			expTab = RSOFindExportSymbol(rsoExp, impName);
			if (expTab)
			{
				switch (expTab->section)
				{
				case RSO_SECTION_SDATA:
				case RSO_SECTION_SBSS:
					*p = MAKE_RELOC_EMB_SDA21_REG(*p, 13); // r13 (SysV PPC ABI)

					baseTab = RSOFindExportSymbol(rsoExp, "_SDA_BASE_");
					if (baseTab == nullptr)
					{
						++rel;
						continue;
					}

					base = baseTab->value;
					break;

				case RSO_SECTION_SDATA2:
				case RSO_SECTION_SBSS2:
					*p = MAKE_RELOC_EMB_SDA21_REG(*p, 2); // r2 (PPC EABI)

					baseTab = RSOFindExportSymbol(rsoExp, "_SDA2_BASE_");
					if (baseTab == nullptr)
					{
						++rel;
						continue;
					}

					base = baseTab->value;
					break;

				case RSO_SECTION_EMB_SDATA0:
				case RSO_SECTION_EMB_SBSS0:
					*p = MAKE_RELOC_EMB_SDA21_REG(*p, 0); // (rA|0) (PPC EABI)
					base = 0;
					break;

				case RSO_SECTION_EXTABINDEX:
					OSReport("ERROR: incorrect R_PPC_EMB_SDA21 data.\n");
					break;

				default:
					OSReport("ERROR: incorrect R_PPC_EMB_SDA21 data.\n");
					break;
				}

				++p;

				si = (RSOSectionInfo *)rsoExp->info.sectionInfoOffset
				   + expTab->section;

				x = expTab->value + si->offset + rel->addend - base;
				*(u16 *)p = MAKE_RELOC_EMB_SDA21_ADDR(*p, x);
				--p;

				DCFlushRange(p, ROUND_UP(sizeof *p, 32));
				ICInvalidateRange(p, ROUND_UP(sizeof *p, 32));
			}
		}

		++rel;
	}
}

BOOL RSOListInit(void *i_staticRso)
{
	BOOL r;
	RSOObjectHeader *a_rsoInfo = i_staticRso;

	__RSOObjectInfoList.head = __RSOObjectInfoList.tail = nullptr;

	r = RSOStaticLocateObject(i_staticRso);

	ADD_LINK_TO_LIST_TAIL(&__RSOObjectInfoList, &a_rsoInfo->info);

	return r;
}

static BOOL LinkList(void *i_newRso, void *i_bss, RSOFixedLevel i_fixed_level)
{
	BOOL r;
	RSOObjectHeader *a_rsoInfo = i_newRso;
	RSOObjectInfo *a_target;

	r = LocateObject(i_newRso, i_bss, i_fixed_level);
	if (!r)
		return false;

	for (a_target = __RSOObjectInfoList.head; a_target;
	     a_target = a_target->link.next)
	{
		RSOLink(a_rsoInfo, (RSOObjectHeader *)a_target);
		RSOLink((RSOObjectHeader *)a_target, a_rsoInfo);
	}

	ADD_LINK_TO_LIST_TAIL(&__RSOObjectInfoList, (RSOObjectInfo *)i_newRso);

	if (i_fixed_level >= RSO_FL_EXTERNAL)
	{
		memset(i_bss, 0, a_rsoInfo->bssSize);

		a_rsoInfo->externalRelSize = 0;
		a_rsoInfo->impHeader.tableSize = 0;
	}

	return true;
}

BOOL RSOLinkList(void *i_newRso, void *i_bss)
{
	return LinkList(i_newRso, i_bss, RSO_FL_NON);
}

BOOL RSOLinkListFixed(void *i_newRso, void *i_bss, RSOFixedLevel i_fixed_level)
{
	if (i_fixed_level > RSO_FL_EXTERNAL)
	{
		OSAssert_Line(1430, 0); // not FALSE

		return false;
	}

	return LinkList(i_newRso, i_bss, i_fixed_level);
}

BOOL RSOUnLinkList(void *i_oldRso)
{
	RSOObjectInfo *objectInfo = i_oldRso;
	RSOObjectInfo *a_target;

	if (objectInfo->link.next == nullptr && objectInfo->link.prev == nullptr)
		return false;

	REMOVE_LINK_FROM_LIST(&__RSOObjectInfoList, objectInfo);

	for (a_target = __RSOObjectInfoList.head; a_target;
	     a_target = a_target->link.next)
	{
		RSOUnLink((RSOObjectHeader *)a_target, (RSOObjectHeader *)objectInfo);
		RSOUnLink((RSOObjectHeader *)objectInfo, (RSOObjectHeader *)a_target);
	}

	RSOUnLocateObject(i_oldRso);

	return true;
}

u32 RSOGetFixedSize(void *i_rso, RSOFixedLevel i_fixed_level)
{
	RSOObjectHeader *a_rsoInfo = i_rso;

	switch (i_fixed_level)
	{
	case RSO_FL_NON:
		return 0;

	case RSO_FL_INTERNAL:
		return a_rsoInfo->internalRelOffset; // not internalRelSize?

	case RSO_FL_EXTERNAL:
		return a_rsoInfo->externalRelOffset; // not externalRelSize?

	default:
		OSAssert_Line(1495, 0); // also not FALSE

		return 0;
	}
}

static BOOL checkNeedFarCode(RSOObjectHeader *rso, RSOImportTable *impTab,
                             int impIndex, u32 addr)
{
	RSORel *rel;
	u32 offset;
	u32 *p;
	u32 x;
	u32 y;

	for (rel = (RSORel *)(rso->externalRelOffset + impTab->relOffset);
	     ELF32_R_SYM(rel->info) == impIndex; ++rel)
	{
		if (ELF32_R_TYPE(rel->info) != R_PPC_REL24)
			continue;

		offset = addr;
		p = (u32 *)rel->offset;
		x = offset + rel->addend - (u32)p;

		*p = MAKE_RELOC_REL24(*p, x);

		x = offset + rel->addend;
		y = MAKE_RELOC_REL24((u32)p, ((u32)p & 0x03fffffc) + (*p & 0x03fffffc));

		if (x != 0 && x != y)
			return true;
	}

	return false;
}

static void makeCode(u32 addr, u32 *i_buff)
{
	/*
		// This code injects:

		stw		r12, 4(r1)
		lis		r12, addr@ha
		addi	r12, r12, addr@l
		mtctr	r12
		lwz		r12, 4(r1)
		bctr
	*/

	i_buff[0] = 0x91810004;
	i_buff[1] = 0x3d800000 | MAKE_RELOC_ADDR16_HA(0x3d800000, addr);
	i_buff[2] = 0x398c0000 | MAKE_RELOC_ADDR16_LO(0x398c0000, addr);
	i_buff[3] = 0x7d8903a6;
	i_buff[4] = 0x81810004;
	i_buff[5] = 0x4e800420;
}

static ConvertStatus cnvFarCode(RSOObjectHeader *rso, RSOImportTable *impTab,
                                int impIndex, u32 addr, u32 *i_buff)
{
	RSORel *rel;
	u32 offset;
	u32 *p;
	u32 x;
	u32 y;
	ConvertStatus i_sw = ConvertNone;

	for (rel = (RSORel *)(rso->externalRelOffset + impTab->relOffset);
	     ELF32_R_SYM(rel->info) == impIndex; ++rel)
	{
		if (ELF32_R_TYPE(rel->info) != R_PPC_REL24)
			continue;

		offset = addr;
		p = (u32 *)rel->offset;
		x = offset + rel->addend - (u32)p;

		*p = MAKE_RELOC_REL24(*p, x);

		x = offset + rel->addend;
		y = MAKE_RELOC_REL24((u32)p, ((u32)p & 0x03fffffc) + (*p & 0x03fffffc));

		if (x != 0 && x != y)
		{
			if (i_sw == ConvertNone)
			{
				makeCode(addr, i_buff);
				i_sw = ConvertSuccess;
			}

			x = (u32)i_buff - (u32)p;

			if (x > ~0x01ffffff + 1 || x < 0x01ffffff)
			{
				*p = MAKE_RELOC_REL24(*p, x);

				DCFlushRange(p, ROUND_UP(sizeof *p, 32));
				ICInvalidateRange(p, ROUND_UP(sizeof *p, 32));
			}
			else
			{
				i_sw = ConvertFailure;
				break;
			}
		}
	}

	return i_sw;
}

static ConvertStatus cnvJumpCode(RSOObjectHeader *rso, RSOImportTable *impTab,
                                 int impIndex, u32 addr, u32 *i_buff)
{
	RSORel *rel;
	u32 offset;
	u32 *p;
	u32 x;
	u32 y;
	ConvertStatus i_sw = ConvertNone;

	for (rel = (RSORel *)(rso->externalRelOffset + impTab->relOffset);
	     ELF32_R_SYM(rel->info) == impIndex; ++rel)
	{
		if (ELF32_R_TYPE(rel->info) != R_PPC_REL24)
			continue;

		offset = addr;
		p = (u32 *)rel->offset;
		x = offset + rel->addend - (u32)p;

		*p = MAKE_RELOC_REL24(*p, x);

		x = offset + rel->addend;
		y = MAKE_RELOC_REL24((u32)p, ((u32)p & 0x03fffffc) + (*p & 0x03fffffc));

		if (x != 0 && x != y)
		{
			i_sw = ConvertSuccess;

			x = (u32)i_buff - (u32)p;

			if (x > ~0x01ffffff + 1 || x < 0x01ffffff)
			{
				*p = MAKE_RELOC_REL24(*p, x);

				DCFlushRange(p, ROUND_UP(sizeof *p, 32));
				ICInvalidateRange(p, ROUND_UP(sizeof *p, 32));
			}
			else
			{
				i_sw = ConvertFailure;
				break;
			}
		}
	}

	return i_sw;
}

int RSOGetFarCodeSize(RSOObjectHeader *i_rsoImp, RSOObjectHeader *i_rsoExp)
{
	RSOSymbolHeader *imp = &i_rsoImp->impHeader;
	int i;
	int count = 0;
	char *impName;
	u32 *addr;
	int s_max = RSOGetNumImportSymbols(imp);
	RSOImportTable *impTab = (RSOImportTable *)imp->tableOffset;

	/* explicitly post-increment */
	for (i = 0; i < s_max; i++, impTab++)
	{
		impName = (char *)(impTab->strOffset + imp->stringOffset);
		if (impName == nullptr)
			continue;

		addr = RSOFindExportSymbolAddr(i_rsoExp, impName);
		if (!addr)
			continue;

		if (!checkNeedFarCode(i_rsoImp, impTab, i, (u32)addr))
			continue;

		++count;
	}

	return JUMP_CODE_SIZE * count;
}

int RSOLinkFar(RSOObjectHeader *i_rsoImp, RSOObjectHeader *i_rsoExp,
               void *i_buff)
{
	RSOSymbolHeader *imp = &i_rsoImp->impHeader;
	int i;
	int count = 0;
	char *impName;
	u32 *addr;
	int s_max = RSOGetNumImportSymbols(imp);
	RSOImportTable *impTab = (RSOImportTable *)imp->tableOffset;
	u32 *r_buff = i_buff;
	ConvertStatus ret;

	RSONotifyPreRSOLinkFar(i_rsoImp, i_rsoExp, i_buff);

	/* explicitly post-increment */
	for (i = 0; i < s_max; i++, impTab++)
	{
		impName = (char *)(impTab->strOffset + imp->stringOffset);
		if (impName == nullptr)
			continue;

		addr = RSOFindExportSymbolAddr(i_rsoExp, impName);
		if (!addr)
			continue;

		ret = cnvFarCode(i_rsoImp, impTab, i, (u32)addr, r_buff);

		if (ret == ConvertSuccess)
		{
			++count;
			r_buff += JUMP_CODE_INST_COUNT;
		}
		else if (ret == ConvertFailure)
		{
			count = -1;
			break;
		}
	}

	DCFlushRange(i_buff, JUMP_CODE_SIZE * count);
	ICInvalidateRange(i_buff, JUMP_CODE_SIZE * count);

	RSONotifyPostRSOLinkFar(i_rsoImp, i_rsoExp, i_buff);

	return count;
}

int RSOGetJumpCodeSize(RSOObjectHeader *i_rsoExp)
{
	return JUMP_CODE_SIZE * RSOGetNumExportSymbols(&i_rsoExp->expHeader);
}

/* Using const instead of restrict here makes more logical sense, but I'd like
 * to figure out how much of the library *should* be using const and apply it
 * all at once instead of adding just the one const and propagation. Perhaps
 * another PR for another time.
 */
void RSOMakeJumpCode(RSOObjectHeader * restrict i_rsoExp, void *i_buff)
{
	int s_max = RSOGetNumExportSymbols(&i_rsoExp->expHeader);
	u32 *r_buff = i_buff;
	u32 a_addr;
	int i;

	for (i = 0; i < s_max; ++i)
	{
		a_addr = (u32)RSOGetExportSymbolAddr(i_rsoExp, (u32)i);
		makeCode(a_addr, r_buff);

		r_buff += JUMP_CODE_INST_COUNT;
	}

	DCFlushRange(i_buff, JUMP_CODE_SIZE * s_max);
	ICInvalidateRange(i_buff, JUMP_CODE_SIZE * s_max);
}

int RSOLinkJump(RSOObjectHeader *i_rsoImp, RSOObjectHeader *i_rsoExp,
                void *i_buff)
{
	RSOSymbolHeader *imp = &i_rsoImp->impHeader;
	int i;
	int count = 0;
	char *impName;
	u32 *addr;
	int s_max = RSOGetNumImportSymbols(imp);
	RSOImportTable *impTab = (RSOImportTable *)imp->tableOffset;
	u32 *a_buff = i_buff;
	int a_idx;
	ConvertStatus ret;

	RSONotifyPreRSOLinkFar(i_rsoImp, i_rsoExp, i_buff);

	/* explicitly post-increment */
	for (i = 0; i < s_max; i++, impTab++)
	{
		impName = (char *)(impTab->strOffset + imp->stringOffset);
		if (impName == nullptr)
			continue;

		a_idx = FindExportIndex(i_rsoExp, impName);
		if (a_idx < 0)
			continue;

		addr = RSOGetExportSymbolAddr(i_rsoExp, a_idx);

		ret = cnvJumpCode(i_rsoImp, impTab, i, (u32)addr,
		                  a_buff + JUMP_CODE_INST_COUNT * a_idx);

		if (ret == ConvertSuccess)
		{
			++count;
		}
		else if (ret == ConvertFailure)
		{
			count = -1;
			break;
		}
	}

	RSONotifyPostRSOLinkFar(i_rsoImp, i_rsoExp, i_buff);

	return count;
}
