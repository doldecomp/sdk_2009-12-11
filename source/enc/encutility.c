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

unk_t ENCiCheckParameters(unk_t param_1, unk4_t signed *param_2,
                          unk4_t signed *param_3, unk4_t *param_4,
                          unk_t param_5, unk4_t signed *param_6,
                          unk4_t signed *param_7, unk4_t *param_8)
{
	unk_t signed ret = 0;

	ENCiRegisterVersion();

	if (param_6)
		*param_7 = *param_6;
	else
		*param_7 = -1;

	if (param_2)
	{
		*param_3 = *param_2;
	}
	else
	{
		*param_3 = -1;

		ret = -3;
	}

	if (param_5 == 0)
		ret = -3;

	if (param_1 == 0)
	{
		*param_4 = false;

		*param_3 = -1;
	}

	if (*param_7 < 0)
		*param_8 = false;

	if (ret != 0)
	{
		*param_2 = 0;

		*param_6 = 0;
	}

	return ret;
}

unk_t ENCiCheckBreakType(unk_t unsigned a, unk_t unsigned b)
{
	if (a == '\n')
	{
		return 1;
	}
	else if (a == '\r')
	{
		if (b == '\n')
			return 2;
		else
			return 1;
	}
	else
	{
		return 0;
	}
}

unk_t ENCiWriteBreakType(unk1_t unsigned *param_1, unk_t long param_2, // ?
                         ENCBreakType type, unk_t param_4)
{
	if (param_4 != 0)
	{
		memset(param_1, '\0', param_2 - 1);

		switch (type)
		{
		case ENC_BREAK_TYPE_WINDOWS:
			*(param_1 + param_2 - 1) = '\r';
			memset(param_1 + param_2, '\0', param_2 - 1);
			*(param_1 + param_2 * 2 - 1) = '\n';

			return 2;

		case ENC_BREAK_TYPE_CLASSIC_MAC_OS:
			*(param_1 + param_2 - 1) = '\r';

			return 1;

		case ENC_BREAK_TYPE_UNIX:
			*(param_1 + param_2 - 1) = '\n';

			return 1;
		}
	}
	else
	{
		switch (type)
		{
		case ENC_BREAK_TYPE_WINDOWS:
			return 2;

		case ENC_BREAK_TYPE_CLASSIC_MAC_OS:
			return 1;

		case ENC_BREAK_TYPE_UNIX:
			return 1;
		}
	}

	return 0;
}
