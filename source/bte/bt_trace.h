#ifndef BT_TRACE_H
#define BT_TRACE_H

/*******************************************************************************
 * headers
 */

#include "data_types.h"

/*******************************************************************************
 * log functions
 */

#ifdef __cplusplus
	extern "C" {
#endif

// clang-format off
extern void LogMsg_0(UINT32 trace_set_mask, const char *p_str);
extern void LogMsg_1(UINT32 trace_set_mask, const char *fmt_str, UINT32 p1);
extern void LogMsg_2(UINT32 trace_set_mask, const char *fmt_str, UINT32 p1, UINT32 p2);
extern void LogMsg_3(UINT32 trace_set_mask, const char *fmt_str, UINT32 p1, UINT32 p2, UINT32 p3);
extern void LogMsg_4(UINT32 trace_set_mask, const char *fmt_str, UINT32 p1, UINT32 p2, UINT32 p3, UINT32 p4);
extern void LogMsg_5(UINT32 trace_set_mask, const char *fmt_str, UINT32 p1, UINT32 p2, UINT32 p3, UINT32 p4, UINT32 p5);
extern void LogMsg_6(UINT32 trace_set_mask, const char *fmt_str, UINT32 p1, UINT32 p2, UINT32 p3, UINT32 p4, UINT32 p5, UINT32 p6);
// clang-format on

#ifdef __cplusplus
	}
#endif

/*******************************************************************************
 * main macro work
 */

// clang-format off
#define BDLT_VA_APPLY_(m, _6, _5, _4, _3, _2, _1, _0, x, ...) m(x)
#define BDLT_VA_APPLY(...)	BDLT_VA_APPLY_(__VA_ARGS__, 6, 5, 4, 3, 2, 1, 0, _)

#define BDLT_LOG_MSG_(x)	LogMsg_ ## x
#define BDLT_LOG_MSG(...)	BDLT_VA_APPLY(BDLT_LOG_MSG_, __VA_ARGS__)

#define BDLT_CAST_ARGS_(x)	BDLT_CAST_ARGS_ ## x
#define BDLT_CAST_ARGS(...)	BDLT_VA_APPLY(BDLT_CAST_ARGS_, __VA_ARGS__)(__VA_ARGS__, _)

/* in an effort to make it O(1) over O(n) at the slight cost of kind of
 * duplicated macros (but a lot less than the original!)
 */
#define BDLT_CAST_ARGS_0(m, ...)							m
#define BDLT_CAST_ARGS_1(m, _1, ...)						m, ((UINT32)(_1))
#define BDLT_CAST_ARGS_2(m, _1, _2, ...)					m, ((UINT32)(_1)), ((UINT32)(_2))
#define BDLT_CAST_ARGS_3(m, _1, _2, _3, ...)				m, ((UINT32)(_1)), ((UINT32)(_2)), ((UINT32)(_3))
#define BDLT_CAST_ARGS_4(m, _1, _2, _3, _4, ...)			m, ((UINT32)(_1)), ((UINT32)(_2)), ((UINT32)(_3)), ((UINT32)(_4))
#define BDLT_CAST_ARGS_5(m, _1, _2, _3, _4, _5, ...)		m, ((UINT32)(_1)), ((UINT32)(_2)), ((UINT32)(_3)), ((UINT32)(_4)), ((UINT32)(_5))
#define BDLT_CAST_ARGS_6(m, _1, _2, _3, _4, _5, _6, ...)	m, ((UINT32)(_1)), ((UINT32)(_2)), ((UINT32)(_3)), ((UINT32)(_4)), ((UINT32)(_5)), ((UINT32)(_6))

#define BLUEDROID_LOG_TRACE(ctrl_, layer_, org_, type_, ...)	\
	BDLT_LOG_MSG(__VA_ARGS__)									\
	(															\
		TRACE_MASK(ctrl_, layer_, org_, type_),					\
		BDLT_CAST_ARGS(__VA_ARGS__)								\
	)
// clang-format on

/*******************************************************************************
 * Trace info
 */

#define BT_TRACE_LEVEL_NONE			0	/* No trace messages to be generated    */
#define BT_TRACE_LEVEL_ERROR		1	/* Error condition trace messages       */
#define BT_TRACE_LEVEL_WARNING		2	/* Warning condition trace messages     */
#define BT_TRACE_LEVEL_API			3	/* API traces                           */
#define BT_TRACE_LEVEL_EVENT		4	/* Debug messages for events            */
#define BT_TRACE_LEVEL_DEBUG		5	/* Full debug messages                  */
#define BT_MAX_TRACE_LEVEL			BT_TRACE_LEVEL_DEBUG

// clang-format off
/* TRACE_CTRL_TYPE					0x^^000000 */
#define TRACE_CTRL_MASK				0xff000000
#define TRACE_GET_CTRL(x)			((((UINT32)(x)) & TRACE_CTRL_MASK) >> 24)
#define TRACE_CTRL(x)				((((UINT32)(x)) << 24) & TRACE_CTRL_MASK)

#define TRACE_CTRL_GENERAL			0
#define TRACE_CTRL_MAX_NUM			3

/* LAYER SPECIFIC					0x00^^0000 */
#define TRACE_LAYER_MASK			0x00ff0000
#define TRACE_GET_LAYER(x)			((((UINT32)(x)) & TRACE_LAYER_MASK) >> 16)
#define TRACE_LAYER(x)				((((UINT32)(x)) << 16) & TRACE_LAYER_MASK)

#define TRACE_LAYER_NONE			0
#define TRACE_LAYER_XML				16
#define TRACE_LAYER_MAX_NUM			49

/* TRACE_ORIGINATOR					0x0000^^00 */
#define TRACE_ORG_MASK				0x0000ff00
#define TRACE_GET_ORG(x)			((((UINT32)(x)) & TRACE_ORG_MASK) >> 8)
#define TRACE_ORG(x)				((((UINT32)(x)) << 8) & TRACE_ORG_MASK)

#define TRACE_ORG_STACK				0
#define TRACE_ORG_APPL				5
#define TRACE_ORG_MAX_NUM			10

/* TRACE_TYPE						0x000000^^ */
#define TRACE_TYPE_MASK				0x000000ff
#define TRACE_GET_TYPE(x)			((((UINT32)(x)) & TRACE_TYPE_MASK) >> 0)
#define TRACE_TYPE(x)				((((UINT32)(x)) << 0) & TRACE_TYPE_MASK)

#define TRACE_TYPE_ERROR			0
#define TRACE_TYPE_WARNING			1
#define TRACE_TYPE_API				2
#define TRACE_TYPE_EVENT			3
#define TRACE_TYPE_DEBUG			4
#define TRACE_TYPE_MAX_NUM			20

#define TRACE_MASK(ctrl, layer, org, type)	\
	( TRACE_CTRL(TRACE_CTRL_ ## ctrl)		\
	| TRACE_LAYER(TRACE_LAYER_ ## layer)	\
	| TRACE_ORG(TRACE_ORG_ ## org)			\
	| TRACE_TYPE(TRACE_TYPE_ ## type))
// clang-format on

/*******************************************************************************
 * trace macros
 */

// clang-format off

#define APPL_TRACE(type_, ...)	do { if (appl_trace_level >= BT_TRACE_LEVEL_ ## type_) BLUEDROID_LOG_TRACE(GENERAL, NONE, APPL, type_, __VA_ARGS__); } while (FALSE)
#define XML_TRACE(type_, ...)	BLUEDROID_LOG_TRACE(GENERAL, XML, STACK, type_, __VA_ARGS__)

// clang-format on

#endif // BT_TRACE_H
