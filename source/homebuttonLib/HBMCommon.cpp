#include <revolution/hbm.h>
#include "HBMCommon.h"

/*******************************************************************************
 * headers
 */

#include <macros.h>

#include "buildstamp.h"
#include <revolution/types.h>

#include "HBMAxSound.h"
#include "HBMBase.h"

#include "nw4hbm/lyt/layout.h"
#include "nw4hbm/ut/Font.h" // IWYU pragma: keep (text)

// #include <revolution/mem/expHeap.h>
// #include <revolution/mem/allocator.h>
// #include <revolution/os.h>

#include <context_rvl.h>

/*******************************************************************************
 * macros
 */

#define RVL_SDK_HBM_DEBUG_BUILD_DATE	"Feb 24 2010"
#define RVL_SDK_HBM_DEBUG_BUILD_TIME	"16:18:45"

#define RVL_SDK_HBM_RELEASE_BUILD_DATE	"Feb 24 2010"
#define RVL_SDK_HBM_RELEASE_BUILD_TIME	"16:19:07"

/*******************************************************************************
 * types
 */

// [SC5PGN]/build/libs/Debug/slamWiiD.a:HBMCommon.o(1)::.debug_info::0x2c0 [original object]
enum HBMAllocatorType
{
	HBM_ALLOCATOR_APPLI/*cation*/,
	HBM_ALLOCATOR_LOCAL,
	HBM_ALLOCATOR_NW4R
};

/*******************************************************************************
 * local function declarations
 */

static HBMAllocatorType getAllocatorType(HBMDataInfo const *info);

/*******************************************************************************
 * variables
 */

// .data
char const *__HBMVersion = RVL_SDK_LIB_VERSION_STRING(HBM);

// .bss
static MEMAllocator sAllocator;

// .data (not .sdata; hbm is -sdata 0)
MEMAllocator *spAllocator = &sAllocator;

/*******************************************************************************
 * functions
 */

void *HBMAllocMem(u32 size)
{
	void *addr = MEMAllocFromAllocator(spAllocator, size);

	// Stripped assert?

	return addr;
}

void HBMFreeMem(void *mem)
{
	MEMFreeToAllocator(spAllocator, mem);
}

static HBMAllocatorType getAllocatorType(HBMDataInfo const *pHBInfo)
{
	if (pHBInfo->pAllocator)
		return HBM_ALLOCATOR_APPLI;
	else if (pHBInfo->mem)
		return HBM_ALLOCATOR_LOCAL;
	else
		return HBM_ALLOCATOR_NW4R;
}

void HBMCreate(HBMDataInfo const *pHBInfo)
{
	MEMiHeapHead *hExpHeap;

	if (getAllocatorType(pHBInfo) == HBM_ALLOCATOR_LOCAL)
	{
		hExpHeap = MEMCreateExpHeap(pHBInfo->mem, pHBInfo->memSize);
		MEMInitAllocatorForExpHeap(&sAllocator, hExpHeap, 32);
		spAllocator = &sAllocator;
	}

	switch (getAllocatorType(pHBInfo))
	{
	case HBM_ALLOCATOR_APPLI:
		nw4hbm::lyt::Layout::SetAllocator(pHBInfo->pAllocator);
		spAllocator = pHBInfo->pAllocator;
		break;

	case HBM_ALLOCATOR_LOCAL:
		nw4hbm::lyt::Layout::SetAllocator(spAllocator);
		break;

	case HBM_ALLOCATOR_NW4R:
		OSAssert_Line(87, nw4hbm::lyt::Layout::GetAllocator());

		spAllocator = nw4hbm::lyt::Layout::GetAllocator();
		break;
	}

	homebutton::HomeButton::createInstance(pHBInfo);
	homebutton::HomeButton::getInstance()->create();
}

void HBMDelete(void)
{
	HBMDataInfo const *pHBInfo =
		homebutton::HomeButton::getInstance()->getHBMDataInfo();

	homebutton::HomeButton::deleteInstance();

	if (getAllocatorType(pHBInfo) == HBM_ALLOCATOR_LOCAL)
		MEMDestroyExpHeap(spAllocator->pHeap);
}

void HBMInit(void)
{
	OSAssert_Line(113, homebutton::HomeButton::getInstance());

	OSRegisterVersion(__HBMVersion);
	homebutton::HomeButton::getInstance()->init();
}

HBMSelectBtnNum HBMCalc(HBMControllerData const *pController)
{
	OSAssert_Line(120, homebutton::HomeButton::getInstance());

	homebutton::HomeButton::getInstance()->calc(pController);
	return homebutton::HomeButton::getInstance()->getSelectBtnNum();
}

void HBMDraw(void)
{
	OSAssert_Line(128, homebutton::HomeButton::getInstance());

	homebutton::HomeButton::getInstance()->draw();
}

HBMSelectBtnNum HBMGetSelectBtnNum(void)
{
	return homebutton::HomeButton::getInstance()->getSelectBtnNum();
}

void HBMSetAdjustFlag(BOOL flag)
{
	homebutton::HomeButton::getInstance()->setAdjustFlag(flag);
}

void HBMStartBlackOut(void)
{
	homebutton::HomeButton::getInstance()->startBlackOut();
}

BOOL HBMIsReassignedControllers(void)
{
	OSAssert_Line(149, homebutton::HomeButton::getInstance());

	return homebutton::HomeButton::getInstance()->getReassignedFlag();
}

/* New! */
void HBMSetBlackOutColor(u8 red, u8 green, u8 blue)
{
	OSAssert_Line(155, homebutton::HomeButton::getInstance());

	homebutton::HomeButton::getInstance()->setBlackOutColor(red, green, blue);
}

/* New! */
GXColor HBMGetBlackOutColor(void)
{
	OSAssert_Line(161, homebutton::HomeButton::getInstance());

	return homebutton::HomeButton::getInstance()->getBlackOutColor();
}

/* New! */
void HBMSetVIBlack(BOOL flag)
{
	OSAssert_Line(167, homebutton::HomeButton::getInstance());

	homebutton::HomeButton::getInstance()->setVIBlack(flag);
}

void HBMCreateSound(void *soundData, void *memBuf, u32 memSize)
{
	HBMDataInfo const *pHBInfo ATTR_UNUSED =
		homebutton::HomeButton::getInstance()->getHBMDataInfo();

	// Stripped assert?

	homebutton::InitAxSound(soundData, memBuf, memSize);
}

void HBMDeleteSound(void)
{
	homebutton::ShutdownAxSound();
}

void HBMUpdateSound(void)
{
	OSAssert_Line(212, homebutton::HomeButton::getInstance());

	homebutton::HomeButton::getInstance()->update_sound();
}
