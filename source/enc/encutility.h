#ifndef RVL_SDK_ENC_UTILITY_H
#define RVL_SDK_ENC_UTILITY_H

/*******************************************************************************
 * headers
 */

#include <decomp.h>

#include <revolution/types.h>

#include <revolution/enc.h>

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef unk_t ENCBreakType;
enum ENCBreakType_et
{
	ENC_BREAK_TYPE_NONE,
	ENC_BREAK_TYPE_WINDOWS,
	ENC_BREAK_TYPE_CLASSIC_MAC_OS,
	ENC_BREAK_TYPE_UNIX,
	// Hey do you think anyone uses \n\r? Or maybe ^_? What about \u0085
};

/*******************************************************************************
 * functions
 */

void ENCiRegisterVersion(void);
unk_t ENCiCheckParameters(unk_t, unk4_t signed *, unk4_t signed *, unk4_t *,
                          unk_t, unk4_t signed *, unk4_t signed *, unk4_t *);
unk_t ENCiCheckBreakType(unk_t unsigned, unk_t unsigned);
unk_t ENCiWriteBreakType(unk1_t unsigned *, unk_t long, ENCBreakType type,
                         unk_t);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_ENC_UTILITY_H
