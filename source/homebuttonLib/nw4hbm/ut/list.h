#ifndef NW4HBM_UT_LIST_H
#define NW4HBM_UT_LIST_H

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

/*******************************************************************************
 * classes and functions
 */

namespace nw4hbm { namespace ut
{
	// [SC5PGN]/build/libs/Debug/slamWiiD.a:ut_list.o(1)::.debug_info::0xc0 [original object]
	struct List
	{
		void	*headObject;	// size 0x04, offset 0x00
		void	*tailObject;	// size 0x04, offset 0x04
		u16		numObjects;		// size 0x02, offset 0x08
		u16		offset;			// size 0x02, offset 0x0a
	}; // size 0x0c

	// [SC5PGN]/build/libs/Debug/slamWiiD.a:ut_list.o(1)::.debug_info::0x150 [original object]
	struct Link
	{
		void	*prevObject;	// size 0x04, offset 0x00
		void	*nextObject;	// size 0x04, offset 0x04
	}; // size 0x08

	void List_Init(List *list, u16 offset);

	void List_Append(List *list, void *object);
	void List_Prepend(List *list, void *object);
	void List_Insert(List *list, void *target, void *object);
	void List_Remove(List *list, void *object);

	void *List_GetNext(List const *list, void const *object);
	void *List_GetPrev(List const *list, void const *object);
	void *List_GetNth(List const *list, u16 index);

	inline void *List_GetFirst(List const *list)
	{
		return List_GetNext(list, nullptr);
	}

	inline u16 List_GetSize(List const *list)
	{
		return list->numObjects;
	}

	inline void const *List_GetNthConst(List const *list, u16 const index)
	{
		return List_GetNth(list, index);
	}
}} // namespace nw4hbm::ut

#endif // NW4HBM_UT_LIST_H
