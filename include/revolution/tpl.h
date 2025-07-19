#ifndef RVL_SDK_TPL_H
#define RVL_SDK_TPL_H

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

#if 0
#include <revolution/gx/GXEnum.h>
#else
#include <context_rvl.h>
#endif

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

// [SPQE7T]/ISpyD.elf:.debug_info::0x288606
typedef struct TPLHeader
{
	u16				height;			// size 0x02, offset 0x00
	u16				width;			// size 0x02, offset 0x02
	u32				format;			// size 0x04, offset 0x04
	char			*data;			// size 0x04, offset 0x08
	GXTexWrapMode	wrapS;			// size 0x04, offset 0x0c
	GXTexWrapMode	wrapT;			// size 0x04, offset 0x10
	GXTexFilter		minFilter;		// size 0x04, offset 0x14
	GXTexFilter		magFilter;		// size 0x04, offset 0x18
	f32				LODBias;		// size 0x04, offset 0x1c
	u8				edgeLODEnable;	// size 0x01, offset 0x20
	u8				minLOD;			// size 0x01, offset 0x21
	u8				maxLOD;			// size 0x01, offset 0x22
	u8				unpacked;		// size 0x01, offset 0x23
} TPLHeader; // size 0x24

// [SPQE7T]/ISpyD.elf:.debug_info::0x28870b
typedef struct TPLClutHeader
{
	u16			numEntries;	// size 0x02, offset 0x00
	u8			unpacked;	// size 0x01, offset 0x02
	byte1_t		pad8;
	GXTlutFmt	format;		// size 0x04, offset 0x04
	char		*data;		// size 0x04, offset 0x08
} TPLClutHeader; // size 0x0c

// [SPQE7T]/ISpyD.elf:.debug_info::0x2885be
typedef struct TPLDescriptor
{
	TPLHeader		*textureHeader;	// size 0x04, offset 0x00
	TPLClutHeader	*CLUTHeader;	// size 0x04, offset 0x04
} TPLDescriptor; // size 0x08

// [SPQE7T]/ISpyD.elf:.debug_info::0x28855b
typedef struct TPLPalette
{
	byte4_t			versionNumber;		// size 0x04, offset 0x00
	u32				numDescriptors;		// size 0x04, offset 0x04
	TPLDescriptor	*descriptorArray;	// size 0x04, offset 0x08
} TPLPalette; // size 0x0c

/*******************************************************************************
 * functions
 */

void TPLBind(TPLPalette *pal);
TPLDescriptor *TPLGet(TPLPalette *pal, u32 id);
void TPLGetGXTexObjFromPalette(TPLPalette *palette, GXTexObj *texObj, u32 id);
void TPLGetGXTexObjFromPaletteCI(TPLPalette *palette, GXTexObj *texObj,
                                 GXTlutObj *tlutObj, u32, u32 id);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_TPL_H
