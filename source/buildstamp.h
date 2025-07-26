#ifndef RVL_SDK_BUILDSTAMP_H
#define RVL_SDK_BUILDSTAMP_H

/*******************************************************************************
 * Local macro helpers
 */

#define RVLB_STR_(x)					#x
#define RVLB_STR(x)						RVLB_STR_(x)

#define RVLB_CONCAT5_(a, b, c, d, e)	a ## b ## c ## d ## e
#define RVLB_CONCAT5(a, b, c, d, e)		RVLB_CONCAT5_(a, b, c, d, e)

/*******************************************************************************
 * Library version strings
 */

/* If you just want to specify all the fields manually, use
 * REVOLUTION_LIB_VERSION_STRING_EX.
 */

#define REVOLUTION_LIB_BUILD_TYPE_DEBUG		2
#define REVOLUTION_LIB_BUILD_TYPE_RELEASE	3

#ifndef REVOLUTION_LIB_BUILD_TYPE
# if !defined(NDEBUG)
#  define REVOLUTION_LIB_BUILD_TYPE	REVOLUTION_LIB_BUILD_TYPE_DEBUG
# else
#  define REVOLUTION_LIB_BUILD_TYPE	REVOLUTION_LIB_BUILD_TYPE_RELEASE
# endif
#endif

// internal use (this header)
#if REVOLUTION_LIB_BUILD_TYPE == REVOLUTION_LIB_BUILD_TYPE_DEBUG
# define RVLB_INFIX_	DEBUG
# define RVLB_STRING_	"debug"
#elif REVOLUTION_LIB_BUILD_TYPE == REVOLUTION_LIB_BUILD_TYPE_RELEASE
# define RVLB_INFIX_	RELEASE
# define RVLB_STRING_	"release"
#endif

// clang-format off
#define REVOLUTION_LIB_VERSION_STRING_EX(main_lib_, lib_, build_type_, date_, time_, cwcc_, cwbuild_)	\
	"<< " main_lib_ " - " lib_ " \t" build_type_ " build: " date_ " " time_ " (" cwcc_ "_" cwbuild_ ") >>"

#define REVOLUTION_LIB_VERSION_STRING(main_lib_, lib_, build_type_, date_, time_)	\
	REVOLUTION_LIB_VERSION_STRING_EX(main_lib_, lib_, build_type_, date_, time_, RVLB_STR(__CWCC__), RVLB_STR(__CWBUILD__))
// clang-format on

// RVL_SDK

// clang-format off
#define RVL_SDK_LIB_BUILD_DATE(lib_)	\
	RVLB_CONCAT5(RVL_SDK_, lib_, _, RVLB_INFIX_, _BUILD_DATE)
#define RVL_SDK_LIB_BUILD_TIME(lib_)	\
	RVLB_CONCAT5(RVL_SDK_, lib_, _, RVLB_INFIX_, _BUILD_TIME)

#define RVL_SDK_LIB_VERSION_STRING_EX(lib_, cwcc_, cwbuild_)					\
	REVOLUTION_LIB_VERSION_STRING_EX("RVL_SDK", RVLB_STR(lib_), RVLB_STRING_,	\
		RVL_SDK_LIB_BUILD_DATE(lib_), RVL_SDK_LIB_BUILD_TIME(lib_), cwcc_,		\
		cwbuild_)

#define RVL_SDK_LIB_VERSION_STRING(lib_)										\
	REVOLUTION_LIB_VERSION_STRING("RVL_SDK", RVLB_STR(lib_), RVLB_STRING_,		\
		RVL_SDK_LIB_BUILD_DATE(lib_), RVL_SDK_LIB_BUILD_TIME(lib_))
// clang-format on

#endif // RVL_SDK_BUILDSTAMP_H
