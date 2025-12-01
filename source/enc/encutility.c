#include "encutility.h"

/*******************************************************************************
 * headers
 */

#include <string.h>

#include <decomp.h>

#include <revolution/types.h>

#include "buildstamp.h"

// #include <revolution/os/OS.h>

#include <context_rvl.h>

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
		OSRegisterVersion(__ENCVersion);

		encRegistered = true;
	}
}

ENCResult ENCiCheckParameters(BOOL dstValid, unk_t signed *dstSizeIn,
                              unk_t signed *dstSizeOut, BOOL *dstValidOut,
                              BOOL srcValid, unk_t signed *srcSizeIn,
                              unk_t signed *srcSizeOut, BOOL *srcLimitedOut)
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
		return sizeof(char) * 1;
	}
	else if (a == '\r')
	{
		if (b == '\n')
			return sizeof(char) * 2;
		else
			return sizeof(char) * 1;
	}
	else
	{
		return sizeof(char) * 0;
	}
}

int ENCiWriteBreakType(void *stream, unk_t width, ENCBreakType breakType,
                       BOOL valid)
{
	if (valid)
	{
		memset(stream, '\0', width - 1);

		switch (breakType)
		{
		case ENC_BREAK_TYPE_WINDOWS:
			*((byte1_t *)stream + width - 1) = '\r';
			memset((byte1_t *)stream + width, '\0', width - 1);
			*((byte1_t *)stream + width * 2 - 1) = '\n';

			return sizeof(char) * 2;

		case ENC_BREAK_TYPE_CLASSIC_MAC_OS:
			*((byte1_t *)stream + width - 1) = '\r';

			return sizeof(char) * 1;

		case ENC_BREAK_TYPE_UNIX:
			*((byte1_t *)stream + width - 1) = '\n';

			return sizeof(char) * 1;
		}
	}
	else
	{
		switch (breakType)
		{
		case ENC_BREAK_TYPE_WINDOWS:
			return sizeof(char) * 2;

		case ENC_BREAK_TYPE_CLASSIC_MAC_OS:
			return sizeof(char) * 1;

		case ENC_BREAK_TYPE_UNIX:
			return sizeof(char) * 1;
		}
	}

	return sizeof(char) * 0;
}
