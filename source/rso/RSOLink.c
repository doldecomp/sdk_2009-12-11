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
#define NULL	((void *)0)

#define BIND_POINTER(target_, base_)	\
	do { *(u32 *)&(target_) += (u32)(base_); } while (false)

#define UNBIND_POINTER(target_, base_)	\
	do { *(u32 *)&(target_) -= (u32)(base_); } while (false)

// TODO: what is this
struct mtsi {char _[12]; RSOSectionInfo *at_0x0c;};
#define MODULE_TO_SECTION_INFO(module_)	\
	(((struct mtsi *)(module_))->at_0x0c)

// TODO: rename these
#define MACRO_1(a_, b_)						\
	do										\
	{										\
		RSOObjectInfo *__prev = (a_)->tail;	\
											\
		if (__prev == nullptr)				\
			(a_)->head = (b_);				\
		else								\
			__prev->link.next = (b_);		\
											\
		(b_)->link.prev = __prev;			\
		(b_)->link.next = nullptr;			\
											\
		(a_)->tail = (b_);					\
	} while (false)

#define MACRO_2(a_, b_)								\
	do												\
	{												\
		RSOObjectInfo *__next = (b_)->link.next;	\
		RSOObjectInfo *__prev = (b_)->link.prev;	\
													\
		if (__next == nullptr)						\
			(a_)->tail = __prev;					\
		else										\
			__next->link.prev = __prev;				\
													\
		if (__prev == nullptr)						\
			(a_)->head = __next;					\
		else										\
			__prev->link.next = __next;				\
	} while (false)

/*******************************************************************************
 * local function declarations
 */

static BOOL LocateObject(void *newModule, void *bss,
                         RSOFixedLevel i_fixed_level);

static void RSOResolveImportSymbol(RSOObjectHeader *rsoImp, int index,
                                   void *addr);
static void RSOUnresolveImportSymbol(RSOObjectHeader *rsoImp, int index);
static void RSOUnresolveImportSymbolAll(RSOObjectHeader *rso);

static int FindExportIndex(RSOObjectHeader *rso, char const *name);

static void RSORelocate(RSORel *rel, int index, u32 offset);
static void RSORelocateImportSymbol(RSOObjectHeader *rso,
                                    RSOImportTable *impTab, int impIndex);
static void RSORelocateSmallDataSection(RSOObjectHeader *rsoImp, int impIndex,
                                        RSOObjectHeader *rsoExp);

static BOOL LinkList(void *i_newRso, void *i_bss, RSOFixedLevel i_fixed_level);

static BOOL checkNeedFarCode(RSOObjectHeader *rso, RSOImportTable *impTab,
                             int impIndex, u32 addr);
static void makeCode(u32 addr, u32 *i_buff);
static int cnvFarCode(RSOObjectHeader *rso, RSOImportTable *impTab,
                      int impIndex, u32 addr, u32 *i_buff);
static int cnvJumpCode(RSOObjectHeader *rso, RSOImportTable *impTab,
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
	return;
}

ATTR_WEAK ATTR_NOINLINE void RSONotifyModuleUnloaded(
	RSOObjectHeader *moduleHeader ATTR_UNUSED)
{
	return;
}

ATTR_WEAK ATTR_NOINLINE void RSONotifyPreRSOLink(
	RSOObjectHeader *rsoImp ATTR_UNUSED, RSOObjectHeader *rsoExp ATTR_UNUSED)
{
	return;
}

ATTR_WEAK ATTR_NOINLINE void RSONotifyPostRSOLink(
	RSOObjectHeader *rsoImp ATTR_UNUSED, RSOObjectHeader *rsoExp ATTR_UNUSED)
{
	return;
}

ATTR_WEAK ATTR_NOINLINE void RSONotifyPreRSOLinkFar(
	RSOObjectHeader *i_rsoImp ATTR_UNUSED,
	RSOObjectHeader *i_rsoExp ATTR_UNUSED, void *i_buff ATTR_UNUSED)
{
	return;
}

ATTR_WEAK ATTR_NOINLINE void RSONotifyPostRSOLinkFar(
	RSOObjectHeader *i_rsoImp ATTR_UNUSED,
	RSOObjectHeader *i_rsoExp ATTR_UNUSED, void *i_buff ATTR_UNUSED)
{
	return;
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
		                     + (intRel->info >> 8);

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

	for (i = 0; i < s_max; i++, impTab++) /* explicitly post-increment */
	{
		impName = (char *)(impTab->strOffset + imp->stringOffset);
		if (impName == nullptr)
			continue;

		RSORelocateSmallDataSection(rsoImp, i, rsoExp);

		addr = RSOFindExportSymbolAddr(rsoExp, impName);
		if (addr)
		{
			RSOResolveImportSymbol(rsoImp, i, addr);

			++count;
		}
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

u32 RSOGetHash(char const *symbolname)
{
	u32 h = 0;
	u32 g;

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

int RSOGetNumExportSymbols(RSOSymbolHeader const *exp)
{
	return exp->tableSize / sizeof(RSOExportTable);
}

char *RSOGetExportSymbolName(RSOSymbolHeader *exp, int index)
{
	RSOExportTable *expTab = (RSOExportTable *)exp->tableOffset + index;

	return (char *)(expTab->strOffset + exp->stringOffset);
}

void *RSOGetExportSymbolAddr(RSOObjectHeader const *rso, int index)
{
	RSOExportTable const *expTab =
		(RSOExportTable const *)rso->expHeader.tableOffset + index;
	RSOSectionInfo const *expSec =
		(RSOSectionInfo const *)rso->info.sectionInfoOffset + expTab->section;

	return (void *)(expSec->offset + expTab->value);
}

static int FindExportIndex(RSOObjectHeader *rso, char const *name)
{
	u32 a_hash = RSOGetHash(name);
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

void *RSOFindExportSymbolAddr(RSOObjectHeader *rso, char const *name)
{
	int a_idx = FindExportIndex(rso, name);

	if (a_idx == -1)
		return nullptr;

	return RSOGetExportSymbolAddr(rso, a_idx);
}

RSOExportTable *RSOFindExportSymbol(RSOObjectHeader *rso, char const *name)
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

	switch (rel->info & 0xff)
	{
	case R_PPC_NONE:
		break;

	case R_PPC_ADDR32:
		x = offset + rel->addend;
		*p = x;
		break;

	case R_PPC_ADDR24:
		x = offset + rel->addend;
		*p = (*p & ~0x03fffffc) | (x & 0x03fffffc);
		break;

	case R_PPC_ADDR16:
		x = offset + rel->addend;
		*(u16 *)p = x & 0xffff;
		break;

	case R_PPC_ADDR16_LO:
		x = offset + rel->addend;
		*(u16 *)p = x & 0xffff;
		break;

	case R_PPC_ADDR16_HI:
		x = offset + rel->addend;
		*(u16 *)p = (x >> 16) & 0xffff;
		break;

	case R_PPC_ADDR16_HA:
		x = offset + rel->addend;
		*(u16 *)p = ((x >> 16) + (x & 0x8000 ? 1 : 0)) & 0xffff;
		break;

	case R_PPC_ADDR14:
	case R_PPC_ADDR14_BRTAKEN:
	case R_PPC_ADDR14_BRNTAKEN:
		x = offset + rel->addend;
		*p = (*p & ~0x0000fffc) | (x & 0x0000fffc);
		break;

	case R_PPC_REL24:
		x = offset + rel->addend - (u32)p;
		*p = (*p & ~0x03fffffc) | (x & 0x03fffffc);

		x = offset + rel->addend;
		y = ((u32)p & ~0x03fffffc)
		  | ((((u32)p & 0x03fffffc) + (*p & 0x03fffffc))
		     & 0x03fffffc);

		if (x != 0 && x != y)
			*p = (*p & ~0x03fffffc) | 0x03fffffc;

		break;

	case R_PPC_REL14:
	case R_PPC_REL14_BRTAKEN:
	case R_PPC_REL14_BRNTAKEN:
		x = offset + rel->addend - (u32)p;
		*(u32 *)p = (*(u32 *)p & ~0x0000fffc) | (x & 0x0000fffc);
		break;

	case R_PPC_EMB_SDA21:
		break;

	default:
		OSReport("OSLink: unknown relocation type %3d\n", (u8)rel->info);
		break;
	}

	DCFlushRange(p, 0x20);
	ICInvalidateRange(p, 0x20);
}

static void RSORelocateImportSymbol(RSOObjectHeader *rso,
                                    RSOImportTable *impTab, int impIndex)
{
	RSORel *ret;

	for (ret = (RSORel *)(rso->externalRelOffset + impTab->relOffset);
	     (ret->info >> 8) == impIndex; ++ret)
	{
		RSORelocate(ret, 0, impTab->value);
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

	while ((rel->info >> 8) == impIndex)
	{
		if ((rel->info & 0xff) == R_PPC_EMB_SDA21)
		{
			p = (u8 *)rel->offset;

			switch (rel->offset & 0x03)
			{
			case 0x00:
				p += 1;
				break;

			case 0x01:
				// want 0x01
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
					*p = (*p & ~0x1f) | 13; // r13

					baseTab = RSOFindExportSymbol(rsoExp, "_SDA_BASE_");

					if (baseTab == nullptr)
					{
						++rel;
						goto skip;
					}

					base = baseTab->value;
					break;

				case RSO_SECTION_SDATA2:
				case RSO_SECTION_SBSS2:
					*p = (*p & ~0x1f) | 2; // r2

					baseTab = RSOFindExportSymbol(rsoExp, "_SDA2_BASE_");

					if (baseTab == nullptr)
					{
						++rel;
						goto skip;
					}

					base = baseTab->value;
					break;

				case RSO_SECTION_EMB_SDATA0:
				case RSO_SECTION_EMB_SBSS0:
					*p = (*p & ~0x1f) | 0; // (rA|0)
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
				*(u16 *)p = x & 0xffff;
				--p;

				DCFlushRange(p, 0x20);
				ICInvalidateRange(p, 0x20);
			}
		}

		++rel;
	skip:;
	}
}

BOOL RSOListInit(void *i_staticRso)
{
	BOOL r;
	RSOObjectHeader *a_rsoInfo = i_staticRso;

	__RSOObjectInfoList.head = __RSOObjectInfoList.tail = nullptr;

	r = RSOStaticLocateObject(i_staticRso);

	MACRO_1(&__RSOObjectInfoList, &a_rsoInfo->info);

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

	MACRO_1(&__RSOObjectInfoList, (RSOObjectInfo *)i_newRso);

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

	MACRO_2(&__RSOObjectInfoList, objectInfo);

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
		OSAssert_Line(1495, 0); // not FALSE again
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
	     (rel->info >> 8) == impIndex; ++rel)
	{
		if ((rel->info & 0xff) != R_PPC_REL24)
			continue;

		offset = addr;
		p = (u32 *)rel->offset;
		x = offset + rel->addend - (u32)p;

		*p = (*p & ~0x03fffffc) | (x & 0x03fffffc);

		x = offset + rel->addend;
		y = ((u32)p & ~0x03fffffc)
		  | ((((u32)p & 0x03fffffc) + (*p & 0x03fffffc)) & 0x03fffffc);

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
	lis		r12, r3@ha
	addi	r12, r12, r3@l
	mtctr	r12
	lwz		r12, 4(r1)
	bctr
*/

	i_buff[0] = 0x91810004;
	i_buff[1] =
		0x3d800000 | ((((addr >> 16) + (addr & (1 << 15) ? 1 : 0)) & 0xffff));
	i_buff[2] = 0x398c0000 | (addr & 0xffff);
	i_buff[3] = 0x7d8903a6;
	i_buff[4] = 0x81810004;
	i_buff[5] = 0x4e800420;
}

static int cnvFarCode(RSOObjectHeader *rso, RSOImportTable *impTab,
                      int impIndex, u32 addr, u32 *i_buff)
{
	RSORel *rel;
	u32 offset;
	u32 *p;
	u32 x;
	u32 y;
	int i_sw = 0;

	for (rel = (RSORel *)(rso->externalRelOffset + impTab->relOffset);
	     (rel->info >> 8) == impIndex; ++rel)
	{
		if ((rel->info & 0xff) != R_PPC_REL24)
			continue;

		offset = addr;
		p = (u32 *)rel->offset;
		x = offset + rel->addend - (u32)p;

		*p = (*p & ~0x03fffffc) | (x & 0x03fffffc);

		x = offset + rel->addend;
		y = ((u32)p & ~0x03fffffc)
		  | ((((u32)p & 0x03fffffc) + (*p & 0x03fffffc)) & 0x03fffffc);

		if (x == 0 || x == y)
			continue;

		if (!i_sw)
		{
			makeCode(addr, i_buff);
			i_sw = 1;
		}

		x = (u32)i_buff - (u32)p;

		if (x > ~0x01fffffe || x < 0x01ffffff)
		{
			*p = (*p & ~0x03fffffc) | (x & 0x03fffffc);

			DCFlushRange(p, 0x20);
			ICInvalidateRange(p, 0x20);
		}
		else
		{
			i_sw = -1;
			break;
		}
	}

	return i_sw;
}

static int cnvJumpCode(RSOObjectHeader *rso, RSOImportTable *impTab,
                       int impIndex, u32 addr, u32 *i_buff)
{
	RSORel *rel;
	u32 offset;
	u32 *p;
	u32 x;
	u32 y;
	int i_sw = 0;

	for (rel = (RSORel *)(rso->externalRelOffset + impTab->relOffset);
	     (rel->info >> 8) == impIndex; ++rel)
	{
		if ((rel->info & 0xff) != R_PPC_REL24)
			continue;

		offset = addr;
		p = (u32 *)rel->offset;
		x = offset + rel->addend - (u32)p;

		*p = (*p & ~0x03fffffc) | (x & 0x03fffffc);

		x = offset + rel->addend;
		y = ((u32)p & ~0x03fffffc)
		  | ((((u32)p & 0x03fffffc) + (*p & 0x03fffffc)) & 0x03fffffc);

		if (x == 0 || x == y)
			continue;

		i_sw = 1;

		x = (u32)i_buff - (u32)p;

		if (x > ~0x01fffffe || x < 0x01ffffff)
		{
			*p = (*p & ~0x03fffffc) | (x & 0x03fffffc);

			DCFlushRange(p, 0x20);
			ICInvalidateRange(p, 0x20);
		}
		else
		{
			i_sw = -1;
			break;
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

	for (i = 0; i < s_max; i++, impTab++) /* explicitly post-increment */
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

	return count * (sizeof(u32) * 6);
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
	int ret;

	RSONotifyPreRSOLinkFar(i_rsoImp, i_rsoExp, i_buff);

	for (i = 0; i < s_max; i++, impTab++) /* explicitly post-increment */
	{
		impName = (char *)(impTab->strOffset + imp->stringOffset);
		if (impName == nullptr)
			continue;

		addr = RSOFindExportSymbolAddr(i_rsoExp, impName);
		if (!addr)
			continue;

		ret = cnvFarCode(i_rsoImp, impTab, i, (u32)addr, r_buff);

		if (ret == 1)
		{
			++count;
			r_buff += 6;
		}
		else if (ret == -1)
		{
			count = -1;
			break;
		}
	}

	DCFlushRange(i_buff, count * (sizeof(u32) * 6));
	ICInvalidateRange(i_buff, count * (sizeof(u32) * 6));

	RSONotifyPostRSOLinkFar(i_rsoImp, i_rsoExp, i_buff);

	return count;
}

int RSOGetJumpCodeSize(RSOObjectHeader *i_rsoExp)
{
	return RSOGetNumExportSymbols(&i_rsoExp->expHeader) * (sizeof(u32) * 6);
}

void RSOMakeJumpCode(RSOObjectHeader const *i_rsoExp, void *i_buff)
{
	int s_max = RSOGetNumExportSymbols(&i_rsoExp->expHeader);
	u32 *r_buff = i_buff;
	u32 a_addr;
	int i;

	for (i = 0; i < s_max; ++i)
	{
		a_addr = (u32)RSOGetExportSymbolAddr(i_rsoExp, (u32)i);

		makeCode(a_addr, r_buff);
		r_buff += 6;
	}

	DCFlushRange(i_buff, (sizeof(u32) * 6) * s_max);
	ICInvalidateRange(i_buff, (sizeof(u32) * 6) * s_max);
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
	int ret;

	RSONotifyPreRSOLinkFar(i_rsoImp, i_rsoExp, i_buff);

	for (i = 0; i < s_max; i++, impTab++) /* explicitly post-increment */
	{
		impName = (char *)(impTab->strOffset + imp->stringOffset);
		if (impName == nullptr)
			continue;

		a_idx = FindExportIndex(i_rsoExp, impName);
		if (a_idx < 0)
			continue;

		addr = RSOGetExportSymbolAddr(i_rsoExp, a_idx);

		ret = cnvJumpCode(i_rsoImp, impTab, i, (u32)addr, a_buff + 6 * a_idx);

		if (ret == 1)
		{
			++count;
		}
		else if (ret == -1)
		{
			count = -1;
			break;
		}
	}

	RSONotifyPostRSOLinkFar(i_rsoImp, i_rsoExp, i_buff);

	return count;
}
