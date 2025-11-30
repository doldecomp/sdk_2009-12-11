#ifndef NW4HBM_UT_RES_FONT_H
#define NW4HBM_UT_RES_FONT_H

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

#include "../macros.h"

#include "ResFontBase.h"

/*******************************************************************************
 * classes and functions
 */

// context declarations
namespace nw4hbm { namespace ut { struct FontInformation; }}
namespace nw4hbm { namespace ut { struct BinaryFileHeader; }}

namespace nw4hbm { namespace ut
{
	// [SC5PGN]/build/libs/Debug/slamWiiD.a:ut_ResFont.o(1)::.debug_info::0x1d4 [original object]
	class ResFont : public detail::ResFontBase
	{
	// methods
	public:
		// cdtors
		ResFont();
		virtual ~ResFont();

		// methods
		bool SetResource(void *brfnt);
		void *RemoveResource();

		// static methods
		static FontInformation *Rebuild(BinaryFileHeader *fileHeader);

	// static members
	public:
		static byte4_t const SIGNATURE_FONT = NW4HBM_FOUR_CHAR('R', 'F', 'N', 'T');
		static byte4_t const SIGNATURE_FONT_UNPACKED = NW4HBM_FOUR_CHAR('R', 'F', 'N', 'U');
		static byte4_t const SIGNATURE_FONT_INFO = NW4HBM_FOUR_CHAR('F', 'I', 'N', 'F');
		static byte4_t const SIGNATURE_FONT_TEX_GLYPH = NW4HBM_FOUR_CHAR('T', 'G', 'L', 'P');
		static byte4_t const SIGNATURE_FONT_CHAR_WIDTH = NW4HBM_FOUR_CHAR('C', 'W', 'D', 'H');
		static byte4_t const SIGNATURE_FONT_CODE_MAP = NW4HBM_FOUR_CHAR('C', 'M', 'A', 'P');

		// TODO: What is this? glyph something? graphic?
		static byte4_t const SIGNATURE_FONT_GLGR = NW4HBM_FOUR_CHAR('G', 'L', 'G', 'R');

		static int const FILE_VERSION = NW4HBM_FILE_VERSION(1, 4);

	// members
	private:
		/* base ResFontBase */	// size 0x18, offset 0x00
	}; // size 0x18
}} // namespace nw4hbm::ut

#endif // NW4HBM_UT_RES_FONT_H
