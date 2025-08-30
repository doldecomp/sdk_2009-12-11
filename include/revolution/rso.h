#ifndef RVL_SDK_RSO_H
#define RVL_SDK_RSO_H

/*******************************************************************************
 * headers
 */

#include <macros.h> // ATTR_NOINLINE

#include <revolution/types.h>

/*******************************************************************************
 * macros
 */

#define RSO_VERSION				1	/* name known from asserts */

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef u32 RSOHash;

enum RSOSection
{
	RSO_SECTION_NONE		= 0,	// ---

	RSO_SECTION_INIT		= 1,	// .init
	RSO_SECTION_TEXT		= 2,	// .text
	RSO_SECTION_CTORS		= 3,	// .ctors
	RSO_SECTION_DTORS		= 4,	// .dtors
	RSO_SECTION_RODATA		= 5,	// .rodata
	RSO_SECTION_DATA		= 6,	// .data
	RSO_SECTION_BSS			= 7,	// .bss
	RSO_SECTION_SDATA		= 8,	// .sdata
	RSO_SECTION_SDATA2		= 9,	// .sdata2
	RSO_SECTION_EMB_SDATA0	= 10,	// .PPC.EMB.sdata0
	RSO_SECTION_SBSS		= 11,	// .sbss
	RSO_SECTION_SBSS2		= 12,	// .sbss2
	RSO_SECTION_EMB_SBSS0	= 13,	// .PPC.EMB.sbss0

	RSO_SECTION_EXTABINDEX	= 241,	// extabindex
};

// [SC5PGN]/build/libs/Debug/slamWiiD.a:RSOLink.o(1)::.debug_info::0x217 (original object)
typedef enum /* explicitly untagged */
{
	RSO_FL_NON,
	RSO_FL_INTERNAL,
	RSO_FL_EXTERNAL,
} RSOFixedLevel;

typedef struct RSOObjectInfo RSOObjectInfo;

// [SC5PGN]/build/libs/Debug/slamWiiD.a:RSOLink.o(1)::.debug_info::0x4f4 (original object)
typedef struct RSOSymbolHeader
{
	u32	tableOffset;	// size 0x04, offset 0x00
	u32	tableSize;		// size 0x04, offset 0x04
	u32	stringOffset;	// size 0x04, offset 0x08
} RSOSymbolHeader; // size 0x0c

// [SC5PGN]/build/libs/Debug/slamWiiD.a:RSOLink.o(1)::.debug_info::0x4a0 (original object)
typedef struct RSOObjectLink
{
	RSOObjectInfo	*next;	// size 0x04, offset 0x00
	RSOObjectInfo	*prev;	// size 0x04, offset 0x04
} RSOObjectLink; // size 0x08

// [SC5PGN]/build/libs/Debug/slamWiiD.a:RSOLink.o(1)::.debug_info::0x410 (original object)
struct RSOObjectInfo
{
	RSOObjectLink	link;				// size 0x08, offset 0x00
	u32				numSections;		// size 0x04, offset 0x08
	u32				sectionInfoOffset;	// size 0x04, offset 0x0c
	u32				nameOffset;			// size 0x04, offset 0x10
	u32				nameSize;			// size 0x04, offset 0x14
	u32				version;			// size 0x04, offset 0x18
}; // size 0x1c

// [SC5PGN]/build/libs/Debug/slamWiiD.a:RSOLink.o(1)::.debug_info::0x2a6 (original object)
typedef struct RSOObjectHeader
{
	RSOObjectInfo	info;				// size 0x1c, offset 0x00
	u32				bssSize;			// size 0x04, offset 0x1c
	u8				prologSection;		// size 0x01, offset 0x20
	u8				epilogSection;		// size 0x01, offset 0x21
	u8				unresolvedSection;	// size 0x01, offset 0x22
	u8				bssSection;			// size 0x01, offset 0x23
	u32				prolog;				// size 0x04, offset 0x24
	u32				epilog;				// size 0x04, offset 0x28
	u32				unresolved;			// size 0x04, offset 0x2c
	u32				internalRelOffset;	// size 0x04, offset 0x30
	u32				internalRelSize;	// size 0x04, offset 0x34
	u32				externalRelOffset;	// size 0x04, offset 0x38
	u32				externalRelSize;	// size 0x04, offset 0x3c
	RSOSymbolHeader	expHeader;			// size 0x0c, offset 0x40
	RSOSymbolHeader	impHeader;			// size 0x0c, offset 0x4c
} RSOObjectHeader; // size 0x58

// [SC5PGN]/build/libs/Debug/slamWiiD.a:RSOLink.o(1)::.debug_info::0x5f1 (original object)
typedef struct RSOImportTable
{
	u32	strOffset;	// size 0x04, offset 0x00
	u32	value;		// size 0x04, offset 0x04
	u32	relOffset;	// size 0x04, offset 0x08
} RSOImportTable; // size 0x0c

// [SC5PGN]/build/libs/Debug/slamWiiD.a:RSOLink.o(1)::.debug_info::0x64b (original object)
typedef struct RSOExportTable
{
	u32		strOffset;	// size 0x04, offset 0x00
	u32		value;		// size 0x04, offset 0x04
	u32		section;	// size 0x04, offset 0x08
	RSOHash	hash;		// size 0x04, offset 0x0c
} RSOExportTable; // size 0x10

/*******************************************************************************
 * functions
 */

/* Users may define these callouts to implement custom behavior. */
/* NOTE: The ATTR_NOINLINE attribute is required here in the declaration for
 * RSOLink.c.
 */
extern ATTR_NOINLINE void RSONotifyModuleLoaded(RSOObjectHeader *moduleHeader);
extern ATTR_NOINLINE void RSONotifyModuleUnloaded(
	RSOObjectHeader *moduleHeader);
extern ATTR_NOINLINE void RSONotifyPreRSOLink(RSOObjectHeader *rsoImp,
                                              RSOObjectHeader *rsoExp);
extern ATTR_NOINLINE void RSONotifyPostRSOLink(RSOObjectHeader *rsoImp,
                                               RSOObjectHeader *rsoExp);
extern ATTR_NOINLINE void RSONotifyPreRSOLinkFar(RSOObjectHeader *i_rsoImp,
                                                 RSOObjectHeader *i_rsoExp,
                                                 void *i_buff);
extern ATTR_NOINLINE void RSONotifyPostRSOLinkFar(RSOObjectHeader *i_rsoImp,
                                                  RSOObjectHeader *i_rsoExp,
                                                  void *i_buff);

BOOL RSOLocateObject(void *newModule, void *bss);
BOOL RSOLocateObjectFixed(void *newModule, void *bss);
BOOL RSOStaticLocateObject(void *newModule);
BOOL RSOUnLocateObject(void *oldModule);
RSOImportTable *RSOGetImport(RSOSymbolHeader *imp);
RSOExportTable *RSOGetExport(RSOSymbolHeader *exp);
int RSOLink(RSOObjectHeader *rsoImp, RSOObjectHeader *rsoExp);
void RSOUnLink(RSOObjectHeader *rsoImp, RSOObjectHeader *rsoExp);
RSOHash RSOGetHash(char *symbolname);
int RSOGetNumImportSymbols(RSOSymbolHeader *imp);
int RSOGetNumImportSymbolsUnresolved(RSOObjectHeader *rso);
char *RSOGetImportSymbolName(RSOSymbolHeader *imp, int index);
BOOL RSOIsImportSymbolResolved(RSOObjectHeader *rso, int index);
BOOL RSOIsImportSymbolResolvedAll(RSOObjectHeader *rso);

int RSOGetNumExportSymbols(RSOSymbolHeader *exp);
char *RSOGetExportSymbolName(RSOSymbolHeader *exp, int index);
void *RSOGetExportSymbolAddr(RSOObjectHeader *rso, int index);

void *RSOFindExportSymbolAddr(RSOObjectHeader *rso, char *name);
RSOExportTable *RSOFindExportSymbol(RSOObjectHeader *rso, char *name);

BOOL RSOListInit(void *i_staticRso);

BOOL RSOLinkList(void *i_newRso, void *i_bss);
BOOL RSOLinkListFixed(void *i_newRso, void *i_bss, RSOFixedLevel i_fixed_level);
BOOL RSOUnLinkList(void *i_oldRso);
u32 RSOGetFixedSize(void *i_rso, RSOFixedLevel i_fixed_level);

int RSOGetFarCodeSize(RSOObjectHeader *i_rsoImp, RSOObjectHeader *i_rsoExp);
int RSOLinkFar(RSOObjectHeader *i_rsoImp, RSOObjectHeader *i_rsoExp,
               void *i_buff);
int RSOGetJumpCodeSize(RSOObjectHeader *i_rsoExp);
void RSOMakeJumpCode(RSOObjectHeader *i_rsoExp, void *i_buff);
int RSOLinkJump(RSOObjectHeader *i_rsoImp, RSOObjectHeader *i_rsoExp,
                void *i_buff);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_RSO_H
