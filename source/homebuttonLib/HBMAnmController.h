#ifndef RVL_SDK_HBM_ANM_CONTROLLER_H
#define RVL_SDK_HBM_ANM_CONTROLLER_H

/*******************************************************************************
 * headers
 */

#include "HBMFrameController.h"

/*******************************************************************************
 * classes and functions
 */

// forward declarations
namespace nw4hbm { namespace lyt { class AnimTransform; }}
namespace nw4hbm { namespace lyt { class Group; }}

namespace homebutton
{
	// [SC5PGN]/build/libs/Debug/slamWiiD.a:HBMAnmController.o(1)::.debug_info::0x16a [original object]
	class GroupAnmController : public FrameController
	{
	// methods
	public:
		// cdtors
		GroupAnmController();
		virtual ~GroupAnmController();

		// methods
		void do_calc();

	// members
	public: // HomeButton::create
		/* base FrameController */					// size 0x20, offset 0x00
		nw4hbm::lyt::Group			*mpGroup;		// size 0x04, offset 0x20
		nw4hbm::lyt::AnimTransform	*mpAnimGroup;	// size 0x04, offset 0x24
	}; // size 0x28
} // namespace homebutton

#endif // RVL_SDK_HBM_ANM_CONTROLLER_H
