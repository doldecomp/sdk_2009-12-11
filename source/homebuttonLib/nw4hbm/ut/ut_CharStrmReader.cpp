#include "CharStrmReader.h"

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

/*******************************************************************************
 * local function declarations
 */

namespace nw4hbm { namespace ut { namespace
{
	inline bool IsSJISLeadByte(byte_t c)
	{
		return (0x81 <= c && c < 0xA0) || 0xE0 <= c;
	}
}}} // namespace nw4hbm::ut::(unnamed)

/*******************************************************************************
 * functions
 */

namespace nw4hbm { namespace ut {

char16_t CharStrmReader::ReadNextCharUTF8()
{
	byte2_t code;

	if ((GetChar<char8_t>(0) & 0x80) == 0x00)
	{
		// 1-byte UTF-8 sequence
		code = GetChar<char8_t>(0);
		StepStrm<char8_t>(1);
	}
	else if ((GetChar<char8_t>(0) & 0xE0) == 0xC0)
	{
		// 2-byte UTF-8 sequence
		code = (GetChar<char8_t>(0) & 0x1f) << 6 | (GetChar<char8_t>(1) & 0x3f);
		StepStrm<char8_t>(2);
	}
	else // if ((GetChar<char8_t>(0) & 0xF0) == 0xE0)
	{
		// 3-byte UTF-8 sequence

		// technical ERRATUM: the mask of GetChar<char8_t>(0) should be 0x0f
		/* Technical because the unmasked bit doesn't fit in code. Also it would
		 * be a 0 anyways. So it doesn't matter that much
		 */
		code = (GetChar<char8_t>(0) & 0x1f) << 12
		     | (GetChar<char8_t>(1) & 0x3f) << 6 | (GetChar<char8_t>(2) & 0x3f);
		StepStrm<char8_t>(3);
	}

	/* NOTE: 4-byte to 7-byte UTF-8 sequences usually encode code points outside
	 * of the BMP; I think sticking to the BMP is fine here.
	 */

	return code;
}

char16_t CharStrmReader::ReadNextCharUTF16()
{
	byte2_t code;

	code = GetChar<char16_t>(0);
	StepStrm<char16_t>(1);

	return code;
}

char16_t CharStrmReader::ReadNextCharCP1252()
{
	byte2_t code;

	code = GetChar<unsigned char>(0);
	StepStrm<unsigned char>(1);

	return code;
}

char16_t CharStrmReader::ReadNextCharSJIS()
{
	byte2_t code;

	if (IsSJISLeadByte(GetChar<unsigned char>(0)))
	{
		code = GetChar<unsigned char>(0) << 8 | GetChar<unsigned char>(1);
		StepStrm<unsigned char>(2);
	}
	else
	{
		code = GetChar<unsigned char>(0);
		StepStrm<unsigned char>(1);
	}

	return code;
}

}} // namespace nw4hbm::ut
