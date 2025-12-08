#ifndef NW4HBM_UT_INLINES_H
#define NW4HBM_UT_INLINES_H

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

/*******************************************************************************
 * classes and functions
 */

namespace nw4hbm { namespace ut { namespace
{
	/* [SC5PGN]/build/libs/Debug/slamWiiD.a:HBMAnmController.o(1)::.debug_info::0x32e,
	 * [SC5PGN]/build/libs/Debug/slamWiiD.a:HBMGUIManager.o(1)::.debug_info::0x1086...
	 * [original objects]
	 */
	class NonCopyable
	{
	// methods
	public:
		// cdtors
		NonCopyable() {}
		~NonCopyable() {}

	// deleted methods
	private:
		NonCopyable(NonCopyable const &)/* = delete */;
		NonCopyable const &operator =(NonCopyable const &)/* = delete */;
	}; // empty

	template <typename T>
	inline T Min(T a, T b)
	{
		return a > b ? b : a;
	}

	template <typename T>
	inline T Max(T a, T b)
	{
		return a < b ? b : a;
	}

	template <typename T>
	inline T BitExtract(T bits, int pos, int len)
	{
		T mask = (1 << len) - 1;

		return bits >> pos & mask;
	}

	inline byte4_t ReverseEndian(byte4_t x)
	{
		return BitExtract(x, 0, 8) << 24 | BitExtract(x, 8, 8) << 16
		     | BitExtract(x, 16, 8) << 8 | BitExtract(x, 24, 8);
	}

	inline byte2_t ReverseEndian(byte2_t x)
	{
		return BitExtract(x, 0, 8) << 8 | BitExtract(x, 8, 8);
	}

	inline u32 GetIntPtr(void const *ptr)
	{
		return reinterpret_cast<u32>(ptr);
	}

	template <typename T>
	inline void *AddOffsetToPtr(void *ptr, T offset)
	{
		return reinterpret_cast<void *>(GetIntPtr(ptr) + offset);
	}
}}} // namespace nw4hbm::ut::(unnamed)

#endif // NW4HBM_UT_INLINES_H
