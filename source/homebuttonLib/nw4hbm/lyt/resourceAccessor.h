#ifndef NW4HBM_LYT_RESOURCE_ACCESSOR_H
#define NW4HBM_LYT_RESOURCE_ACCESSOR_H

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

#include "../macros.h"

/*******************************************************************************
 * classes and functions
 */

// context declarations
namespace nw4hbm { namespace ut { class Font; }}

namespace nw4hbm { namespace lyt
{
	// [SC5PGN]/build/libs/Debug/slamWiiD.a:lyt_resourceAccessor.o(1)::.debug_info::0x13f [original object]
	class ResourceAccessor
	{
	// methods
	public:
		// cdtors
		ResourceAccessor();
		virtual ~ResourceAccessor();

		// virtual function ordering
		// vtable ResourceAccessor
		virtual void *GetResource(byte4_t resType, char const *name,
		                           u32 *pSize) = 0;
		virtual ut::Font *GetFont(char const *name);

	// static members
	public:
		static byte4_t const SIGNATURE_FONT =
			NW4HBM_FOUR_CHAR('f', 'o', 'n', 't');

		static byte4_t const SIGNATURE_TEXTURE_IMAGE =
			NW4HBM_FOUR_CHAR('t', 'i', 'm', 'g');

		// members
	private:
		/* vtable */	// size 0x04, offset 0x00
	}; // size 0x04
}} // namespace nw4hbm::lyt

#endif // NW4HBM_LYT_RESOURCE_ACCESSOR_H
