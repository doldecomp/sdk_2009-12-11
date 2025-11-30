#include "drawInfo.h"

/*******************************************************************************
 * headers
 */

#include <cstring>

#include "../math/types.h"

/*******************************************************************************
 * functions
 */

namespace nw4hbm { namespace lyt {

DrawInfo::DrawInfo():
	mLocationAdjustScale	(1.0f, 1.0f),
	mGlobalAlpha			(1.0f)
{
	std::memset(&mFlag, 0, sizeof mFlag);

	math::MTX34Identity(&mViewMtx);
}

DrawInfo::~DrawInfo() {}

}} // namespace nw4hbm::lyt
