#ifndef NW4HBM_UT_ROM_FONT_H
#define NW4HBM_UT_ROM_FONT_H

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

#include "Font.h"

// #include <revolution/gx/GXEnum.h>
// #include <revolution/os/OSFont.h>

#include <context_rvl.h>

/*******************************************************************************
 * classes and functions
 */

namespace nw4hbm { namespace ut
{
	// [SC5PGN]/build/libs/Debug/slamWiiD.a:ut_RomFont.o(1)::.debug_info::0x1f4 [original object]
	class RomFont : public Font
	{
	// methods
	public:
		// cdtors
		RomFont();
		virtual ~RomFont();

		// virtual function ordering
		// vtable Font
		virtual int GetWidth() const;
		virtual int GetHeight() const;
		virtual int GetAscent() const;
		virtual int GetDescent() const;
		virtual int GetBaselinePos() const;
		virtual int GetCellHeight() const;
		virtual int GetCellWidth() const;
		virtual int GetMaxCharWidth() const;
		virtual Type GetType() const;
		virtual GXTexFmt GetTextureFormat() const;
		virtual int GetLineFeed() const;
		virtual CharWidths GetDefaultCharWidths() const;
		virtual void SetDefaultCharWidths(CharWidths const &widths);
		virtual bool SetAlternateChar(char16_t c);
		virtual void SetLineFeed(int linefeed);
		virtual int GetCharWidth(char16_t c) const;
		virtual CharWidths GetCharWidths(char16_t c) const;
		virtual void GetGlyph(Glyph *glyph, char16_t c) const;
		virtual FontEncoding GetEncoding() const;

		// methods
		u32 GetRequireBufferSize();

		bool Load(void *buffer);
		void *Unload();
		void MakeCharPtr(char *buffer, char16_t c) const;
		char16_t HandleUndefinedChar(byte2_t c) const;

	// static members
	private:
		static OSFontEncode mFontEncode;

	// members
	private:
		/* base Font */					// size 0x10, offset 0x00
		OSFontHeader	*mFontHeader;	// size 0x04, offset 0x10
		CharWidths		mDefaultWidths;	// size 0x03, offset 0x14
		/* 1 byte padding */
		char16_t		mAlternateChar;	// size 0x02, offset 0x18
		/* 2 bytes padding */
	}; // size 0x1c
}} // namespace nw4hbm::ut

#endif // NW4HBM_UT_ROM_FONT_H
