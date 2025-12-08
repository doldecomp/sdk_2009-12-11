#include "bounding.h"

/*******************************************************************************
 * headers
 */

#include "common.h"
#include "drawInfo.h"
#include "pane.h"

#include "../ut/Color.h"
#include "../ut/RuntimeTypeInfo.h"

/*******************************************************************************
 * variables
 */

namespace nw4hbm { namespace lyt
{
	// .bss
	ut::detail::RuntimeTypeInfo const Bounding::typeInfo(&Pane::typeInfo);
}} // namespace nw4hbm::lyt

/*******************************************************************************
 * functions
 */

namespace nw4hbm { namespace lyt {

Bounding::Bounding(res::Bounding const *pBlock, ResBlockSet const &):
	Pane	(pBlock)
	{}

Bounding::~Bounding() {}

void Bounding::DrawSelf(DrawInfo const &drawInfo)
{
	if (drawInfo.IsDebugDrawMode())
	{
		LoadMtx(drawInfo);
		detail::DrawLine(GetVtxPos(), mSize, 0x00ff00ff); // green
	}
}

}} // namespace nw4hbm::lyt
