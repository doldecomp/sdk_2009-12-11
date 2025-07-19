#include <revolution/tpl.h>

/*******************************************************************************
 * headers
 */

#include <macros.h> // POINTER_ADD_TYPE

#include <revolution/types.h>

#if 0
#include <revolution/os/OSError.h>
#else
#include <context_rvl.h>
#endif

/*******************************************************************************
 * macros
 */

#define TPL_VERSION						2142000

#define BIND_POINTER(palette_, item_)	\
	POINTER_ADD_TYPE(__typeof__((palette_)->item_), palette_, (palette_)->item_)

/*******************************************************************************
 * functions
 */

void TPLBind(TPLPalette *palette)
{
	u16 i;

	if (palette->versionNumber != TPL_VERSION)
		OSError_Line(25, "invalid version number for texture palette");

	// is this accidentally backwards or what
	// palette->descriptorArray = BIND_POINTER(palette, descriptorArray);
	palette->descriptorArray =
		POINTER_ADD_TYPE(TPLDescriptor *, palette->descriptorArray, palette);

	for (i = 0; i < palette->numDescriptors; ++i)
	{
		if (palette->descriptorArray[i].textureHeader)
		{
			palette->descriptorArray[i].textureHeader =
				BIND_POINTER(palette, descriptorArray[i].textureHeader);

			if (!palette->descriptorArray[i].textureHeader->unpacked)
			{
				palette->descriptorArray[i].textureHeader->data = BIND_POINTER(
					palette, descriptorArray[i].textureHeader->data);

				palette->descriptorArray[i].textureHeader->unpacked = true;
			}
		}

		if (palette->descriptorArray[i].CLUTHeader)
		{
			palette->descriptorArray[i].CLUTHeader =
				BIND_POINTER(palette, descriptorArray[i].CLUTHeader);

			if (!palette->descriptorArray[i].CLUTHeader->unpacked)
			{
				palette->descriptorArray[i].CLUTHeader->data =
					BIND_POINTER(palette, descriptorArray[i].CLUTHeader->data);

				palette->descriptorArray[i].CLUTHeader->unpacked = true;
			}
		}
	}
}

TPLDescriptor *TPLGet(TPLPalette *palette, u32 id)
{
	OSAssertMessage_Line(60, id < palette->numDescriptors,
	                     "TPLGet: non-existing id.");

	id %= palette->numDescriptors;
	return &palette->descriptorArray[id];
}

void TPLGetGXTexObjFromPalette(TPLPalette *palette, GXTexObj *texObj, u32 id)
{
	TPLDescriptor *descriptor = TPLGet(palette, id);

	GXBool a;
	if (descriptor->textureHeader->minLOD == descriptor->textureHeader->maxLOD)
		a = GX_FALSE;
	else
		a = GX_TRUE;

	GXInitTexObj(
		texObj, descriptor->textureHeader->data,
		descriptor->textureHeader->width, descriptor->textureHeader->height,
		descriptor->textureHeader->format, descriptor->textureHeader->wrapS,
		descriptor->textureHeader->wrapT, a);

	GXInitTexObjLOD(
		texObj, descriptor->textureHeader->minFilter,
		descriptor->textureHeader->magFilter, descriptor->textureHeader->minLOD,
		descriptor->textureHeader->maxLOD, descriptor->textureHeader->LODBias,
		GX_FALSE, descriptor->textureHeader->edgeLODEnable, GX_ANISO_1);
}

void TPLGetGXTexObjFromPaletteCI(TPLPalette *palette, GXTexObj *texObj,
                                 GXTlutObj *tlutObj, u32 param_4, u32 id)
{
	TPLDescriptor *descriptor = TPLGet(palette, id);

	GXBool a;
	if (descriptor->textureHeader->minLOD == descriptor->textureHeader->maxLOD)
		a = GX_FALSE;
	else
		a = GX_TRUE;

	GXInitTlutObj(tlutObj, descriptor->CLUTHeader->data,
	              descriptor->CLUTHeader->format,
	              descriptor->CLUTHeader->numEntries);

	GXInitTexObjCI(
		texObj, descriptor->textureHeader->data,
		descriptor->textureHeader->width, descriptor->textureHeader->height,
		descriptor->textureHeader->format, descriptor->textureHeader->wrapS,
		descriptor->textureHeader->wrapT, a, param_4);

	GXInitTexObjLOD(
		texObj, descriptor->textureHeader->minFilter,
		descriptor->textureHeader->magFilter, descriptor->textureHeader->minLOD,
		descriptor->textureHeader->maxLOD, descriptor->textureHeader->LODBias,
		GX_FALSE, descriptor->textureHeader->edgeLODEnable, GX_ANISO_1);
}
