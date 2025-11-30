#ifndef NW4HBM_UT_FONT_H
#define NW4HBM_UT_FONT_H

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

#include "CharStrmReader.h"

// #include <revolution/gx/GXEnum.h>

#include <context_rvl.h>

/*******************************************************************************
 * classes and functions
 */

namespace nw4hbm { namespace ut
{
	// [SC5PGN]/build/libs/Debug/slamWiiD.a:ut_Font.o(1)::.debug_info::0x153 [original object]
	enum FontEncoding
	{
		FONT_ENCODING_UTF8,
		FONT_ENCODING_UTF16,
		FONT_ENCODING_SJIS,
		FONT_ENCODING_CP1252,

		NUM_OF_FONT_ENCODING
	};

	typedef u16 FontMapMethod;
	enum FontMapMethod_et
	{
		FONT_MAPMETHOD_DIRECT,	/* name taken from asserts ([SPQE7T]:/ISpyD.elf) */
		FONT_MAPMETHOD_TABLE,	/* name taken from asserts ([SPQE7T]:/ISpyD.elf) */
		FONT_MAPMETHOD_SCAN		/* name taken from asserts ([SPQE7T]:/ISpyD.elf) */
	};

	// [SC5PGN]/build/libs/Debug/slamWiiD.a:ut_ResFont.o(1)::.debug_info::0x3af [original object]
	struct CharWidths
	{
		s8	left;			// size 0x01, offset 0x00
		u8	glyphWidth;		// size 0x01, offset 0x01
		s8	charWidth;		// size 0x01, offset 0x02
	}; // size 0x03

	// [SC5PGN]/build/libs/Debug/slamWiiD.a:ut_ResFont.o(1)::.debug_info::0x5ab [original object]
	struct FontCodeMap
	{
		char16_t		ccodeBegin;		// size 0x02, offset 0x00
		char16_t		ccodeEnd;		// size 0x02, offset 0x02
		FontMapMethod	mappingMethod;	// size 0x02, offset 0x04
		byte2_t			reserved;
		FontCodeMap		*pNext;			// size 0x04, offset 0x08
		u16				mapInfo[];		// flexible,  offset 0x0c (unit size 0x02)
	}; // size 0x0c (flexible)

	// [SC5PGN]/build/libs/Debug/slamWiiD.a:ut_ResFont.o(1)::.debug_info::0x3ff [original object]
	struct FontTextureGlyph
	{
		u8		cellWidth;		// size 0x01, offset 0x00
		u8		cellHeight;		// size 0x01, offset 0x01
		s8		baselinePos;	// size 0x01, offset 0x02
		u8		maxCharWidth;	// size 0x01, offset 0x03
		u32		sheetSize;		// size 0x04, offset 0x04
		u16		sheetNum;		// size 0x02, offset 0x08
		u16		sheetFormat;	// size 0x02, offset 0x0a
		u16		sheetRow;		// size 0x02, offset 0x0c
		u16		sheetLine;		// size 0x02, offset 0x0e
		u16		sheetWidth;		// size 0x02, offset 0x10
		u16		sheetHeight;	// size 0x02, offset 0x12
		byte_t	*sheetImage;	// size 0x04, offset 0x14
	}; // size 0x18

	// [SC5PGN]/build/libs/Debug/slamWiiD.a:ut_ResFont.o(1)::.debug_info::0x532 [original object]
	struct FontWidth
	{
		u16			indexBegin;		// size 0x02, offset 0x00
		u16			indexEnd;		// size 0x02, offset 0x02
		FontWidth	*pNext;			// size 0x04, offset 0x04
		CharWidths	widthTable[];	// flexible,  offset 0x08 (unit size 0x03)
	}; // size 0x08 (flexible)

	// down here for no forward declarations
	// [SC5PGN]/build/libs/Debug/slamWiiD.a:ut_ResFont.o(1)::.debug_info::0x27b [original object]
	struct FontInformation
	{
		u8					fontType;		// size 0x01, offset 0x00
		s8					linefeed;		// size 0x01, offset 0x01
		u16					alterCharIndex;	// size 0x02, offset 0x02
		CharWidths			defaultWidth;	// size 0x03, offset 0x04
		u8					encoding;		// size 0x01, offset 0x07
		FontTextureGlyph	*pGlyph;		// size 0x04, offset 0x08
		FontWidth			*pWidth;		// size 0x04, offset 0x0c
		FontCodeMap			*pMap;			// size 0x04, offset 0x10
		u8					height;			// size 0x01, offset 0x14
		u8					width;			// size 0x01, offset 0x15
		u8					ascent;			// size 0x01, offset 0x16
		byte1_t				padding_[1];
	}; // size 0x18

	// [SC5PGN]/build/libs/Debug/slamWiiD.a:ut_ResFontBase.o(1)::.debug_info::0x908 [original object]
	struct Glyph
	{
		void		*pTexture;	// size 0x04, offset 0x00
		CharWidths	widths;		// size 0x03, offset 0x04
		u8			height;		// size 0x01, offset 0x07
		GXTexFmt	texFormat;	// size 0x04, offset 0x08
		u16			texWidth;	// size 0x02, offset 0x0c
		u16			texHeight;	// size 0x02, offset 0x0e
		u16			cellX;		// size 0x02, offset 0x10
		u16			cellY;		// size 0x02, offset 0x12
	}; // size 0x14

	// [SC5PGN]/build/libs/Debug/slamWiiD.a:ut_Font.o(1)::.debug_info::0x11a [original object]
	class Font
	{
	// enums
	public:
		// [SC5PGN]/build/libs/Debug/slamWiiD.a:ut_RomFontBase.o(1)::.debug_info::0x6b8 [original object]
		enum Type
		{
			TYPE_NULL,

			TYPE_ROM,
			TYPE_RESOURCE
		};

	// methods
	public:
		// cdtors
		Font():
			mReaderFunc	(&CharStrmReader::ReadNextCharCP1252)
			{}

		virtual ~Font() {}

		// virtual function ordering
		// vtable Font
		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;
		virtual int GetAscent() const = 0;
		virtual int GetDescent() const = 0;
		virtual int GetBaselinePos() const = 0;
		virtual int GetCellHeight() const = 0;
		virtual int GetCellWidth() const = 0;
		virtual int GetMaxCharWidth() const = 0;
		virtual Type GetType() const = 0;
		virtual GXTexFmt GetTextureFormat() const = 0;
		virtual int GetLineFeed() const = 0;
		virtual CharWidths GetDefaultCharWidths() const = 0;
		virtual void SetDefaultCharWidths(CharWidths const &widths) = 0;
		virtual bool SetAlternateChar(char16_t c) = 0;
		virtual void SetLineFeed(int linefeed) = 0;
		virtual int GetCharWidth(char16_t c) const = 0;
		virtual CharWidths GetCharWidths(char16_t c) const = 0;
		virtual void GetGlyph(Glyph *glyph, char16_t c) const = 0;
		virtual FontEncoding GetEncoding() const = 0;

		// methods
		void InitReaderFunc(FontEncoding encoding);

		/* NOTE: This function generates ~CharStrmReader() simply by being
		 * defined, on account of returning one by value. If you've got it at
		 * the beginning of an object, this function might be the reason.
		 */
		CharStrmReader GetCharStrmReader() const
		{
			CharStrmReader reader(mReaderFunc);

			return reader;
		}

	// members
	private:
		/* vtable */								// size 0x04, offset 0x00
		CharStrmReader::ReadFunc	mReaderFunc;	// size 0x0c, offset 0x04
	}; // size 0x10
}} // namespace nw4hbm::ut

#endif // NW4HBM_UT_FONT_H
