#include "encutility.h"

/*******************************************************************************
 * headers
 */

#include <string.h> // memset

#include <revolution/types.h>

#include "buildstamp.h"

// #include <revolution/os.h>

/*******************************************************************************
 * macros
 */

#define RVL_SDK_ENC_DEBUG_BUILD_DATE	"Dec 11 2009"
#define RVL_SDK_ENC_DEBUG_BUILD_TIME	"15:54:47"

#define RVL_SDK_ENC_RELEASE_BUILD_DATE	"Dec 11 2009"
#define RVL_SDK_ENC_RELEASE_BUILD_TIME	"15:59:26"

/*******************************************************************************
 * variables
 */

// .data, .sdata
char const *__ENCVersion = RVL_SDK_LIB_VERSION_STRING(ENC);

// .sbss
static BOOL encRegistered = false;

/*******************************************************************************
 * functions
 */

void ENCiRegisterVersion(void)
{
	if (!encRegistered)
	{
		/* CLEANUP: #include the <os.h> header whenever it's good to go (just
		 * doing this to remove <context_rvl.h> dependencies since this is the
		 * only thing it needs it for)
		 */
		extern void OSRegisterVersion(char const *version);

		OSRegisterVersion(__ENCVersion);

		encRegistered = true;
	}
}

ENCResult ENCiCheckParameters(BOOL dstValid, s32 *dstSizeIn, s32 *dstSizeOut,
                              BOOL *dstValidOut, BOOL srcValid, s32 *srcSizeIn,
                              s32 *srcSizeOut, BOOL *srcLimitedOut)
{
	ENCResult ret = ENC_ESUCCESS;

	ENCiRegisterVersion();

	if (srcSizeIn)
		*srcSizeOut = *srcSizeIn;
	else
		*srcSizeOut = ENC_SRC_UNLIMITED;

	if (dstSizeIn)
	{
		*dstSizeOut = *dstSizeIn;
	}
	else
	{
		*dstSizeOut = ENC_DST_INVALID;

		ret = ENC_EINVAL;
	}

	if (!srcValid)
		ret = ENC_EINVAL;

	if (!dstValid)
	{
		*dstValidOut = false;

		*dstSizeOut = ENC_DST_INVALID;
	}

	if (*srcSizeOut < 0) // ENC_SRC_UNLIMITED
		*srcLimitedOut = false;

	if (ret != ENC_ESUCCESS)
	{
		*dstSizeIn = 0;
		*srcSizeIn = 0;
	}

	return ret;
}

int ENCiCheckBreakType(unsigned a, unsigned b)
{
	if (a == '\n')
	{
		return sizeof "\n" - 1;
	}
	else if (a == '\r')
	{
		if (b == '\n')
			return sizeof "\n\r" - 1;
		else
			return sizeof "\r" - 1;
	}

	return sizeof "" - 1;
}

int ENCiWriteBreakType(void *stream, int width, ENCBreakType breakType,
                       BOOL valid)
{
	if (valid)
	{
		memset(stream, '\0', width - 1);

		switch (breakType)
		{
		case ENC_BR_CRLF:
			*((byte1_t *)stream + width - 1) = '\r';
			memset((byte1_t *)stream + width, '\0', width - 1);
			*((byte1_t *)stream + width * 2 - 1) = '\n';

			return sizeof "\r\n" - 1;

		case ENC_BR_CR:
			*((byte1_t *)stream + width - 1) = '\r';

			return sizeof "\r" - 1;

		case ENC_BR_LF:
			*((byte1_t *)stream + width - 1) = '\n';

			return sizeof "\n" - 1;
		}
	}
	else
	{
		switch (breakType)
		{
		case ENC_BR_CRLF:
			return sizeof "\r\n" - 1;

		case ENC_BR_CR:
			return sizeof "\r" - 1;

		case ENC_BR_LF:
			return sizeof "\n" - 1;
		}
	}

	return sizeof "" - 1;
}
