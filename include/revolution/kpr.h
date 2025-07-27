#ifndef RVL_SDK_KPR_H
#define RVL_SDK_KPR_H

/*******************************************************************************
 * headers
 */

#include <wchar.h>

#include <decomp.h>

#include <revolution/types.h>

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef int KPRMode;
enum KPRMode_et
{
	KPR_MODE_DEADKEY			= 1 << 1,
	KPR_MODE_JP_ROMAJI_HIRAGANA	= 1 << 2,
	KPR_MODE_JP_ROMAJI_KATAKANA	= 1 << 3,
};

/* struct tag known from symbols */
typedef struct _KPRQueue
{
	wchar_t			buffer[6];	// size 0x0c, offset 0x00
	KPRMode			mode;		// size 0x04, offset 0x0c
	u8				count;		// size 0x00, offset 0x10
	u8				iCount;		// size 0x01, offset 0x11	/* name known from asserts */
	/* 2 bytes padding */
	unk4_t unsigned	at_0x14;	// size 0x04, offset 0x14
} KPRQueue; // size 0x18?

/*******************************************************************************
 * functions
 */

void KPRInitRegionUS(void);
void KPRInitRegionJP(void);
void KPRInitRegionEU(void);

void KPRInitQueue(KPRQueue *queue);
void KPRClearQueue(KPRQueue *queue);

void KPRSetMode(KPRQueue *queue, KPRMode mode);
KPRMode KPRGetMode(KPRQueue *queue);

u32 KPRPutChar(KPRQueue *queue, wchar_t ch);
wchar_t KPRGetChar(KPRQueue *queue);
wchar_t KPRRemoveChar(KPRQueue *queue);
u8 KPRLookAhead(KPRQueue *queue, wchar_t *buffer, u32 count);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_KPR_H
