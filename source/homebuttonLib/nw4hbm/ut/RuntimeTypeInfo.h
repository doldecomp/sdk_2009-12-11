#ifndef NW4HBM_UT_RUNTIME_TYPE_INFO_H
#define NW4HBM_UT_RUNTIME_TYPE_INFO_H

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

/*******************************************************************************
 * classes and functions
 */

namespace nw4hbm { namespace ut
{
	namespace detail
	{
		// [SC5PGN]/build/libs/Debug/slamWiiD.a:HBMGUIManager.o(1)::.debug_info::0x1883 [original object]
		class RuntimeTypeInfo
		{
		// methods
		public:
			// cdtors
			RuntimeTypeInfo(RuntimeTypeInfo const *parent):
				mParentTypeInfo	(parent)
				{}

			/* ~RuntimeTypeInfo() = default; */

			// methods
			bool IsDerivedFrom(RuntimeTypeInfo const *typeInfo) const
			{
				RuntimeTypeInfo const *self;

				for (self = this; self; self = self->mParentTypeInfo)
				{
					if (self == typeInfo)
						return true;
				}

				return false;
			}

		// members
		private:
			RuntimeTypeInfo	const *mParentTypeInfo;	// size 0x04, offset 0x00
		}; // size 0x04

		template <class T>
		inline RuntimeTypeInfo const *GetTypeInfoFromPtr_(T *)
		{
			return &T::typeInfo;
		}
	} // namespace detail

	template <typename U, class T>
	inline U DynamicCast(T *obj)
	{
		detail::RuntimeTypeInfo const *typeInfoU =
			detail::GetTypeInfoFromPtr_(static_cast<U>(nullptr));

		if (obj->GetRuntimeTypeInfo()->IsDerivedFrom(typeInfoU))
			return reinterpret_cast<U>(obj);

		return nullptr;
	}
}} // namespace nw4hbm::ut

#endif // NW4HBM_UT_RUNTIME_TYPE_INFO_H
