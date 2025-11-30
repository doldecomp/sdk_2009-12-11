#include "HBMAnmController.h"

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

#include "HBMFrameController.h"

#include "nw4hbm/lyt/animation.h"
#include "nw4hbm/lyt/group.h"
#include "nw4hbm/lyt/pane.h"
#include "nw4hbm/ut/LinkList.h"

/*******************************************************************************
 * functions
 */

namespace homebutton {

GroupAnmController::GroupAnmController():
	mpGroup		(nullptr),
	mpAnimGroup	(nullptr)
	{}

GroupAnmController::~GroupAnmController() {}

void GroupAnmController::do_calc()
{
	bool flag;

	if (mState == eState_Playing)
	{
		calc();

		flag = true;

		mpAnimGroup->SetFrame(mCurFrame);
	}
	else
	{
		flag = false;
	}

	nw4hbm::lyt::detail::PaneLink::LinkList &list = mpGroup->GetPaneList();

	NW4HBM_RANGE_FOR(it, list)
		it->mTarget->SetAnimationEnable(mpAnimGroup, flag, false);
}

} // namespace homebutton
