#ifndef	RVL_SDK_HBM_H
#define RVL_SDK_HBM_H

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

#include "hbm/HBMBase.h"

#include <revolution/kpad.h>
// #include <revolution/mem/allocator.h>
#include <revolution/wpad.h>

#include <context_rvl.h>

/*******************************************************************************
 * macros
 */

#define HBM_MEM_SIZE_SOUND	0x18700	/* name known from asserts */

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

// [SC5PGN]/build/libs/Debug/slamWiiD.a:HBMCommon.o(1)::.debug_info::0x12c4 [original object]
typedef enum HBMSelectBtnNum
{
	HBM_SELECT_NULL = -1,

	HBM_SELECT_HOMEBTN,
	HBM_SELECT_BTN1,
	HBM_SELECT_BTN2,
	HBM_SELECT_BTN3,
	HBM_SELECT_BTN4,

	HBM_SELECT_MAX
} HBMSelectBtnNum;

typedef int HBMSoundEvent;
enum HBMSoundEvent_et
{
	HBM_SOUND_EVENT_0, // StartInitSound? and then num would not matter
	HBM_SOUND_EVENT_1, // EndInitSound? and then num would not matter
	HBM_SOUND_EVENT_2, // Fadeout, num = ms? see calc_fadeoutAnm
	HBM_SOUND_EVENT_3, // Blackout, num = ms? from various
	HBM_SOUND_EVENT_4, // ShutdownSound? and then num would not matter
	HBM_SOUND_EVENT_PLAY,	/* num = HBMSound_et (see below) */
};

enum HBMSound_et
{
	HBM_SOUND_HOME_BUTTON,
	HBM_SOUND_RETURN_APP,
	HBM_SOUND_GOTO_MENU,
	HBM_SOUND_RESET_APP,
	HBM_SOUND_FOCUS,
	HBM_SOUND_SELECT,
	HBM_SOUND_CANCEL,
	HBM_SOUND_OPEN_CONTROLLER,
	HBM_SOUND_CLOSE_CONTROLLER,
	HBM_SOUND_VOLUME_PLUS,
	HBM_SOUND_VOLUME_MINUS,
	HBM_SOUND_VOLUME_PLUS_LIMIT,
	HBM_SOUND_VOLUME_MINUS_LIMIT,
	HBM_SOUND_NOTHING_DONE,
	HBM_SOUND_VIBE_ON,
	HBM_SOUND_VIBE_OFF,
	HBM_SOUND_START_CONNECT_WINDOW,
	HBM_SOUND_CONNECTED1,
	HBM_SOUND_CONNECTED2,
	HBM_SOUND_CONNECTED3,
	HBM_SOUND_CONNECTED4,
	HBM_SOUND_END_CONNECT_WINDOW,
	HBM_SOUND_MANUAL_OPEN,
	HBM_SOUND_MANUAL_FOCUS,
	HBM_SOUND_MANUAL_SELECT,
	HBM_SOUND_MANUAL_SCROLL,
	HBM_SOUND_MANUAL_CANCEL,
	HBM_SOUND_MANUAL_RETURN_APP
};

// [SC5PGN]/build/libs/Debug/slamWiiD.a:HBMCommon.o(1)::.debug_info::0x485 [original object]
typedef BOOL HBMSoundCallback(HBMSoundEvent evt, int num);

// [SC5PGN]/build/libs/Debug/slamWiiD.a:HBMCommon.o(1)::.debug_info::0x32b [original object]
typedef struct HBMDataInfo
{
	void				*layoutBuf;			// size 0x04, offset 0x00
	void				*spkSeBuf;			// size 0x04, offset 0x04
	void				*msgBuf;			// size 0x04, offset 0x08
	void				*configBuf;			// size 0x04, offset 0x0c
	void				*mem;				// size 0x04, offset 0x10
	HBMSoundCallback	*sound_callback;	// size 0x04, offset 0x14
	BOOL				backFlag;			// size 0x04, offset 0x18
	int					region;				// size 0x04, offset 0x1c
	BOOL				cursor;				// size 0x04, offset 0x20
	int					messageFlag;		// size 0x04, offset 0x24
	u32					configBufSize;		// size 0x04, offset 0x28
	u32					memSize;			// size 0x04, offset 0x2c
	f32					frameDelta;			// size 0x04, offset 0x30
	Vec2				adjust;				// size 0x08, offset 0x34
	MEMAllocator		*pAllocator;		// size 0x04, offset 0x3c
} HBMDataInfo; // size 0x40

typedef void HBMDataInfoEx;

// [SC5PGN]/build/libs/Debug/slamWiiD.a:HBMCommon.o(1)::.debug_info::0x317d [original object]
typedef struct HBMKPadData
{
	KPADStatus	*kpad;			// size 0x04, offset 0x00
	Vec2		pos;			// size 0x08, offset 0x04
	u32			use_devtype;	// size 0x04, offset 0x0c
} HBMKPadData; // size 0x10

// [SC5PGN]/build/libs/Debug/slamWiiD.a:HBMCommon.o(1)::.debug_info::0x3144 [original object]
typedef struct HBMControllerData
{
	HBMKPadData	wiiCon[WPAD_MAX_CONTROLLERS];	// size 0x40, offset 0x00
} HBMControllerData; // size 0x40

/*******************************************************************************
 * functions
 */

void HBMCreate(HBMDataInfo const *pHBInfo);
void HBMDelete(void);
void HBMInit(void);
HBMSelectBtnNum HBMCalc(HBMControllerData const *pController);
void HBMDraw(void);
HBMSelectBtnNum HBMGetSelectBtnNum(void);
void HBMSetAdjustFlag(BOOL flag);
void HBMStartBlackOut(void);
BOOL HBMIsReassignedControllers(void);
void HBMSetBlackOutColor(u8 red, u8 green, u8 blue);
GXColor HBMGetBlackOutColor(void);
void HBMSetVIBlack(BOOL flag);
void HBMCreateSound(void *soundData, void *memBuf, u32 memSize);
void HBMDeleteSound(void);
void HBMUpdateSound(void);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_HBM_H
