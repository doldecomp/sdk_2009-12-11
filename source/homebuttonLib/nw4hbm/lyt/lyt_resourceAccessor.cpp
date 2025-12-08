#include "resourceAccessor.h"

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

/*******************************************************************************
 * types
 */

// context declarations
namespace nw4hbm { namespace ut { class Font; }}

/*******************************************************************************
 * functions
 */

namespace nw4hbm { namespace lyt {

// Destructor is first for some reason. Idk
ResourceAccessor::~ResourceAccessor() {}

ResourceAccessor::ResourceAccessor() {}

ut::Font *ResourceAccessor::GetFont(char const *)
{
	return nullptr;
}

}} // namespace nw4hbm::lyt
