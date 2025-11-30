#include "RomFont.h"

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

#include "Font.h"
#include "inlines.h"

// #include <revolution/gx/GXEnum.h>
// #include <revolution/os/OSFont.h>

#include <context_rvl.h>

/*******************************************************************************
 * local function declarations
 */

namespace nw4hbm { namespace ut { namespace
{
	inline bool IsCP1252Char(byte2_t c)
	{
		return 0x20 <= c && c <= 0xFF;
	}

	inline bool IsSJISHalfWidthChar(byte2_t c)
	{
		if (c > 0xFF)
			return false;

		return (0x20 <= c && c <= 0x7E) || (0xA1 <= c && c <= 0xCF);
	}

	inline bool IsSJISFullWidthChar(byte2_t c)
	{
		byte1_t lead = BitExtract(c, 8, 8);
		byte1_t trail = BitExtract(c, 0, 8);

		// NOTE: parentheses required
		return 0x81 <= lead && lead <= 0x98 && (0x40 <= trail && trail <= 0xFC);
	}
}}} // namespace nw4hbm::ut::(unnamed)

/*******************************************************************************
 * variables
 */

namespace nw4hbm { namespace ut
{
	// .data
	OSFontEncode RomFont::mFontEncode = OS_FONT_ENCODE_INVALID;
}} // namespace nw4hbm::ut

/*******************************************************************************
 * functions
 */

namespace nw4hbm { namespace ut {

RomFont::RomFont():
	mFontHeader		(nullptr),
	mAlternateChar	(L'?')
{
	mDefaultWidths.left			= 0;
	mDefaultWidths.glyphWidth	= 0;
	mDefaultWidths.charWidth	= 0;
}

RomFont::~RomFont() {}

bool RomFont::Load(void *buffer)
{
	OSFontHeader *fontHeader = static_cast<OSFontHeader *>(buffer);

	if (mFontHeader)
		return false;

	BOOL bSuccess = OSInitFont(fontHeader);
	if (bSuccess)
	{
		mFontEncode = OSGetFontEncode();

		mFontHeader = fontHeader;
		mDefaultWidths.left = 0;
		mDefaultWidths.glyphWidth = GetCellWidth();
		mDefaultWidths.charWidth = GetMaxCharWidth();

		InitReaderFunc(GetEncoding());
	}

	return bSuccess;
}

void *RomFont::Unload()
{
	void *buffer = mFontHeader;

	mFontHeader = nullptr;

	return buffer;
}

u32 RomFont::GetRequireBufferSize()
{
	OSFontEncode fontEncode = OSGetFontEncode();

	switch (fontEncode)
	{
	case OS_FONT_ENCODE_ANSI:
		return OS_FONT_SIZE_ANSI;

	case OS_FONT_ENCODE_SJIS:
		return OS_FONT_SIZE_SJIS;

	default:
		return 0;
	}
}

int RomFont::GetWidth() const
{
	return mFontHeader->width;
}

int RomFont::GetHeight() const
{
	return GetAscent() + GetDescent();
}

int RomFont::GetAscent() const
{
	return mFontHeader->ascent;
}

int RomFont::GetDescent() const
{
	return mFontHeader->descent;
}

int RomFont::GetBaselinePos() const
{
	return mFontHeader->ascent; // ?
}

int RomFont::GetCellHeight() const
{
	return mFontHeader->cellHeight;
}

int RomFont::GetCellWidth() const
{
	return mFontHeader->cellWidth;
}

int RomFont::GetMaxCharWidth() const
{
	return mFontHeader->width;
}

Font::Type RomFont::GetType() const
{
	return TYPE_ROM;
}

GXTexFmt RomFont::GetTextureFormat() const
{
	return GX_TF_I4;
}

int RomFont::GetLineFeed() const
{
	return mFontHeader->leading; // ?
}

CharWidths RomFont::GetDefaultCharWidths() const
{
	return mDefaultWidths;
}

void RomFont::SetDefaultCharWidths(CharWidths const &widths)
{
	mDefaultWidths = widths;
}

bool RomFont::SetAlternateChar(char16_t c)
{
	char16_t const oldValue = mAlternateChar;

	mAlternateChar = 0xFFFF; // non-character in UTF-8; safe to use as sentinel

	char16_t check = HandleUndefinedChar(c);

	if (check != 0xFFFF)
	{
		mAlternateChar = c;

		return true;
	}
	else
	{
		mAlternateChar = oldValue;

		return false;
	}
}

void RomFont::SetLineFeed(int linefeed)
{
	mFontHeader->leading = linefeed; // ?
}

int RomFont::GetCharWidth(char16_t c) const
{
	s32 width;
	char buf[3];

	MakeCharPtr(buf, c);

	OSGetFontWidth(buf, &width);

	return width;
}

CharWidths RomFont::GetCharWidths(char16_t c) const
{
	int width = GetCharWidth(c);

	CharWidths widths;
	widths.left			= 0;
	widths.glyphWidth	= width;
	widths.charWidth	= width;

	return widths;
}

void RomFont::GetGlyph(Glyph *glyph, char16_t c) const
{
	void *pTexture;
	s32 x, y, width;
	char buf[3];

	MakeCharPtr(buf, c);
	OSGetFontTexture(buf, &pTexture, &x, &y, &width);

	glyph->pTexture				= pTexture;
	glyph->widths.left			= 0;
	glyph->widths.glyphWidth	= width;
	glyph->widths.charWidth		= width;
	glyph->height				= mFontHeader->cellHeight;
	glyph->texFormat			= GX_TF_I4;
	glyph->texWidth				= mFontHeader->sheetWidth;
	glyph->texHeight			= mFontHeader->sheetHeight;
	glyph->cellX				= x;
	glyph->cellY				= y;
}

FontEncoding RomFont::GetEncoding() const
{
	switch (mFontEncode)
	{
	case OS_FONT_ENCODE_ANSI:
		return FONT_ENCODING_CP1252;

	case OS_FONT_ENCODE_SJIS:
		return FONT_ENCODING_SJIS;

	default:
		return FONT_ENCODING_CP1252;
	}
}

void RomFont::MakeCharPtr(char *buffer, char16_t c) const
{
	c = HandleUndefinedChar(c);

	if (BitExtract(c, 8, 8) == 0x00)
	{
		buffer[0] = BitExtract(c, 0, 8);
		buffer[1] = '\0';
	}
	else
	{
		buffer[0] = BitExtract(c, 8, 8);
		buffer[1] = BitExtract(c, 0, 8);
		buffer[2] = '\0';
	}
}

char16_t RomFont::HandleUndefinedChar(byte2_t c) const
{
	bool bDefined;

	switch (mFontEncode)
	{
	case OS_FONT_ENCODE_ANSI:
		bDefined = IsCP1252Char(c);
		break;

	case OS_FONT_ENCODE_SJIS:
		bDefined = IsSJISHalfWidthChar(c) || IsSJISFullWidthChar(c);
		break;

	default: // NOLINT
		/* stripped assert? in which case the program would have aborted before
		 * reaching the return statement and using bDefined uninitialized
		 */
		(void)0;

		break;
	}

	return bDefined ? c : mAlternateChar;
}

}} // namespace nw4hbm::ut
