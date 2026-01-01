#ifndef RVL_SDK_ENC_MACROS_H
#define RVL_SDK_ENC_MACROS_H

/*******************************************************************************
 * headers
 */

#include <macros.h> // SWALLOW_SEMICOLON

/*******************************************************************************
 * macros
 */

// TODO: maybe all of this can go in encutility.h?

// Internal declarations

// NOLINTBEGIN(bugprone-macro-parentheses) (Type specifiers cannot be parenthesized.)
#define DECLARE_PRIVATE_ENC_FUNCTION(srcEnc_, srcChar_, dstEnc_, dstChar_)	\
	ENCResult ENCiConvertString ## srcEnc_ ## To ## dstEnc_(				\
		dstChar_ *dst, s32 *dstSize, srcChar_ const *src, s32 *srcSize,		\
		ENCBreakType breakType);

#define DECLARE_PRIVATE_ENC_FUNCTION_WITH_MB_STATE(srcEnc_, srcChar_, dstEnc_,	\
												   dstChar_)					\
	ENCResult ENCiConvertString ## srcEnc_ ## To ## dstEnc_(					\
		dstChar_ *dst, s32 *dstSize, srcChar_ const *src, s32 *srcSize,			\
		ENCBreakType breakType, ENCMBState *mbState);
// NOLINTEND(bugprone-macro-parentheses)

#define DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16(srcEnc_)	\
	DECLARE_PRIVATE_ENC_FUNCTION(srcEnc_, byte_t, Unicode, char16_t)
#define DECLARE_PRIVATE_ENC_FUNCTION_FROM_UTF16(dstEnc_)	\
	DECLARE_PRIVATE_ENC_FUNCTION(Unicode, char16_t, dstEnc_, byte_t)

#define DECLARE_PRIVATE_ENC_FUNCTION_TO_UTF16_WITH_MB_STATE(srcEnc_)		\
	DECLARE_PRIVATE_ENC_FUNCTION_WITH_MB_STATE(srcEnc_, byte_t, Unicode,	\
											   char16_t)
#define DECLARE_PRIVATE_ENC_FUNCTION_FROM_UTF16_WITH_MB_STATE(dstEnc_)		\
	DECLARE_PRIVATE_ENC_FUNCTION_WITH_MB_STATE(Unicode, char16_t, dstEnc_,	\
											   byte_t)

// Trampolines

// NOLINTBEGIN(bugprone-macro-parentheses)
#define DEFINE_PUBLIC_ENC_TRAMPOLINE_VIA(srcEnc_, srcChar_, srcVia_, dstEnc_,	\
										 dstChar_, dstVia_)						\
	ENCResult ENCConvertString ## srcEnc_ ## To ## dstEnc_(						\
		dstChar_ *dst, s32 *dstSize, srcChar_ const *src, s32 *srcSize)			\
	{																			\
		return ENCiConvertString ## srcVia_ ## To ## dstVia_(					\
			dst, dstSize, src, srcSize, ENC_BR_KEEP);							\
	}

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE_VIA(					\
	srcEnc_, srcChar_, srcVia_, dstEnc_, dstChar_, dstVia_)				\
	ENCResult ENCConvertString ## srcEnc_ ## To ## dstEnc_(				\
		dstChar_ *dst, s32 *dstSize, srcChar_ const *src, s32 *srcSize)	\
	{																	\
		return ENCiConvertString ## srcVia_ ## To ## dstVia_(			\
			dst, dstSize, src, srcSize, ENC_BR_KEEP, nullptr);			\
	}

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_GIVEN_MB_STATE_VIA(			\
	srcEnc_, srcChar_, srcVia_, dstEnc_, dstChar_, dstVia_, mbState_)	\
	ENCResult ENCConvertString ## srcEnc_ ## To ## dstEnc_(				\
		dstChar_ *dst, s32 *dstSize, srcChar_ const *src, s32 *srcSize)	\
	{																	\
		ENCMBState mbState = (mbState_);								\
																		\
		return ENCiConvertString ## srcVia_ ## To ## dstVia_(			\
			dst, dstSize, src, srcSize, ENC_BR_KEEP, &mbState);			\
	}
// NOLINTEND(bugprone-macro-parentheses)

#define DEFINE_PUBLIC_ENC_TRAMPOLINE(srcEnc_, srcChar_, dstEnc_, dstChar_)	\
	DEFINE_PUBLIC_ENC_TRAMPOLINE_VIA(srcEnc_, srcChar_, srcEnc_, dstEnc_,	\
									 dstChar_, dstEnc_)

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE(srcEnc_, srcChar_,	\
												   dstEnc_, dstChar_)	\
	DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE_VIA(srcEnc_, srcChar_,	\
												   srcEnc_, dstEnc_,	\
												   dstChar_, dstEnc_)

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_GIVEN_MB_STATE(srcEnc_, srcChar_,	\
														 dstEnc_, dstChar_,	\
														 mbState_)			\
	DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_GIVEN_MB_STATE_VIA(srcEnc_, srcChar_,	\
														 srcEnc_, dstEnc_,	\
														 dstChar_, dstEnc_,	\
														 mbState_)

// #define DEFINE_PUBLIC_ENC_TRAMPOLINE_FROM_UTF16_VIA(dstEnc_, dstVia_)	\
	DEFINE_PUBLIC_ENC_TRAMPOLINE_VIA(Unicode, char16_t, Unicode, dstEnc_,	\
									 byte_t, dstVia_)

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_FROM_UTF16(dstEnc_)					\
	DEFINE_PUBLIC_ENC_TRAMPOLINE_VIA(Unicode, char16_t, Unicode, dstEnc_,	\
									 byte_t, dstEnc_)

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_TO_UTF16_VIA(srcEnc_, srcVia_)		\
	DEFINE_PUBLIC_ENC_TRAMPOLINE_VIA(srcEnc_, byte_t, srcVia_, Unicode,	\
									 char16_t, Unicode)

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_TO_UTF16(srcEnc_)					\
	DEFINE_PUBLIC_ENC_TRAMPOLINE_VIA(srcEnc_, byte_t, srcEnc_, Unicode,	\
									 char16_t, Unicode)

// #define DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE_FROM_UTF16_VIA(dstEnc_,	\
																  dstVia_)		\
	DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE_VIA(Unicode, char16_t, Unicode,	\
												   dstEnc_, byte_t, dstVia_)

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE_FROM_UTF16(dstEnc_)			\
	DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE_VIA(Unicode, char16_t, Unicode,	\
												   dstEnc_, byte_t, dstEnc_)

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE_TO_UTF16_VIA(srcEnc_,		\
																srcVia_)		\
	DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE_VIA(srcEnc_, byte_t, srcVia_,	\
												   Unicode, char16_t, Unicode)

#define DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE_TO_UTF16(srcEnc_)			\
	DEFINE_PUBLIC_ENC_TRAMPOLINE_WITH_MB_STATE_VIA(srcEnc_, byte_t, srcEnc_,	\
												   Unicode, char16_t, Unicode)

// General

// NOTE: Unsafe for use within single-statement clauses
/* But it shouldn't really be there anyways. It should only be at the beginning
 * of a conversion function to declare some variables
 */
// NOTE: Does not swallow semicolon. Do not feed it one
// NOTE: Not to be confused with multibyte state, which is different.
#define CREATE_STATE_VARIABLES(dstCnt_, dstLimit_, dstValid_, srcCnt_,	\
							   srcLimit_, srcLimited_)					\
	s32 (srcCnt_) = 0;													\
	s32 (dstCnt_) = 0;													\
																		\
	s32 (srcLimit_) = ENC_SRC_UNLIMITED;								\
	s32 (dstLimit_) = ENC_DST_INVALID;									\
																		\
	BOOL (dstValid_) = true;											\
	BOOL (srcLimited_) = true;											\

#define CHECK_PARAMETERS(ret_, dstStream_, dstSize_, dstLimit_, dstValid_,		\
						 srcStream_, srcSize_, srcLimit_, srcLimited_)			\
	do																			\
	{																			\
		*(ret_) = ENCiCheckParameters(											\
			(dstStream_) != nullptr, dstSize_, dstLimit_, dstValid_,			\
			(srcStream_) != nullptr, srcSize_, srcLimit_, srcLimited_);			\
																				\
		if (*(ret_) != ENC_ESUCCESS)											\
			return *(ret_);														\
	} while (0)

// TODO: better name?
// WARNING: Unsafe for use within nested loop constructs, single-statement clauses
#define UNSAFE_THROW_AND_QUIT(ret_, err_)	\
	{										\
		*(ret_) = (err_);					\
		break;								\
	}										\
											\
	SWALLOW_SEMICOLON()

// WARNING: Unsafe for use within nested loop constructs, single-statement clauses
#define UNSAFE_CHECK_DST_SPACE(cnt_, limit_, valid_, ret_)	\
	if ((cnt_) >= (limit_) && (valid_))						\
	{														\
		UNSAFE_THROW_AND_QUIT(ret_, ENC_ENOBUFS);			\
	}														\
															\
	SWALLOW_SEMICOLON()

// WARNING: Unsafe for use within nested loop constructs, single-statement clauses
#define UNSAFE_CHECK_BREAK_TYPE(dstStream_, dstCnt_, dstLimit_, dstValid_,		\
								srcStream_, srcCnt_, srcLimit_, srcLimited_,	\
								breakType_, ret_)								\
	if ((breakType_) > ENC_BR_KEEP)												\
	{																			\
		int srcBrkLen = ENCiCheckBreakType(										\
			(*(srcStream_))[0], (srcLimit_) - *(srcCnt_) > 1 || !(srcLimited_)	\
								 ? (*(srcStream_))[1]							\
								 : 0x00);										\
																				\
		if (srcBrkLen > 0)														\
		{																		\
			UNSAFE_CHECK_DST_BREAK_TYPE(dstStream_, dstCnt_, dstLimit_,			\
										dstValid_, srcStream_, srcCnt_,			\
										breakType_, ret_);						\
		}																		\
	}																			\
																				\
	SWALLOW_SEMICOLON()

// WARNING: Unsafe for use within nested loop constructs, single-statement clauses
#define UNSAFE_CHECK_DST_BREAK_TYPE(dstStream_, dstCnt_, dstLimit_, dstValid_,	\
									srcStream_, srcCnt_, breakType_, ret_)		\
	{ /* limit scope of dstBrkLen */											\
		int dstBrkLen = ENCiWriteBreakType(										\
			*(dstStream_), sizeof **(dstStream_), breakType_, dstValid_);		\
																				\
		if ((dstLimit_) - *(dstCnt_) < dstBrkLen && (dstValid_))				\
		{																		\
			UNSAFE_THROW_AND_QUIT(ret_, ENC_ENOBUFS);							\
		}																		\
																				\
		*(srcStream_) += srcBrkLen;												\
		*(srcCnt_) += srcBrkLen;												\
		*(dstCnt_) += dstBrkLen;												\
																				\
		if (dstValid_)															\
			*(dstStream_) += dstBrkLen;											\
																				\
		continue;																\
	}																			\
																				\
	SWALLOW_SEMICOLON()

#define WRITE_CHAR(char_, dstStream_, dstCnt_, dstValid_, srcStream_, srcCnt_)	\
	do																			\
	{																			\
		if (dstValid_)															\
		{																		\
			**(dstStream_) = (char_);											\
			++*(dstStream_);													\
		}																		\
																				\
		++*(dstCnt_);															\
		++*(srcStream_);														\
		++*(srcCnt_);															\
	} while (0)

#define INCREMENT_STREAMS(dstStream_, dstCnt_, dstValid_, srcStream_, srcCnt_)	\
	do																			\
	{																			\
		if (dstValid_)															\
		{																		\
			/* *(dstStream_) = (char_); */ /* Doesn't happen here */			\
			++*(dstStream_);													\
		}																		\
																				\
		++*(dstCnt_);															\
		++*(srcStream_);														\
		++*(srcCnt_);															\
	} while (0)

#define WRITE_BACK_SIZES(srcSize_, srcCnt_, dstSize_, dstCnt_)	\
	do															\
	{															\
		if (srcSize_)											\
			*(srcSize_) = (srcCnt_);							\
																\
		if (dstSize_)											\
			*(dstSize_) = (dstCnt_);							\
	} while (0)

// WARNING: Unsafe for use within nested loop constructs, single-statement clauses
#define UNSAFE_WRITE_MULTI_BYTE_CHAR(mb_, dstStream_, dstCnt_, dstLimit_,	\
									 dstValid_, srcStream_, srcCnt_, ret_)	\
	{ /* limit scope of b1 and b2 */										\
		byte_t b1 = (mb_)[0];												\
		byte_t b2 = (mb_)[1];												\
																			\
		if (!b1)															\
		{																	\
			UNSAFE_THROW_AND_QUIT(ret_, ENC_ERANGE);						\
		}																	\
																			\
		if (!b2)															\
		{																	\
			if (dstValid_)													\
			{																\
				/* NOTE: Doubled expressions */								\
				**(dstStream_) = b1; ++*(dstStream_);						\
			}																\
		}																	\
		else																\
		{																	\
			if (dstValid_)													\
			{																\
				if ((dstLimit_) - *(dstCnt_) < 2)							\
				{															\
					UNSAFE_THROW_AND_QUIT(ret_, ENC_ENOBUFS);				\
				}															\
																			\
				/* NOTE: Doubled expressions */								\
				**(dstStream_) = b1; ++*(dstStream_);						\
				**(dstStream_) = b2; ++*(dstStream_);						\
																			\
				++*(dstCnt_);												\
			}																\
			else															\
			{																\
				++*(dstCnt_);												\
			}																\
		}																	\
																			\
		++*(dstCnt_);														\
		++*(srcStream_);													\
		++*(srcCnt_);														\
	}																		\
																			\
	SWALLOW_SEMICOLON()

// ASCII

#define IS_ASCII(x)		((x) < 0x80)

// UTF-16

#define UTF16_BE_BOM	0xFEFF
#define UTF16_LE_BOM	0xFFFE

#define UTF16_CHECK_BOM(stream_, srcSize_, limit_, limited_, cnt_, dstSize_)	\
	do																			\
	{																			\
		if ((limit_) > 0 || !(limited_))										\
		{																		\
			if (**(stream_) == UTF16_BE_BOM)									\
			{																	\
				++*(cnt_);														\
				++*(stream_);													\
			}																	\
			else if (**(stream_) == UTF16_LE_BOM)								\
			{																	\
				if (dstSize_)													\
					*(dstSize_) = 0;											\
																				\
				*(srcSize_) = 0;												\
				return ENC_EILSEQ;												\
			}																	\
		}																		\
	} while (0)

#endif // RVL_SDK_ENC_MACROS_H
