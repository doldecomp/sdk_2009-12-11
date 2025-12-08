#ifndef RVL_SDK_HBM_GUI_MANAGER_H
#define RVL_SDK_HBM_GUI_MANAGER_H

/*******************************************************************************
 * headers
 */

#include <cstddef>

#include <macros.h>

#include <revolution/types.h>

#include "nw4hbm/lyt/pane.h"
#include "nw4hbm/ut/list.h"

#include <revolution/kpad.h>
// #include <revolution/mem/allocator.h>
// #include <revolution/mtx.h>

#include <context_rvl.h>

/*******************************************************************************
 * classes and functions
 */

// context declarations
namespace nw4hbm { namespace lyt { class DrawInfo; }}
namespace nw4hbm { namespace lyt { class Layout; }}

namespace homebutton { namespace gui
{
	// forward declarations
	class Manager;
	class PaneComponent;

	// [SC5PGN]/build/libs/Debug/slamWiiD.a:HBMGUIManager.o(1)::.debug_info::0x4f1 [original object]
	class Interface
	{
	// methods
	public:
		// cdtors
		/* Interface() = default; */
		/* virtual ~Interface() {} */ // virtual function ordering

		// virtual function ordering
		// vtable Interface
		virtual void create() {}
		virtual void init() {}
		virtual void calc() {}
		virtual void draw(Mtx &) {}
		virtual void draw() {}
		virtual ~Interface() {}

	// members
	private:
		/* vtable */	// size 0x04, offset 0x00
	}; // size 0x04

	// [SC5PGN]/build/libs/Debug/slamWiiD.a:HBMGUIManager.o(1)::.debug_info::0x5d1 [original object]
	class EventHandler
	{
	// enums
	public:
		typedef u32 EventType;
		enum EventType_et
		{
			TrigEvent,
			PointEvent,
			LeftEvent,
			Event3,			// unsure. some sort of MoveEvent maybe? idk
			Event4,			// Completely unused. Old removed event?
			ReleaseEvent	// Possibly; see Manager::update
		};

	// methods
	public:
		// cdtors
		/* EventHandler() = default; */
		/* ~EventHandler() = default; */ // NOTE: Not virtual

		// virtual function ordering
		// vtable EventHandler
		virtual void onEvent(u32, EventType, void *) {}
		virtual void setManager(Manager *pManager) { mpManager = pManager; }

	// members
	protected: // HomeButtonEventHandler::onEvent
		/* vtable */		// size 0x04, offset 0x00
		Manager	*mpManager;	// size 0x04, offset 0x04
	}; // size 0x08

	// [SC5PGN]/build/libs/Debug/slamWiiD.a:HBMGUIManager.o(1)::.debug_info::0x43a [original object]
	class Component : public Interface
	{
		// NOTE the misspelling of triggerTarget as triggerTarger

	// methods
	public:
		// cdtors
		Component(u32 uID):
			mbDragging			(false),
			muDraggingButton	(0),
			muID				(uID),
			mbTriggerTarger		(false),
			mpManager			(nullptr)
		{
			init();
		}

		/* virtual ~Component() = default; */

		// virtual function ordering
		// vtable Interface
		virtual void init()
		{
			mbDragging = false;

			for (int i = 0; i < (int)ARRAY_LENGTH(mabPointed); ++i)
				mabPointed[i] = false;
		}

		// vtable Component
		virtual u32 getID() { return muID; }
		virtual int isPointed(int n) { return mabPointed[n]; }
		virtual void setPointed(int n, bool b) { mabPointed[n] = b; }
		virtual void onPoint() {}
		virtual void offPoint() {}
		virtual void onDrag(f32, f32) {}
		virtual void onMove(f32, f32) {}
		virtual void onTrig(byte4_t uFlag, Vec &vec)
		{
			if (uFlag & muDraggingButton)
			{
				mDragStartPos = vec;
				mbDragging = true;
			}
		}

		virtual void setDraggingButton(byte4_t uDraggingButton)
		{
			muDraggingButton = uDraggingButton;
		}

		virtual bool update(int, KPADStatus const *, f32, f32, void *)
		{
			return false;
		}

		virtual bool update(int i, f32 x, f32 y, byte4_t uTrigFlag,
		                    byte4_t uHoldFlag, byte4_t uReleaseFlag,
		                    void *pData);
		virtual bool isTriggerTarger() { return mbTriggerTarger; }
		virtual void setTriggerTarget(bool bTriggerTarget)
		{
			mbTriggerTarger = bTriggerTarget;
		}

		virtual void setManager(Manager *pManager) { mpManager = pManager; }
		virtual bool isVisible() { return true; }
		virtual bool contain(f32 x_, f32 y_) = 0;

	// members
	protected: // PaneComponent::contain
		/* base Interface */		// size 0x04, offset 0x00
		bool	mabPointed[8];		// size 0x08, offset 0x04
		Vec		mDragStartPos;		// size 0x0c, offset 0x0c
		bool	mbDragging;			// size 0x01, offset 0x18
		/* 3 bytes padding */
		byte4_t	muDraggingButton;	// size 0x04, offset 0x1c
		u32		muID;				// size 0x04, offset 0x20
		bool	mbTriggerTarger;	// size 0x01, offset 0x24
		/* 3 bytes padding */
		Manager	*mpManager;			// size 0x04, offset 0x28
	}; // size 0x2c

	// [SC5PGN]/build/libs/Debug/slamWiiD.a:HBMGUIManager.o(1)::.debug_info::0x568 [original object]
	class Manager : public Interface
	{
	// nested types
	private:
		// [SC5PGN]/build/libs/Debug/slamWiiD.a:HBMGUIManager.o(1)::.debug_info::0x7ef [original object]
		struct IDToComponent
		{
		// methods
		public:
			// cdtors
			IDToComponent(u32 uID, Component *pComponent):
				muID		(uID),
				mpComponent	(pComponent)
				{}

		// members
		public:
			u32					muID;			// size 0x04, offset 0x00
			Component			*mpComponent;	// size 0x04, offset 0x04
			nw4hbm::ut::Link	mLink;			// size 0x08, offset 0x08
		}; // size 0x10

	// methods
	public:
		// cdtors
		Manager(EventHandler *pEventHandler, MEMAllocator *pAllocator):
			mpEventHandler	(pEventHandler),
			mpAllocator		(pAllocator)
		{
			if (mpEventHandler)
				mpEventHandler->setManager(this);

			nw4hbm::ut::List_Init(&mIDToComponent,
			                      offsetof(IDToComponent, mLink));
		}

		virtual ~Manager();

		// virtual function ordering
		// vtable Interface
		virtual void init();
		virtual void calc();
		virtual void draw();

		// vtable Manager
		virtual void addComponent(Component *pComponent);
		virtual void delComponent(Component *pComponent);
		virtual Component *getComponent(u32 uID);
		virtual bool update(int, KPADStatus const *, f32, f32, void *)
		{
			return false;
		}

		virtual bool update(int i, f32 x, f32 y, byte4_t uTrigFlag,
		                    byte4_t uHoldFlag, byte4_t uReleaseFlag,
		                    void *pData);
		virtual void onEvent(u32 uID, EventHandler::EventType uEvent,
		                     void *pData)
		{
			if (mpEventHandler)
				mpEventHandler->onEvent(uID, uEvent, pData);
		}

		virtual void setAllComponentTriggerTarget(bool b);
		virtual void setEventHandler(EventHandler *pEventHandler)
		{
			mpEventHandler = pEventHandler;

			if (mpEventHandler)
				mpEventHandler->setManager(this);
		}

	// members
	protected: // PaneManager::~PaneManager
		/* base Interface */					// size 0x04, offset 0x00
		EventHandler		*mpEventHandler;	// size 0x04, offset 0x04
		nw4hbm::ut::List	mIDToComponent;		// size 0x0c, offset 0x08
		MEMAllocator		*mpAllocator;		// size 0x04, offset 0x14
	}; // size 0x18

	// [SC5PGN]/build/libs/Debug/slamWiiD.a:HBMGUIManager.o(1)::.debug_info::0x94d [original object]
	class PaneManager : public Manager
	{
	// nested types
	private:
		// [SC5PGN]/build/libs/Debug/slamWiiD.a:HBMGUIManager.o(1)::.debug_info::0xe6d [original object]
		struct PaneToComponent
		{
		// methods
		public:
			// cdtors
			PaneToComponent(nw4hbm::lyt::Pane *pPane,
			                PaneComponent *pComponent):
				mpPane		(pPane),
				mpComponent	(pComponent)
				{}

		// members
		public:
			nw4hbm::lyt::Pane	*mpPane;		// size 0x04, offset 0x00
			PaneComponent		*mpComponent;	// size 0x04, offset 0x04
			nw4hbm::ut::Link	mLink;			// size 0x08, offset 0x08
		}; // size 0x10

	// methods
	public:
		// cdtors
		PaneManager(EventHandler *pEventHandler,
		            nw4hbm::lyt::DrawInfo const *pDrawInfo,
		            MEMAllocator *pAllocator):
			Manager		(pEventHandler, pAllocator),
			mpDrawInfo	(pDrawInfo)
		{
			nw4hbm::ut::List_Init(&mPaneToComponent,
			                      offsetof(PaneToComponent, mLink));
		}

		virtual ~PaneManager();

		// virtual function ordering
		// vtable PaneManager
		virtual void createLayoutScene(nw4hbm::lyt::Layout const &rLayout);
		virtual void addLayoutScene(nw4hbm::lyt::Layout const &rLayout);
		virtual void delLayoutScene(nw4hbm::lyt::Layout const &rLayout);
		virtual PaneComponent *getPaneComponentByPane(nw4hbm::lyt::Pane *pPane);
		virtual nw4hbm::lyt::DrawInfo const *getDrawInfo()
		{
			return mpDrawInfo;
		}

		virtual void setDrawInfo(nw4hbm::lyt::DrawInfo const *pDrawInfo)
		{
			mpDrawInfo = pDrawInfo;
		}

		virtual void setAllBoundingBoxComponentTriggerTarget(bool b);
		virtual void walkInChildren(nw4hbm::lyt::Pane::LinkList &rPaneList);
		virtual void walkInChildrenDel(nw4hbm::lyt::Pane::LinkList &rPaneList);

	// static members
	private:
		static u32 suIDCounter;

	// members
	private:
		/* base Manager */							// size 0x18, offset 0x00
		nw4hbm::ut::List		mPaneToComponent;	// size 0x0c, offset 0x18
		nw4hbm::lyt::DrawInfo	const *mpDrawInfo;	// size 0x04, offset 0x24
		u16						muNumPoint;			// size 0x02, offset 0x28
		byte2_t					muPadding;
	}; // size 0x2c

	// [SC5PGN]/build/libs/Debug/slamWiiD.a:HBMGUIManager.o(1)::.debug_info::0x1438 [original object]
	class PaneComponent : public Component
	{
	// methods
	public:
		// cdtors
		PaneComponent(u32 uID):
			Component	(uID),
			mpPane		(nullptr)
			{}

		/* virtual ~PaneComponent() = default; */

		// virtual function ordering
		// vtable Interface
		virtual void draw();

		// vtable Component
		virtual bool isVisible();
		virtual bool contain(f32 x_, f32 y_);

		// vtable PaneComponent
		virtual void setPane(nw4hbm::lyt::Pane *pPane) { mpPane = pPane; }
		virtual nw4hbm::lyt::Pane *getPane() { return mpPane; }

	// members
	private:
		/* base Component */			// size 0x2c, offset 0x00
		nw4hbm::lyt::Pane	*mpPane;	// size 0x04, offset 0x2c
	}; // size 0x30
}} // namespace homebutton::gui

#endif // RVL_SDK_HBM_GUI_MANAGER_H
