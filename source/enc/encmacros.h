#ifndef RVL_SDK_ENC_MACROS_H
#define RVL_SDK_ENC_MACROS_H

// TODO: maybe this can go in encutility.h?

// Generic, most-purpose

#ifndef SWALLOW_SEMICOLON
# define SWALLOW_SEMICOLON()	do { /* ... */ } while (0)
#endif

// WARNING: No double-evaluation protection
#define SWAP_ENDIAN_16(x)		(((x) >> 8) & 0x00ff) | (((x) << 8) & 0xff00)

// WARNING: Unsafe for use within single-statement clauses
// NOTE: Does not swallow semicolon. You don't need to feed it one
#define CREATE_STATE_VARIABLES(dstCnt_, dstLimit_, dstValid_, srcCnt_,	\
							   srcLimit_, srcLimited_)					\
	unk_t signed (srcCnt_) = 0;											\
	unk_t signed (dstCnt_) = 0;											\
																		\
	unk_t signed (srcLimit_) = -1;										\
	unk_t signed (dstLimit_) = -1;										\
																		\
	BOOL (dstValid_) = true;											\
	BOOL (srcLimited_) = true;											\

#define CHECK_PARAMETERS(dstStream_, dstSize_, dstLimit_, dstValid_,			\
						 srcStream_, srcSize_, srcLimit_, srcLimited_, ret_)	\
	do																			\
	{																			\
		(ret_) = ENCiCheckParameters(											\
			(dstStream_) != nullptr, (dstSize_), &(dstLimit_), &(dstValid_),	\
			(srcStream_) != nullptr, (srcSize_), &(srcLimit_), &(srcLimited_));	\
																				\
		if ((ret_) != ENC_ESUCCESS)												\
			return (ret_);														\
	} while (0)

#define COLLECT_MB_STATE(curState_, state_)	\
	do										\
	{										\
		if ((state_))						\
			(curState_) = *(state_);		\
	} while (0)

// WARNING: Unsafe for use within loop constructs, single-statement clauses
#define THROW_AND_QUIT(ret_, err_)	\
	{								\
		(ret_) = (err_);			\
		break;						\
	}								\
									\
	SWALLOW_SEMICOLON()

// WARNING: Unsafe for use within loop constructs, single-statement clauses
#define CHECK_DST_SPACE(cnt_, limit_, valid_, ret_)	\
	if ((cnt_) >= (limit_) && (valid_))				\
	{												\
		/* unsafe for single-statement clause */	\
													\
		THROW_AND_QUIT(ret_, ENC_ENOSPC);			\
	}												\
													\
	SWALLOW_SEMICOLON()

// WARNING: Unsafe for use within loop constructs, single-statement clauses
#define CHECK_BREAK_TYPE(dstStream_, dstCnt_, dstLimit_, dstValid_,				\
						 srcStream_, srcCnt_, srcLimit_, srcLimited_,			\
						 breakType_, ret_)										\
	if ((breakType_) > ENC_BREAK_TYPE_NONE)										\
	{																			\
		int srcBrkLen = ENCiCheckBreakType(										\
			(srcStream_)[0], (srcLimit_) - (srcCnt_) > 1 || !(srcLimited_)		\
								 ? (srcStream_)[1]								\
								 : 0);											\
																				\
		if (srcBrkLen > 0)														\
		{																		\
			/* unsafe for single-statement clause */							\
																				\
			CHECK_DST_BREAK_TYPE(dstStream_, dstCnt_, dstLimit_, dstValid_,		\
								 srcStream_, srcCnt_, breakType_, ret_);		\
		}																		\
	}																			\
																				\
	SWALLOW_SEMICOLON()

// WARNING: Unsafe for use within loop constructs, single-statement clauses
#define CHECK_DST_BREAK_TYPE(dstStream_, dstCnt_, dstLimit_, dstValid_,	\
							 srcStream_, srcCnt_, breakType_, ret_)		\
	{ /* limit scope of dstBrkLen */									\
		int dstBrkLen = ENCiWriteBreakType(								\
			dstStream_, sizeof *(dstStream_), breakType_, dstValid_);	\
																		\
		if ((dstLimit_) - (dstCnt_) < dstBrkLen && (dstValid_))			\
		{																\
			/* unsafe for single-statement clause */					\
																		\
			THROW_AND_QUIT(ret_, ENC_ENOSPC);							\
		}																\
																		\
		(srcStream_) += srcBrkLen;										\
		(srcCnt_) += srcBrkLen;											\
		(dstCnt_) += dstBrkLen;											\
																		\
		if ((dstValid_))												\
			(dstStream_) += dstBrkLen;									\
																		\
		continue;														\
	}																	\
																		\
	SWALLOW_SEMICOLON()

#define COPY_CHAR_AS_IS(char_, dstStream_, dstCnt_, dstValid_, srcStream_,	\
						srcCnt_)											\
	do																		\
	{																		\
		if (dstValid_)														\
		{																	\
			*(dstStream_) = (char_);										\
			++(dstStream_);													\
		}																	\
																			\
		++(dstCnt_);														\
		++(srcStream_);														\
		++(srcCnt_);														\
	} while (0)

#define INCREMENT_STREAMS(dstStream_, dstCnt_, dstValid_, srcStream_, srcCnt_)	\
	do																			\
	{																			\
		if (dstValid_)															\
		{																		\
			/* *(dstStream_) = (char_); */ /* Doesn't happen here */			\
			++(dstStream_);														\
		}																		\
																				\
		++(dstCnt_);															\
		++(srcStream_);															\
		++(srcCnt_);															\
	} while (0)

#define WRITE_BACK_STATE(state_, curState_)	\
	do										\
	{										\
		if (state_)							\
			*(state_) = (curState_);		\
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

// ASCII

#define IS_ASCII(x)						((x) < 0x80)

// only exists because > 0x7f != !(< 0x80)
#define IS_OUTSIDE_ASCII(x)				((x) > 0x7F)

// Unicode

#define UTF_IS_DEFINED_CODEPOINT(x)		((x) <= 0x10FFFF)
#define UTF_IS_2_BYTES_IN_UTF8(x)		((x) <    0x0800)
#define UTF_IS_3_BYTES_IN_UTF8(x)		((x) <=   0xFFFF)

// UTF-8

#define UTF8_BOM_LEN					3
#define UTF8_BOM_0						0xEF
#define UTF8_BOM_1						0xBB
#define UTF8_BOM_2						0xBF

#define UTF8_IS_CONT(x)					((unsigned)((x) & 0xC0) == 0x80)
#define UTF8_IS_INITIAL_2(x)			((unsigned)((x) & 0xE0) == 0xC0)
#define UTF8_IS_INITIAL_3(x)			((unsigned)((x) & 0xF0) == 0xE0)
#define UTF8_IS_INITIAL_4(x)			((unsigned)((x) & 0xF8) == 0xF0)

#define UTF8_GET_INITIAL_1(x)			 (x)
#define UTF8_GET_INITIAL_2(x)			((x) & 0x1F)
#define UTF8_GET_INITIAL_3(x)			((x) & 0x0F)
#define UTF8_GET_INITIAL_4(x)			((x) & 0x07)
#define UTF8_GET_CONT(x)				((x) & 0x3F)

#define UTF8_MAKE_INITIAL_1(x)			        (x)
#define UTF8_MAKE_INITIAL_2(x)			(0xC0 + (x))
#define UTF8_MAKE_INITIAL_3(x)			(0xE0 + (x))
#define UTF8_MAKE_INITIAL_4(x)			(0xF0 + (x))
#define UTF8_MAKE_CONT(x)				(0x80 + (x))

#define UTF8_CHECK_BOM(stream_, limit_, limited_, cnt_)	\
	do if ((limit_) >= UTF8_BOM_LEN || !(limited_))		\
	{													\
		char8_t const *bom = (stream_);					\
														\
		if (*bom == UTF8_BOM_0)							\
		{												\
			++bom;										\
														\
			if (*bom == UTF8_BOM_1)						\
			{											\
														\
				++bom;									\
														\
				if (*bom == UTF8_BOM_2)					\
				{										\
					(stream_) = bom + 1;				\
					(cnt_) += UTF8_BOM_LEN;				\
				}										\
			}											\
		}												\
	} while (0)

// UTF-16

#define UTF16_BOM_LEN					1
#define UTF16_BE_BOM					0xFEFF
#define UTF16_LE_BOM					0xFFFE

#define UTF16_IS_SURROGATE(x)			((unsigned)((x) & 0xF800) == 0xD800)

#define UTF16_GET_HIGH_SURROGATE(x)		(((x) - 0xD7C0) << 10)
#define UTF16_GET_LOW_SURROGATE(x)		( (x) & 0x03EF       )

#define UTF16_MAKE_HIGH_SURROGATE(x)	(0xD7C0 + (x))
#define UTF16_MAKE_LOW_SURROGATE(x)		(0xDC00 + (x))

#define UTF16_CHECK_BOM(stream_, srcSize_, limit_, limited_, cnt_, dstSize_)	\
	do if ((limit_) > 0 || !(limited_))											\
	{																			\
		if (*(stream_) == UTF16_BE_BOM)											\
		{																		\
			++(cnt_);															\
			++(stream_);														\
		}																		\
		else if (*(stream_) == UTF16_LE_BOM)									\
		{																		\
			if (dstSize_)														\
				*(dstSize_) = 0;												\
																				\
			*(srcSize_) = 0;													\
			return ENC_EILSEQ;													\
		}																		\
	} while (0)

// UTF-32

#define UTF32_BOM_LEN					1
#define UTF32_BE_BOM					0x0000FEFF
#define UTF32_LE_BOM					0xFFFE0000

#define UTF32_CHECK_BOM(stream_, srcSize_, limit_, limited_, cnt_, dstSize_)	\
	do if ((limit_) > 0 || !(limited_))											\
	{																			\
		if (*(stream_) == UTF32_BE_BOM)											\
		{																		\
			++(cnt_);															\
			++(stream_);														\
		}																		\
		else if (*(stream_) == UTF32_LE_BOM)									\
		{																		\
			if (dstSize_)														\
				*(dstSize_) = 0;												\
																				\
			*(srcSize_) = 0;													\
			return ENC_EILSEQ;													\
		}																		\
	} while (0)

// JIS/Shift_JIS

// WARNING: Unsafe for use within loop constructs, single-statement clauses
#define JIS_CHECK_BREAK_TYPE(dstStream_, dstCnt_, dstLimit_, dstValid_,			\
						 	srcStream_, srcCnt_, srcLimit_, srcLimited_,		\
						 	breakType_, state_, ret_)							\
	if ((breakType_) > ENC_BREAK_TYPE_NONE)										\
	{																			\
		int srcBrkLen = ENCiCheckBreakType(										\
			(srcStream_)[0], (srcLimit_) - (srcCnt_) > 1 || !(srcLimited_)		\
								 ? (srcStream_)[1]								\
								 : 0);											\
																				\
		if (srcBrkLen > 0)														\
		{																		\
			/* unsafe for single-statement clause */							\
																				\
			JIS_CHECK_DST_BREAK_TYPE(dstStream_, dstCnt_, dstLimit_, dstValid_,	\
				srcStream_, srcCnt_, breakType_, state_, ret_);					\
		}																		\
	}																			\
																				\
	SWALLOW_SEMICOLON()

// WARNING: Unsafe for use within loop constructs, single-statement clauses
#define JIS_CHECK_DST_BREAK_TYPE(dstStream_, dstCnt_, dstLimit_, dstValid_,		\
								 srcStream_, srcCnt_, breakType_, state_, ret_)	\
	{ /* limit scope of dstBrkLen */											\
		int dstBrkLen = ENCiWriteBreakType(										\
			dstStream_, sizeof *(dstStream_), breakType_, dstValid_);			\
																				\
		if ((dstLimit_) - (dstCnt_) < dstBrkLen && (dstValid_))					\
		{																		\
			/* unsafe for single-statement clause */							\
																				\
			THROW_AND_QUIT(ret_, ENC_ENOSPC);									\
		}																		\
																				\
		/* Notably, there is a state change here. */							\
		(state_) = ENC_JIS_STATE_ASCII;											\
																				\
		(srcStream_) += srcBrkLen;												\
		(srcCnt_) += srcBrkLen;													\
		(dstCnt_) += dstBrkLen;													\
																				\
		if ((dstValid_))														\
			(dstStream_) += dstBrkLen;											\
																				\
		continue;																\
	}																			\
																				\
	SWALLOW_SEMICOLON()

// WARNING: Unsafe for use within loop constructs, single-statement clauses
#define JIS_CHECK_ESCAPE_SEQUENCE(state_, srcStream_, srcCnt_, srcLimit_,	\
								  srcLimited_, escLen_)						\
	{ /* this block doesn't re-make it safe */								\
		(escLen_) = ENCiGetEscapeSequence(&(state_), (srcStream_),			\
			(srcLimit_) - (srcCnt_), (srcLimited_));						\
																			\
		if ((escLen_) < 0)													\
			break;															\
																			\
		if ((escLen_) > 0)													\
		{																	\
			(srcStream_) += (escLen_);										\
			(srcCnt_) += (escLen_);											\
			continue;														\
		}																	\
	}

#define JIS_WRITE_CHAR(mb_, dstStream_, dstCnt_, dstValid_, srcStream_,	\
					   srcCnt_)											\
	do																	\
	{																	\
		++(srcStream_);													\
		++(srcCnt_);													\
		++(dstCnt_);													\
																		\
		if (dstValid_)													\
		{																\
			*(dstStream_) = (mb_)[0];									\
			++(dstStream_);												\
		}																\
																		\
		if ((mb_)[1] != 0x00)											\
		{																\
			++(dstCnt_);												\
																		\
			if (dstValid_)												\
			{															\
				*(dstStream_) = (mb_)[1];								\
				++(dstStream_);											\
			}															\
		}																\
	} while (0)

#define JIS_SAVE_STATE(curState_, prevState_, dstStream_, dstCnt_, dstLimit_,	\
					   dstValid_, srcCnt_, ret_)								\
	do																			\
	{																			\
		if ((ret_) != ENC_ESUCCESS)												\
			(curState_) = (prevState_);											\
																				\
		if ((curState_) != ENC_JIS_STATE_ASCII && (srcCnt_) > 0)				\
		{																		\
			(curState_) = ENC_JIS_STATE_ASCII;									\
																				\
			if ((dstValid_) && (dstLimit_) - (dstCnt_) >= 3)					\
			{																	\
				*(dstStream_)++ = 0x1B;											\
				*(dstStream_)++ = 0x28;											\
				*(dstStream_)++ = 0x42;											\
			}																	\
																				\
			if (!(dstValid_) || (dstLimit_) - (dstCnt_) >= 3)					\
				(dstCnt_) += 3;													\
		}																		\
	} while (0)

#endif // RVL_SDK_ENC_MACROS_H
