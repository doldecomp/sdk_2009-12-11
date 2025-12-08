#ifndef NW4HBM_UT_CHAR_STRM_READER_H
#define NW4HBM_UT_CHAR_STRM_READER_H

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

/*******************************************************************************
 * classes and functions
 */

namespace nw4hbm { namespace ut
{
	// [SC5PGN]/build/libs/Debug/slamWiiD.a:ut_CharStrmReader.o(1)::.debug_info::0xd4 [original object]
	class CharStrmReader
	{
	// typedefs
	public:
		typedef char16_t (CharStrmReader::*ReadFunc)();

	// methods
	public:
		// cdtors
		CharStrmReader(ReadFunc func):
			mCharStrm	(nullptr),
			mReadFunc	(func)
			{}

		~CharStrmReader() {}

		// methods
		void const *GetCurrentPos() const { return mCharStrm; }

		void Set(char const *stream) { mCharStrm = stream; }
		void Set(wchar_t const *stream) { mCharStrm = stream; }

		char16_t ReadNextCharUTF8();
		char16_t ReadNextCharUTF16();
		char16_t ReadNextCharCP1252();
		char16_t ReadNextCharSJIS();

		char16_t Next() { return (this->*mReadFunc)(); }

		template <typename CharT>
		CharT GetChar(int offset) const
		{
			CharT const *charStrm = static_cast<CharT const *>(mCharStrm);

			return charStrm[offset];
		}

		template <typename CharT>
		void StepStrm(int step)
		{
			CharT const *&charStrm =
				reinterpret_cast<CharT const *&>(mCharStrm);

			charStrm += step;
		}

	// members
	private:
		void		const *mCharStrm;	// size 0x04, offset 0x00
		ReadFunc	mReadFunc;			// size 0x0c, offset 0x04
	}; // size 0x10
}} // namespace nw4hbm::ut

#endif // NW4HBM_UT_CHAR_STRM_READER_H
