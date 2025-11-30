#ifndef CONTEXT_RVL_H
#define CONTEXT_RVL_H

#include <macros.h>

#include <revolution/types.h>

#include <context_bte.h>

#ifdef __cplusplus
	extern "C" {
#endif

/* Contains the context of the other Revolution SDK libraries that the WPAD
 * library needs to compile.
 *
 * This is not the full context; the other half of the context is in
 * <context_bte.h>.
 *
 * Ideally, this file's usages should be replaced with the headers in your
 * project that declare these symbols.
 */

#if !defined(NDEBUG)
# define OSAssertMessage_FileLine(file_, line_, exp_, ...)	\
	(void)((exp_) || (OSPanic(file_, line_, __VA_ARGS__), 0))
#else
# define OSAssertMessage_FileLine(file_, line_, exp_, ...)	\
	((void)0)
#endif

// defined in terms of OSAssertMessage_FileLine
#define OSAssertMessage_Line(line_, exp_, ...)	\
	OSAssertMessage_FileLine(__FILE__, line_, exp_, __VA_ARGS__)
#define OSAssert_Line(line_, exp_)	\
	OSAssertMessage_FileLine(__FILE__, line_, exp_, "Failed assertion " #exp_)

// OSError
#define OSError_FileLine(file_, line_, ...)	\
	OSPanic(file_, line_, __VA_ARGS__)

// defined in terms of OSError_FileLine
#define OSError_Line(line_, ...)	\
	OSError_FileLine(__FILE__, line_, __VA_ARGS__)

// OS

typedef s64 OSTime;
typedef u32 OSTick;

typedef u8 OSAppType;
enum OSAppType_et
{
	OS_APP_TYPE_IPL		= 0x40,
	OS_APP_TYPE_DVD		= 0x80,
	OS_APP_TYPE_CHANNEL	= 0x81,
};

extern BOOL __OSInIPL;
extern OSTime __OSStartTime;

void OSRegisterVersion(char const *version);
char const *OSGetAppGamename(void);
OSAppType OSGetAppType(void);

#if defined(NDEBUG)
# define OSPhysicalToCached(addr)	(void *)((u32)(addr) + 0x80000000)
# define OSPhysicalToUncached(addr)	(void *)((u32)(addr) + 0xC0000000)
# define OSCachedToPhysical(addr)	(void *)((u32)(addr) - 0x80000000)
# define OSUncachedToPhysical(addr)	(void *)((u32)(addr) - 0xC0000000)
#endif // defined(NDEBUG)

void *(OSPhysicalToCached)(void *addr);
void *(OSPhysicalToUncached)(void *addr);
void *(OSCachedToPhysical)(void *addr);
void *(OSUncachedToPhysical)(void *addr);

typedef struct OSAlarm OSAlarm;
typedef struct OSContext OSContext;

typedef void OSAlarmHandler(OSAlarm *alarm, OSContext *context);

// [SPQE7T]/ISpyD.elf:.debug_info::0x2950d7
struct OSAlarm
{
	OSAlarmHandler	*handler;	// size 0x04, offset 0x00
	u32				tag;		// size 0x04, offset 0x04
	OSTime			fire;		// size 0x08, offset 0x08
	OSAlarm			*prev;		// size 0x04, offset 0x10
	OSAlarm			*next;		// size 0x04, offset 0x14
	OSTime			period;		// size 0x08, offset 0x18
	OSTime			start;		// size 0x08, offset 0x20
	void			*userData;	// size 0x04, offset 0x28
	/* 4 bytes padding */
}; // size 0x30

void OSCreateAlarm(OSAlarm *alarm);
void OSSetAlarm(OSAlarm *alarm, OSTime tick, OSAlarmHandler handler);
void OSSetPeriodicAlarm(OSAlarm *alarm, OSTime tick, OSTime period,
                        OSAlarmHandler handler);
void OSCancelAlarm(OSAlarm *alarm);
void OSSetAlarmUserData(OSAlarm *alarm, void *userData);
void *OSGetAlarmUserData(const OSAlarm *alarm);

#define OSGetAlarmUserDataAny(type_, alarm_)	\
	((type_)(OSGetAlarmUserData(alarm_)))

#define OSSetAlarmUserDataAny(alarm_, data_)	\
	OSSetAlarmUserData(alarm_, (void *)(data_))

void DCFlushRange(void const *buf, u32 len);
void DCInvalidateRange(void const *buf, u32 len);
void ICInvalidateRange(void const *buf, u32 len);

typedef void OSFiber(/* unspecified */);

// [SPQE7T]/ISpyD.elf:.debug_info::0x3135
struct OSContext
{
	register_t	gpr[32];			// size 0x080, offset 0x000
	register_t	cr;					// size 0x004, offset 0x080
	register_t	lr;					// size 0x004, offset 0x084
	register_t	ctr;				// size 0x004, offset 0x088
	register_t	xer;				// size 0x004, offset 0x08c
	f64			fpr[32];			// size 0x100, offset 0x090
	u32			fpscr_pad;			// size 0x004, offset 0x190
	register_t	fpscr;				// size 0x004, offset 0x194
	register_t	srr0;				// size 0x004, offset 0x198
	register_t	srr1;				// size 0x004, offset 0x19c
	u16			mode;				// size 0x002, offset 0x1a0
	u16			state;				// size 0x002, offset 0x1a2
	register_t	gqr[8];				// size 0x020, offset 0x1a4
	u32			psf_pad;			// size 0x004, offset 0x1c4
	f64			psf[32];			// size 0x100, offset 0x1c8
}; // size 0x2c8

void OSSwitchFiberEx(register_t arg0, register_t arg1, register_t arg2,
                     register_t arg3, OSFiber *fiber, void *stack);

byte4_t OSCalcCRC32(void const *data, u32 length);

ATTR_WEAK void OSReport(char const *msg, ...);
ATTR_WEAK void OSPanic(char const *file, int line, char const *msg, ...);

// Initialization
static inline void OSInitFastCast(void)
{
	// TODO: name (verb)

#if defined(__MWERKS__)
	asm // function-level asm not allowed for functions marked inline
	{
		// GQR2 = u8
		li		r3, 4
		oris	r3, r3, 4
		mtspr	0x392, r3

		// GQR3 = u16
		li		r3, 5
		oris	r3, r3, 5
		mtspr	0x393, r3

		// GQR4 = s8
		li		r3, 6
		oris	r3, r3, 6
		mtspr	0x394, r3

		// GQR5 = s16
		li		r3, 7
		oris	r3, r3, 7
		mtspr	0x395, r3
	}
#endif
}

static inline u8 __OSf32tou8(register f32 in)
{
	f32 a;
	register f32 *ptr = &a;
	u8 r;

#if defined(__MWERKS__)
	asm { psq_st in, 0(ptr), true, 2 };
#else
# pragma unused(in)
#endif

	r = *(u8 *)ptr;

	return r;
}

static inline u16 __OSf32tou16(register f32 in)
{
	f32 a;
	register f32 *ptr = &a;
	u16 r;

#if defined(__MWERKS__)
	asm { psq_st in, 0(ptr), true, 3 };
#else
# pragma unused(in)
#endif

	r = *(u16 *)ptr;

	return r;
}

static inline s16 __OSf32tos16(register f32 in)
{
	f32 a;
	register f32 *ptr = &a;
	s16 r;

#if defined(__MWERKS__)
	asm { psq_st in, 0(ptr), true, 5 };
#else
# pragma unused(in)
#endif

	r = *(s16 *)ptr;

	return r;
}

static inline f32 __OSu16tof32(register u16 const *arg)
{
	register f32 ret;

#if defined(__MWERKS__)
	asm { psq_l ret, 0(arg), true, 3 };
#else
# pragma unused(arg)
	ret = 0;
#endif

	return ret;
}

static inline f32 __OSs16tof32(register s16 const *arg)
{
	register f32 ret;

#if defined(__MWERKS__)
	asm { psq_l ret, 0(arg), true, 5 };
#else
# pragma unused(arg)
	ret = 0;
#endif

	return ret;
}

static inline void OSf32tou8(f32 const *in, u8 *out)
{
	*out = __OSf32tou8(*in);
}

static inline void OSf32tou16(f32 const *in, u16 *out)
{
	*out = __OSf32tou16(*in);
}

static inline void OSf32tos16(f32 const *in, s16 *out)
{
	*out = __OSf32tos16(*in);
}

static inline void OSs16tof32(s16 const *in, f32 *out)
{
	*out = __OSs16tof32(in);
}

static inline void OSu16tof32(u16 const *in, f32 *out)
{
	*out = __OSu16tof32(in);
}

#define OS_FONT_SIZE_ANSI	0x020120
#define OS_FONT_SIZE_SJIS	0x120f00

typedef u16 OSFontEncode;
enum OSFontEncode_et
{
	OS_FONT_ENCODE_ANSI,
	OS_FONT_ENCODE_SJIS,
	OS_FONT_ENCODE_2,
	OS_FONT_ENCODE_UTF8,
	OS_FONT_ENCODE_UTF16,
	OS_FONT_ENCODE_UTF32,

	OS_FONT_ENCODE_MAX,

	OS_FONT_ENCODE_INVALID = -1
};

// [SC5PGN]/build/libs/Debug/slamWiiD.a:ut_RomFont.o(1)::.debug_info::0x296 [original object]
typedef struct OSFontHeader
{
	u16	fontType;		// size 0x02, offset 0x00
	u16	firstChar;		// size 0x02, offset 0x02
	u16	lastChar;		// size 0x02, offset 0x04
	u16	invalChar;		// size 0x02, offset 0x06
	u16	ascent;			// size 0x02, offset 0x08
	u16	descent;		// size 0x02, offset 0x0a
	u16	width;			// size 0x02, offset 0x0c
	u16	leading;		// size 0x02, offset 0x0e
	u16	cellWidth;		// size 0x02, offset 0x10
	u16	cellHeight;		// size 0x02, offset 0x12
	u32	sheetSize;		// size 0x04, offset 0x14
	u16	sheetFormat;	// size 0x02, offset 0x18
	u16	sheetColumn;	// size 0x02, offset 0x1a
	u16	sheetRow;		// size 0x02, offset 0x1c
	u16	sheetWidth;		// size 0x02, offset 0x1e
	u16	sheetHeight;	// size 0x02, offset 0x20
	u16	widthTable;		// size 0x02, offset 0x22
	u32	sheetImage;		// size 0x04, offset 0x24
	u32	sheetFullSize;	// size 0x04, offset 0x28
	u8	c0;				// size 0x01, offset 0x2c
	u8	c1;				// size 0x01, offset 0x2d
	u8	c2;				// size 0x01, offset 0x2e
	u8	c3;				// size 0x01, offset 0x2f

} OSFontHeader; // size 0x30

BOOL OSInitFont(OSFontHeader *font);
OSFontEncode OSGetFontEncode(void);
char const *OSGetFontWidth(char const *str, s32 *widthOut);
char const *OSGetFontTexture(char const *str, void **texOut, s32 *xOut,
                             s32 *yOut, s32 *widthOut);

BOOL OSDisableInterrupts(void);
BOOL OSRestoreInterrupts(BOOL status);

typedef struct OSMutex OSMutex;

// [SPQE7T]/ISpyD.elf:.debug_info::0x333a
typedef struct OSMutexLink
{
	OSMutex	*next;	// size 0x04, offset 0x00
	OSMutex	*prev;	// size 0x04, offset 0x04
} OSMutexLink; // size 0x08

typedef int OSShutdownPass;
enum OSShutdownPass_et
{
	OS_SHUTDOWN_PASS_FIRST,
	OS_SHUTDOWN_PASS_SECOND,
};

typedef u32 OSShutdownEvent;
enum OSShutdownEvent_et
{
	OS_SHUTDOWN_EVENT_FATAL,
	OS_SHUTDOWN_EVENT_1,
	OS_SHUTDOWN_EVENT_SHUTDOWN,
	OS_SHUTDOWN_EVENT_3,
	OS_SHUTDOWN_EVENT_RESTART,
	OS_SHUTDOWN_EVENT_RETURN_TO_MENU,
	OS_SHUTDOWN_EVENT_LAUNCH_APP,
};

typedef BOOL OSShutdownFunction(OSShutdownPass pass, OSShutdownEvent event);

typedef struct OSShutdownFunctionInfo OSShutdownFunctionInfo;
struct OSShutdownFunctionInfo
{
	OSShutdownFunction		*func;	// size 0x04, offset 0x00	/* name known from asserts */
	u32						prio;	// size 0x04, offset 0x04
	OSShutdownFunctionInfo	*next;	// size 0x04, offset 0x08
	OSShutdownFunctionInfo	*prev;	// size 0x04, offset 0x0c
}; // size 0x10

typedef struct OSShutdownFunctionQueue
{
	OSShutdownFunctionInfo	*head;	// size 0x04, offset 0x00
	OSShutdownFunctionInfo	*tail;	// size 0x04, offset 0x04
} OSShutdownFunctionQueue; // size 0x08

extern BOOL __OSIsReturnToIdle;

void OSRegisterShutdownFunction(OSShutdownFunctionInfo *info);

#define OSSendMessageAny(msgQueue_, msg_, flags_)	\
	OSSendMessage(msgQueue_, (OSMessage)(msg_), flags_)

#define OSReceiveMessageAny(msgQueue_, msgOut_, flags_)	\
	OSReceiveMessage(msgQueue_, (OSMessage *)(msgOut_), flags_)

#define OSJamMessageAny(msgQueue_, msg_, flags_)	\
	OSJamMessage(msgQueue_, (OSMessage)(msg_), flags_)

#define OSReadMessageAs(msg_, type_)	\
	((type_)(msg_))

typedef void *OSMessage;

typedef u32 OSMessageFlags;
enum OSMessageFlags_et
{
	OS_MESSAGE_NO_FLAGS			= 0,

	OS_MESSAGE_FLAG_PERSISTENT	= 1 << 0,
};

typedef struct OSThread OSThread;

typedef enum OSThreadState
{
	OS_THREAD_STATE_EXITED		= 0,
	OS_THREAD_STATE_READY		= 1 << 0,
	OS_THREAD_STATE_RUNNING		= 1 << 1,
	OS_THREAD_STATE_SLEEPING	= 1 << 2,
	OS_THREAD_STATE_MORIBUND	= 1 << 3,
} OSThreadState;

typedef u16 OSThreadFlags;
enum OSThreadFlags_et
{
	OS_THREAD_NO_FLAGS	= 0,

	OS_THREAD_DETACHED	= 1 << 0,
};

typedef void OSSwitchThreadCallback(OSThread *curThread, OSThread *newThread);
typedef void *OSThreadFunction(void *arg);

// [SPQE7T]/ISpyD.elf:.debug_info::0x2fb2
typedef struct OSThreadQueue
{
	OSThread	*head;	// size 0x04, offset 0x00
	OSThread	*tail;	// size 0x04, offset 0x04
} OSThreadQueue; // size 0x08

// [SPQE7T]/ISpyD.elf:.debug_info::0x328b
typedef struct OSThreadLink
{
	OSThread	*next;	// size 0x04, offset 0x00
	OSThread	*prev;	// size 0x04, offset 0x04
} OSThreadLink; // size 0x08

// [SPQE7T]/ISpyD.elf:.debug_info::0x32cf
typedef struct OSMutexQueue
{
	OSMutex	*head;	// size 0x04, offset 0x00
	OSMutex	*tail;	// size 0x04, offset 0x04
} OSMutexQueue; // size 0x08

// [SPQE7T]/ISpyD.elf:.debug_info::0x2feb
struct OSThread
{
	OSContext		context;		// size 0x2c8, offset 0x000
	u16				state;			// size 0x002, offset 0x2c8
	u16				attr;			// size 0x002, offset 0x2ca
	s32				suspend;		// size 0x004, offset 0x2cc
	s32				priority;		// size 0x004, offset 0x2d0
	s32				base;			// size 0x004, offset 0x2d4
	void			*val;			// size 0x004, offset 0x2d8
	OSThreadQueue	*queue;			// size 0x004, offset 0x2dc
	OSThreadLink	link;			// size 0x008, offset 0x2e0
	OSThreadQueue	queueJoin;		// size 0x008, offset 0x2e8
	OSMutex			*mutex;			// size 0x004, offset 0x2f0
	OSMutexQueue	queueMutex;		// size 0x008, offset 0x2f4
	OSThreadLink	linkActive;		// size 0x008, offset 0x2fc
	u8				*stackBase;		// size 0x004, offset 0x304
	u32				*stackEnd;		// size 0x004, offset 0x308
	s32				error;			// size 0x004, offset 0x30c
	void			*specific[2];	// size 0x008, offset 0x310
}; // size 0x318

void OSInitThreadQueue(OSThreadQueue *queue);
BOOL OSCreateThread(OSThread *thread, OSThreadFunction *func, void *funcArg,
                   void *stackBegin, u32 stackSize, int prio,
                   OSThreadFlags flags);
BOOL OSJoinThread(OSThread *thread, void *val);
s32 OSResumeThread(OSThread *thread);
void OSSleepThread(OSThreadQueue *queue);
void OSWakeupThread(OSThreadQueue *queue);

// [SPQE7T]/ISpyD.elf:.debug_info::0x23508c
typedef struct OSMessageQueue
{
	OSThreadQueue	queueSend;		// size 0x08, offset 0x00
	OSThreadQueue	queueReceive;	// size 0x08, offset 0x08
	OSMessage		*msgArray;		// size 0x04, offset 0x10
	s32				msgCount;		// size 0x04, offset 0x14
	s32				firstIndex;		// size 0x04, offset 0x18
	s32				usedCount;		// size 0x04, offset 0x1c
} OSMessageQueue; // size 0x20

void OSInitMessageQueue(OSMessageQueue *msgQueue, OSMessage *buffer,
                        int capacity);
BOOL OSSendMessage(OSMessageQueue *msgQueue, OSMessage msg,
                   OSMessageFlags flags);
BOOL OSReceiveMessage(OSMessageQueue *msgQueue, OSMessage *msgOut,
                      OSMessageFlags flags);
BOOL OSJamMessage(OSMessageQueue *msgQueue, OSMessage msg,
                  OSMessageFlags flags);

// [SPQE7T]/ISpyD.elf:.debug_info::0x2f63
struct OSMutex
{
	OSThreadQueue	queue;		// size 0x08, offset 0x00
	OSThread		*thread;	// size 0x04, offset 0x08
	s32				count;		// size 0x04, offset 0x0c
	OSMutexLink		link;		// size 0x08, offset 0x10
}; // size 0x18

void OSInitMutex(OSMutex *mutex);

extern u32 __OSBusClock AT_ADDRESS(0x800000f8);

#define OS_TIMER_CLOCK	(__OSBusClock / 4)

// clang-format off
#define OSTicksToSeconds(ticks)			((ticks)        / (OS_TIMER_CLOCK         )       )
#define OSTicksToMilliseconds(ticks)	((ticks)        / (OS_TIMER_CLOCK / 1000  )       )
#define OSTicksToMicroseconds(ticks)	((ticks) * 8    / (OS_TIMER_CLOCK / 125000)       )
#define OSTicksToNanoseconds(ticks)		((ticks) * 8000 / (OS_TIMER_CLOCK / 125000)       )

#define OSSecondsToTicks(sec)			((  sec)        * (OS_TIMER_CLOCK         )       )
#define OSMillisecondsToTicks(msec)		(( msec)        * (OS_TIMER_CLOCK / 1000  )       )
#define OSMicrosecondsToTicks(usec)		(( usec)        * (OS_TIMER_CLOCK / 125000) / 8   )
#define OSNanosecondsToTicks(nsec)		(( nsec)        * (OS_TIMER_CLOCK / 125000) / 8000)

#define OSDiffTick(tick1, tick0)		((OSTick)(tick1) - (OSTick)(tick0))
// clang-format on

OSTime OSGetTime(void);
OSTick OSGetTick(void);
OSTime __OSGetSystemTime(void);

// TODO: is there a way to make this work with an object with address declaration?
#define CURRENT_AFH_CHANNEL_PHYSICAL_ADDR ((void *)(0x31a2))

// AI

BOOL AICheckInit(void);
void AIInit(void *stack);

// ARC

typedef s32 ARCEntryNum;
#define ARC_ENTRY_NUM_INVALID	(-1)

// [SPQE7T]/ISpyD.elf:.debug_info::0x119a90
typedef struct ARCHandle
{
	void		*archiveStartAddr;	// size 0x04, offset 0x00
	void		*FSTStart;			// size 0x04, offset 0x04
	void		*fileStart;			// size 0x04, offset 0x08
	u32			entryNum;			// size 0x04, offset 0x0c // more accurately entryCount
	const char	*FSTStringStart;	// size 0x04, offset 0x10
	u32			FSTLength;			// size 0x04, offset 0x14
	u32			currDir;			// size 0x04, offset 0x18 // more accurately what ARCDir calls entryNum
} ARCHandle; // size 0x1c

// [SPQE7T]/ISpyD.elf:.debug_info::0x368e0b
typedef struct ARCFileInfo
{
	ARCHandle	*handle;		// size 0x04, offset 0x00
	u32			startOffset;	// size 0x04, offset 0x04
	u32			length;			// size 0x04, offset 0x08
} ARCFileInfo; // size 0x0c

// [SPQE7T]/ISpyD.elf:.debug_info::0x3749e4
typedef struct ARCDir
{
	ARCHandle	*handle;	// size 0x04, offset 0x00
	u32			entryNum;	// size 0x04, offset 0x04
	u32			location;	// size 0x04, offset 0x08
	u32			next;		// size 0x04, offset 0x0c
} ARCDir; // size 0x10

// [SPQE7T]/ISpyD.elf:.debug_info::0x374a3e
typedef struct ARCDirEntry
{
	ARCHandle	*handle;	// size 0x04, offset 0x00
	u32			entryNum;	// size 0x04, offset 0x04
	BOOL		isDir;		// size 0x04, offset 0x08
	const char	*name;		// size 0x04, offset 0x0c
} ARCDirEntry; // size 0x10

BOOL ARCInitHandle(void *bin, ARCHandle *handle);
BOOL ARCOpen(ARCHandle *handle, const char *filename, ARCFileInfo *af);
BOOL ARCFastOpen(ARCHandle *handle, int entrynum, ARCFileInfo *af);
s32 ARCConvertPathToEntrynum(ARCHandle *handle, const char *path);
void *ARCGetStartAddrInMem(ARCFileInfo *af);
u32 ARCGetLength(ARCFileInfo *af);
BOOL ARCClose(ARCFileInfo *af);
BOOL ARCChangeDir(ARCHandle *handle, const char *dirname);
BOOL ARCOpenDir(ARCHandle *handle, const char *dirname, ARCDir *dir);
BOOL ARCReadDir(ARCDir *dir, ARCDirEntry *direntry);
BOOL ARCCloseDir(ARCDir *dir);

// AX

#define AX_PRIORITY_STACKS	32	/* name known from asserts */

#define AX_PRIORITY_MAX		(AX_PRIORITY_STACKS - 1)

// [SPQE7T]/ISpyD.elf:.debug_info::0x36a6f8
typedef void AXFrameCallback(void);

// [SPQE7T]/ISpyD.elf:.debug_info::0x36c40e
typedef void AXAuxCallback(void *data, void *context);

typedef enum AXMixerCtrlFlags
{
	AX_MIXER_CTRL_L			= 1 << 0,
	AX_MIXER_CTRL_R			= 1 << 1,
	AX_MIXER_CTRL_DELTA		= 1 << 2,
	AX_MIXER_CTRL_S			= 1 << 3,
	AX_MIXER_CTRL_DELTA_S	= 1 << 4,

	AX_MIXER_CTRL_A_L		= 1 << 16,
	AX_MIXER_CTRL_A_R		= 1 << 17,
	AX_MIXER_CTRL_A_DELTA	= 1 << 18,
	AX_MIXER_CTRL_A_S		= 1 << 19,
	AX_MIXER_CTRL_A_DELTA_S	= 1 << 20,

	AX_MIXER_CTRL_B_L		= 1 << 21,
	AX_MIXER_CTRL_B_R		= 1 << 22,
	AX_MIXER_CTRL_B_DELTA	= 1 << 23,
	AX_MIXER_CTRL_B_S		= 1 << 24,
	AX_MIXER_CTRL_B_DELTA_S	= 1 << 25,

	AX_MIXER_CTRL_C_L		= 1 << 26,
	AX_MIXER_CTRL_C_R		= 1 << 27,
	AX_MIXER_CTRL_C_DELTA	= 1 << 28,
	AX_MIXER_CTRL_C_S		= 1 << 29,
	AX_MIXER_CTRL_C_DELTA_S	= 1 << 30,
} AXMixerCtrlFlags;

typedef u32 AXVPBSrcType;
enum AXVPBSrcType_et
{
	AX_SRC_TYPE_0,
	AX_SRC_TYPE_1,
	AX_SRC_TYPE_2,
	AX_SRC_TYPE_3,
	AX_SRC_TYPE_4TAP_16K,	/* name known from asserts */
};

typedef u32 AXVPBSyncFlags;
enum AXVPBSyncFlags_et
{
	AX_VPB_SYNC_FLAG_SRC_TYPE				= 1 <<  0,
	AX_VPB_SYNC_FLAG_MIXER_CTRL				= 1 <<  1,
	AX_VPB_SYNC_FLAG_STATE					= 1 <<  2,
	AX_VPB_SYNC_FLAG_TYPE					= 1 <<  3,
	AX_VPB_SYNC_FLAG_MIX					= 1 <<  4,
	AX_VPB_SYNC_FLAG_ITD					= 1 <<  5,
	AX_VPB_SYNC_FLAG_ITD_TARGET				= 1 <<  6,
	AX_VPB_SYNC_FLAG_DPOP					= 1 <<  7,
	AX_VPB_SYNC_FLAG_VE						= 1 <<  8,
	AX_VPB_SYNC_FLAG_VE_DELTA				= 1 <<  9,
	AX_VPB_SYNC_FLAG_ADDR					= 1 << 10,
	AX_VPB_SYNC_FLAG_ADDR_LOOP_FLAG			= 1 << 11,
	AX_VPB_SYNC_FLAG_ADDR_LOOP_ADDR			= 1 << 12,
	AX_VPB_SYNC_FLAG_ADDR_END_ADDR			= 1 << 13,
	AX_VPB_SYNC_FLAG_ADDR_CURRENT_ADDR		= 1 << 14,
	AX_VPB_SYNC_FLAG_ADPCM					= 1 << 15,
	AX_VPB_SYNC_FLAG_SRC					= 1 << 16,
	AX_VPB_SYNC_FLAG_SRC_RATIO				= 1 << 17,
	AX_VPB_SYNC_FLAG_ADPCM_LOOP				= 1 << 18,
	AX_VPB_SYNC_FLAG_LPF					= 1 << 19,
	AX_VPB_SYNC_FLAG_LPF_COEFS				= 1 << 20,
	AX_VPB_SYNC_FLAG_BIQUAD					= 1 << 21,
	AX_VPB_SYNC_FLAG_BIQUAD_COEFS			= 1 << 22,
	AX_VPB_SYNC_FLAG_RMT_ON					= 1 << 23,
	AX_VPB_SYNC_FLAG_RMT_MIXER_CTRL			= 1 << 24,
	AX_VPB_SYNC_FLAG_RMT_MIX				= 1 << 25,
	AX_VPB_SYNC_FLAG_RMT_DPOP				= 1 << 26,
	AX_VPB_SYNC_FLAG_RMT_SRC				= 1 << 27,
	AX_VPB_SYNC_FLAG_RMT_IIR				= 1 << 28,
	AX_VPB_SYNC_FLAG_RMT_IIR_LPF_COEFS		= 1 << 29,
	AX_VPB_SYNC_FLAG_RMT_IIR_BIQUAD_COEFS	= 1 << 30,
	AX_VPB_SYNC_FLAG_FULL_PB				= 1 << 31
};

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82853
typedef void AXVPBCallback(void *p);

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82a39
typedef struct _AXPBMIX
{
	u16	vL;				// size 0x02, offset 0x00
	u16	vDeltaL;		// size 0x02, offset 0x02
	u16	vR;				// size 0x02, offset 0x04
	u16	vDeltaR;		// size 0x02, offset 0x06
	u16	vAuxAL;			// size 0x02, offset 0x08
	u16	vDeltaAuxAL;	// size 0x02, offset 0x0a
	u16	vAuxAR;			// size 0x02, offset 0x0c
	u16	vDeltaAuxAR;	// size 0x02, offset 0x0e
	u16	vAuxBL;			// size 0x02, offset 0x10
	u16	vDeltaAuxBL;	// size 0x02, offset 0x12
	u16	vAuxBR;			// size 0x02, offset 0x14
	u16	vDeltaAuxBR;	// size 0x02, offset 0x16
	u16	vAuxCL;			// size 0x02, offset 0x18
	u16	vDeltaAuxCL;	// size 0x02, offset 0x1a
	u16	vAuxCR;			// size 0x02, offset 0x1c
	u16	vDeltaAuxCR;	// size 0x02, offset 0x1e
	u16	vS;				// size 0x02, offset 0x10
	u16	vDeltaS;		// size 0x02, offset 0x12
	u16	vAuxAS;			// size 0x02, offset 0x14
	u16	vDeltaAuxAS;	// size 0x02, offset 0x16
	u16	vAuxBS;			// size 0x02, offset 0x18
	u16	vDeltaAuxBS;	// size 0x02, offset 0x1a
	u16	vAuxCS;			// size 0x02, offset 0x1c
	u16	vDeltaAuxCS;	// size 0x02, offset 0x1e
} AXPBMIX; // size 0x30

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82c05
typedef struct _AXPBITD
{
	u16	flag;			// size 0x02, offset 0x00
	u16	bufferHi;		// size 0x02, offset 0x02
	u16	bufferLo;		// size 0x02, offset 0x04
	u16	shiftL;			// size 0x02, offset 0x06
	u16	shiftR;			// size 0x02, offset 0x08
	u16	targetShiftL;	// size 0x02, offset 0x0a
	u16	targetShiftR;	// size 0x02, offset 0x0c
} AXPBITD; // size 0x0e

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82c9a
typedef struct _AXPBDPOP
{
	s16	aL;		// size 0x02, offset 0x00
	s16	aAuxAL;	// size 0x02, offset 0x02
	s16	aAuxBL;	// size 0x02, offset 0x04
	s16	aAuxCL;	// size 0x02, offset 0x06
	s16	aR;		// size 0x02, offset 0x08
	s16	aAuxAR;	// size 0x02, offset 0x0a
	s16	aAuxBR;	// size 0x02, offset 0x0c
	s16	aAuxCR;	// size 0x02, offset 0x0e
	s16	aS;		// size 0x02, offset 0x10
	s16	aAuxAS;	// size 0x02, offset 0x12
	s16	aAuxBS;	// size 0x02, offset 0x14
	s16	aAuxCS;	// size 0x02, offset 0x16
} AXPBDPOP; // size 0x18

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82d6b
typedef struct _AXPBVE
{
	u16	currentVolume;	// size 0x02, offset 0x00
	s16	currentDelta;	// size 0x02, offset 0x02
} AXPBVE; // size 0x04

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82da9
typedef struct _AXPBADDR
{
	u16	loopFlag;			// size 0x02, offset 0x00
	u16	format;				// size 0x02, offset 0x02
	u16	loopAddressHi;		// size 0x02, offset 0x04
	u16	loopAddressLo;		// size 0x02, offset 0x06
	u16	endAddressHi;		// size 0x02, offset 0x08
	u16	endAddressLo;		// size 0x02, offset 0x0a
	u16	currentAddressHi;	// size 0x02, offset 0x0c
	u16	currentAddressLo;	// size 0x02, offset 0x0e
} AXPBADDR; // size 0x10

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x825d6
typedef struct _AXPBADPCM
{
	u16	a[8][2];	// size 0x20, offset 0x00
	u16	gain;		// size 0x02, offset 0x20
	u16	pred_scale;	// size 0x02, offset 0x22
	u16	yn1;		// size 0x02, offset 0x24
	u16	yn2;		// size 0x02, offset 0x26
} AXPBADPCM; // size 0x28

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82e72
typedef struct _AXPBSRC
{
	u16	ratioHi;			// size 0x02, offset 0x00
	u16	ratioLo;			// size 0x02, offset 0x02
	u16	currentAddressFrac;	// size 0x02, offset 0x04
	u16	last_samples[4];	// size 0x08, offset 0x06
} AXPBSRC; // size 0x0e

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82652
typedef struct _AXPBADPCMLOOP
{
	u16	loop_pred_scale;	// size 0x02, offset 0x00
	u16	loop_yn1;			// size 0x02, offset 0x02
	u16	loop_yn2;			// size 0x02, offset 0x04
} AXPBADPCMLOOP; // size 0x06

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82ee9
typedef struct _AXPBLPF
{
	u16	on;		// size 0x02, offset 0x00
	u16	yn1;	// size 0x02, offset 0x02
	u16	a0;		// size 0x02, offset 0x04
	u16	b0;		// size 0x02, offset 0x06
} AXPBLPF; // size 0x08

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82f2e
typedef struct _AXPBBIQUAD
{
	u16	on;		// size 0x02, offset 0x00
	u16	xn1;	// size 0x02, offset 0x02
	u16	xn2;	// size 0x02, offset 0x04
	u16	yn1;	// size 0x02, offset 0x06
	u16	yn2;	// size 0x02, offset 0x08
	u16	b0;		// size 0x02, offset 0x0a
	u16	b1;		// size 0x02, offset 0x0c
	u16	b2;		// size 0x02, offset 0x0e
	u16	a1;		// size 0x02, offset 0x10
	u16	a2;		// size 0x02, offset 0x12
} AXPBBIQUAD; // size 0x14

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82fc7
typedef struct _AXPBRMTMIX
{
	u16	vMain0;			// size 0x02, offset 0x00
	u16	vDeltaMain0;	// size 0x02, offset 0x02
	u16	vAux0;			// size 0x02, offset 0x04
	u16	vDeltaAux0;		// size 0x02, offset 0x06
	u16	vMain1;			// size 0x02, offset 0x08
	u16	vDeltaMain1;	// size 0x02, offset 0x0a
	u16	vAux1;			// size 0x02, offset 0x0c
	u16	vDeltaAux1;		// size 0x02, offset 0x0e
	u16	vMain2;			// size 0x02, offset 0x10
	u16	vDeltaMain2;	// size 0x02, offset 0x12
	u16	vAux2;			// size 0x02, offset 0x14
	u16	vDeltaAux2;		// size 0x02, offset 0x16
	u16	vMain3;			// size 0x02, offset 0x18
	u16	vDeltaMain3;	// size 0x02, offset 0x1a
	u16	vAux3;			// size 0x02, offset 0x1c
	u16	vDeltaAux3;		// size 0x02, offset 0x1e
} AXPBRMTMIX; // size 0x20

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x8310a
typedef struct _AXPBRMTDPOP
{
	s16	aMain0;	// size 0x02, offset 0x00
	s16	aMain1;	// size 0x02, offset 0x02
	s16	aMain2;	// size 0x02, offset 0x04
	s16	aMain3;	// size 0x02, offset 0x06
	s16	aAux0;	// size 0x02, offset 0x08
	s16	aAux1;	// size 0x02, offset 0x0a
	s16	aAux2;	// size 0x02, offset 0x0c
	s16	aAux3;	// size 0x02, offset 0x0e
} AXPBRMTDPOP; // size 0x10

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x831a2
typedef struct _AXPBRMTSRC
{
	u16	currentAddressFrac;	// size 0x02, offset 0x00
	u16	last_samples[4];	// size 0x08, offset 0x02
} AXPBRMTSRC; // size 0x0a

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x831f8
typedef union __AXPBRMTIIR
{
	AXPBLPF		lpf;	// size 0x08
	AXPBBIQUAD	biquad;	// size 0x14
} __AXPBRMTIIR; // size 0x14

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x8286e
typedef struct _AXPB
{
	u16				nextHi;			// size 0x002, offset 0x000 (0x028)
	u16				nextLo;			// size 0x002, offset 0x002 (0x02a)
	u16				currHi;			// size 0x002, offset 0x004 (0x02c)
	u16				currLo;			// size 0x002, offset 0x006 (0x02e)
	u16				srcSelect;		// size 0x002, offset 0x008 (0x030)
	u16				coefSelect;		// size 0x002, offset 0x00a (0x032)
	u32				mixerCtrl;		// size 0x004, offset 0x00c (0x034)
	u16				state;			// size 0x002, offset 0x010 (0x038)
	u16				type;			// size 0x002, offset 0x012 (0x03a)
	AXPBMIX			mix;			// size 0x030, offset 0x014 (0x03c)
	AXPBITD			itd;			// size 0x00e, offset 0x044 (0x06c)
	AXPBDPOP		dpop;			// size 0x018, offset 0x052 (0x07a)
	AXPBVE			ve;				// size 0x004, offset 0x06a (0x092)
	AXPBADDR		addr;			// size 0x010, offset 0x06e (0x096)
	AXPBADPCM		adpcm;			// size 0x028, offset 0x07e (0x0a6)
	AXPBSRC			src;			// size 0x00e, offset 0x0a6 (0x0ce)
	AXPBADPCMLOOP	adpcmLoop;		// size 0x006, offset 0x0b4 (0x0dc)
	AXPBLPF			lpf;			// size 0x008, offset 0x0ba (0x0e2)
	AXPBBIQUAD		biquad;			// size 0x014, offset 0x0c2 (0x0ea)
	u16				remote;			// size 0x002, offset 0x0d6 (0x0fe)
	u16				rmtMixerCtrl;	// size 0x002, offset 0x0d8 (0x100)
	AXPBRMTMIX		rmtMix;			// size 0x020, offset 0x0da (0x102)
	AXPBRMTDPOP		rmtDpop;		// size 0x010, offset 0x0fa (0x122)
	AXPBRMTSRC		rmtSrc;			// size 0x00a, offset 0x10a (0x132)
	__AXPBRMTIIR	rmtIIR;			// size 0x014, offset 0x114 (0x13c)
	byte2_t			pad[12];
} AXPB; // size 0x140

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x82773
typedef struct _AXVPB
{
	void			*next;			// size 0x004, offset 0x000
	void			*prev;			// size 0x004, offset 0x004
	void			*next1;			// size 0x004, offset 0x008
	u32				priority;		// size 0x004, offset 0x00c
	AXVPBCallback	*callback;		// size 0x004, offset 0x010
	register_t		userContext;	// size 0x004, offset 0x014
	u32				index;			// size 0x004, offset 0x018
	AXVPBSyncFlags	sync;			// size 0x004, offset 0x01c
	u32				depop;			// size 0x004, offset 0x020
	void			*itdBuffer;		// size 0x004, offset 0x024
	AXPB			pb;				// size 0x140, offset 0x028
} AXVPB; // size 0x168

void AXInit(void);
BOOL AXIsInit(void);
AXVPB *AXAcquireVoice(u32 priority, AXVPBCallback *cb, register_t userContext);
void AXFreeVoice(AXVPB *p);
AXFrameCallback *AXRegisterCallback(AXFrameCallback *cb);
void AXRegisterAuxACallback(AXAuxCallback *cb, void *context);
void AXGetAuxACallback(AXAuxCallback **cbOut, void **contextOut);

u16 AXGetAuxAReturnVolume(void);
u16 AXGetAuxBReturnVolume(void);
u16 AXGetAuxCReturnVolume(void);
void AXSetAuxAReturnVolume(u16 volume);
void AXSetAuxBReturnVolume(u16 volume);
void AXSetAuxCReturnVolume(u16 volume);

// AXFX

// [SPQE7T]/ISpyD.elf:.debug_info::0x36bdec
typedef void *AXFXAllocFunc(u32 /* explicitly unnamed */);

// [SPQE7T]/ISpyD.elf:.debug_info::0x36be0f
typedef void AXFXFreeFunc(void */* explicitly unnamed */);

// [SPQE7T]/ISpyD.elf:.debug_info::0x36c3ae
typedef struct AXFX_BUS
{
	s32	*left;		// size 0x04, offset 0x00
	s32	*right;		// size 0x04, offset 0x04
	s32	*surround;	// size 0x04, offset 0x08
} AXFX_BUS; // size 0x0c

// [SPQE7T]/ISpyD.elf:.debug_info::0x36bec8
typedef struct AXFX_REVERBHI_EXP
{
	f32			*earlyLine[3];				// size 0x00c, offset 0x000
	u32			earlyPos[3];				// size 0x00c, offset 0x00c
	u32			earlyLength;				// size 0x004, offset 0x018
	u32			earlyMaxLength;				// size 0x004, offset 0x01c
	f32			earlyCoef[3];				// size 0x00c, offset 0x020
	f32			*preDelayLine[3];			// size 0x00c, offset 0x02c
	u32			preDelayPos;				// size 0x004, offset 0x038
	u32			preDelayLength;				// size 0x004, offset 0x03c
	u32			preDelayMaxLength;			// size 0x004, offset 0x040
	f32			*combLine[3][3];			// size 0x024, offset 0x044
	u32			combPos[3];					// size 0x00c, offset 0x068
	u32			combLength[3];				// size 0x00c, offset 0x074
	u32			combMaxLength[3];			// size 0x00c, offset 0x080
	f32			combCoef[3];				// size 0x00c, offset 0x08c
	f32			*allpassLine[3][2];			// size 0x018, offset 0x098
	u32			allpassPos[2];				// size 0x008, offset 0x0b0
	u32			allpassLength[2];			// size 0x008, offset 0x0b8
	u32			allpassMaxLength[2];		// size 0x008, offset 0x0c0
	f32			*lastAllpassLine[3];		// size 0x00c, offset 0x0c8
	u32			lastAllpassPos[3];			// size 0x00c, offset 0x0d4
	u32			lastAllpassLength[3];		// size 0x00c, offset 0x0e0
	u32			lastAllpassMaxLength[3];	// size 0x00c, offset 0x0ec
	f32			allpassCoef;				// size 0x004, offset 0x0f8
	f32			lastLpfOut[3];				// size 0x00c, offset 0x0fc
	f32			lpfCoef;					// size 0x004, offset 0x108
	u32			active;						// size 0x004, offset 0x10c
	u32			earlyMode;					// size 0x004, offset 0x110
	f32			preDelayTimeMax;			// size 0x004, offset 0x114
	f32			preDelayTime;				// size 0x004, offset 0x118
	u32			fusedMode;					// size 0x004, offset 0x11c
	f32			fusedTime;					// size 0x004, offset 0x120
	f32			coloration;					// size 0x004, offset 0x124
	f32			damping;					// size 0x004, offset 0x128
	f32			crosstalk;					// size 0x004, offset 0x12c
	f32			earlyGain;					// size 0x004, offset 0x130
	f32			fusedGain;					// size 0x004, offset 0x134
	AXFX_BUS	*busIn;						// size 0x004, offset 0x138
	AXFX_BUS	*busOut;					// size 0x004, offset 0x13c
	f32			outGain;					// size 0x004, offset 0x140
	f32			sendGain;					// size 0x004, offset 0x144
} AXFX_REVERBHI_EXP; // size 0x148

// [SPQE7T]/ISpyD.elf:.debug_info::0x36be2c
typedef struct AXFX_REVERBHI
{
	AXFX_REVERBHI_EXP	reverbInner;	// size 0x148, offset 0x000
	f32					coloration;		// size 0x004, offset 0x148
	f32					mix;			// size 0x004, offset 0x14c
	f32					time;			// size 0x004, offset 0x150
	f32					damping;		// size 0x004, offset 0x154
	f32					preDelay;		// size 0x004, offset 0x158
	f32					crosstalk;		// size 0x004, offset 0x15c
} AXFX_REVERBHI; // size 0x160

void AXFXSetHooks(AXFXAllocFunc *alloc, AXFXFreeFunc *free);
void AXFXGetHooks(AXFXAllocFunc **allocOut, AXFXFreeFunc **freeOut);
BOOL AXFXReverbHiInit(AXFX_REVERBHI *reverbHi);
BOOL AXFXReverbHiShutdown(AXFX_REVERBHI *reverbHi);

// Available for use as the argument cb to AXRegisterAuxCallback
void AXFXReverbHiCallback(void *data, void *context);

// MTX

// Matrices

typedef f32 Mtx23[2][3];
typedef f32 Mtx[3][4];
typedef f32 Mtx43[4][3];
typedef f32 Mtx44[4][4];

typedef f32 (*Mtx23Ptr)[3];
typedef f32 (*MtxPtr)[4];
typedef f32 (*Mtx43Ptr)[3];
typedef f32 (*Mtx44Ptr)[4];

// not official names, just what i thought would make sense
typedef f32 const (*CMtx23Ptr)[3];
typedef f32 const (*CMtxPtr)[4];
typedef f32 const (*CMtx43Ptr)[3];
typedef f32 const (*CMtx44Ptr)[4];

// Vectors

// [SPQE7T]/ISpyD.elf:.debug_info::0xd64ea
typedef struct Vec2
{
	f32	x;	// size 0x04, offset 0x00
	f32	y;	// size 0x04, offset 0x04
} Vec2; // size 0x08

// [SPQE7T]/ISpyD.elf:.debug_info::0xd64bb
typedef struct Vec
{
	f32	x;	// size 0x04, offset 0x00
	f32	y;	// size 0x04, offset 0x04
	f32	z;	// size 0x04, offset 0x08
} Vec; // size 0x0c

// I think some of these names were in assert strings in debug MTX
typedef Vec2 *Vec2Ptr;
typedef Vec *VecPtr;

typedef Vec2 const *CVec2Ptr;
typedef Vec const *CVecPtr;

#if !defined(NDEBUG)
# define MTXIdentity	C_MTXIdentity
# define MTXConcat		C_MTXConcat
# define MTXInverse		C_MTXInverse
# define MTXRotRad		C_MTXRotRad
# define MTXRotAxisRad	C_MTXRotAxisRad
# define MTXTrans		C_MTXTrans
# define MTXTransApply	C_MTXTransApply
# define MTXScale		C_MTXScale

# define MTXMultVec		C_MTXMultVec
#else
# define MTXIdentity	PSMTXIdentity
# define MTXConcat		PSMTXConcat
# define MTXInverse		PSMTXInverse
# define MTXRotRad		PSMTXRotRad
# define MTXRotAxisRad	PSMTXRotAxisRad
# define MTXTrans		PSMTXTrans
# define MTXTransApply	PSMTXTransApply
# define MTXScale		PSMTXScale

# define MTXMultVec		PSMTXMultVec
#endif

void PSMTXIdentity(Mtx m);
void C_MTXIdentity(Mtx m);

void PSMTXCopy(CMtxPtr src, MtxPtr dst);

void PSMTXConcat(CMtxPtr a, CMtxPtr b, MtxPtr ab);
void C_MTXConcat(CMtxPtr a, CMtxPtr b, MtxPtr ab);

BOOL PSMTXInverse(CMtxPtr src, MtxPtr inv);
BOOL C_MTXInverse(CMtxPtr src, MtxPtr inv);

void PSMTXRotRad(Mtx m, char axis, f32);
void C_MTXRotRad(Mtx m, char axis, f32);

void PSMTXRotAxisRad(MtxPtr, CVecPtr, f32);
void C_MTXRotAxisRad(MtxPtr, CVecPtr, f32);

void PSMTXTrans(Mtx m, f32, f32, f32);
void C_MTXTrans(Mtx m, f32, f32, f32);

void PSMTXTransApply(CMtxPtr src, MtxPtr dst, f32, f32, f32);
void C_MTXTransApply(CMtxPtr src, MtxPtr dst, f32, f32, f32);

void PSMTXScale(MtxPtr m, f32, f32, f32);
void C_MTXScale(MtxPtr m, f32, f32, f32);

void PSMTXMultVec(CMtxPtr m, CVecPtr src, VecPtr dst);
void C_MTXMultVec(CMtxPtr m, CVecPtr src, VecPtr dst);

void PSVECNormalize(CVecPtr src, VecPtr dst);

void PSVECCrossProduct(CVecPtr a, CVecPtr b, CVecPtr ab);

f32 PSVECSquareDistance(CVecPtr a, CVecPtr b);


// GX

/* Reference:
 * Patent US 6,937,245 B1
 * https://patents.google.com/patent/US6937245B1
 * section 13, line 4
 */
typedef u8 GXBool;
#define GX_TRUE		1
#define GX_FALSE	0

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48f80e
typedef enum _GXAlphaOp
{
	GX_AOP_AND,
	GX_AOP_OR,
	GX_AOP_XOR,
	GX_AOP_XNOR,

	GX_MAX_ALPHAOP
} GXAlphaOp;

// [SPQE7T]/ISpyD.elf:.debug_info::0x21232f
typedef enum _GXAnisotropy
{
	GX_ANISO_1,
	GX_ANISO_2,
	GX_ANISO_4,

	GX_MAX_ANISOTROPY
} GXAnisotropy;

// NOTE: older GX version (Feb 24 2009 00:57:49)
// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x10044
typedef enum _GXAttnFn
{
	GX_AF_SPEC,
	GX_AF_SPOT,

	GX_AF_NONE
} GXAttnFn;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x438266
typedef enum _GXAttr
{
	GX_VA_PNMTXIDX,

	GX_VA_TEX0MTXIDX,
	GX_VA_TEX1MTXIDX,
	GX_VA_TEX2MTXIDX,
	GX_VA_TEX3MTXIDX,
	GX_VA_TEX4MTXIDX,
	GX_VA_TEX5MTXIDX,
	GX_VA_TEX6MTXIDX,
	GX_VA_TEX7MTXIDX,

	GX_VA_POS,
	GX_VA_NRM,

	GX_VA_CLR0,
	GX_VA_CLR1,

	GX_VA_TEX0,
	GX_VA_TEX1,
	GX_VA_TEX2,
	GX_VA_TEX3,
	GX_VA_TEX4,
	GX_VA_TEX5,
	GX_VA_TEX6,
	GX_VA_TEX7,

	GX_POS_MTX_ARRAY,
	GX_NRM_MTX_ARRAY,
	GX_TEX_MTX_ARRAY,
	GX_LIGHT_ARRAY,

	GX_VA_NBT,

	GX_VA_MAX_ATTR,
	GX_VA_NULL = 0xff
} GXAttr;

// [SPQE7T]/ISpyD.elf:.debug_info::0x268dc0
typedef enum _GXAttrType
{
	GX_NONE,

	GX_DIRECT,

	GX_INDEX8,
	GX_INDEX16
} GXAttrType;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x45d222
typedef enum _GXBlendFactor
{
	GX_BL_ZERO,
	GX_BL_ONE,

	GX_BL_SRCCLR,
	GX_BL_INVSRCCLR,

	GX_BL_SRCALPHA,
	GX_BL_INVSRCALPHA,

	GX_BL_DSTALPHA,
	GX_BL_INVDSTALPHA,

	GX_BL_DSTCLR		= GX_BL_SRCCLR,
	GX_BL_INVDSTCLR		= GX_BL_INVSRCCLR
} GXBlendFactor;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48f667
typedef enum _GXBlendMode
{
	GX_BM_NONE,

	GX_BM_BLEND,
	GX_BM_LOGIC,
	GX_BM_SUBTRACT,

	GX_MAX_BLENDMODE
} GXBlendMode;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x4070a5
typedef enum _GXChannelID
{
	GX_COLOR0,
	GX_COLOR1,

	GX_ALPHA0,
	GX_ALPHA1,

	GX_COLOR0A0,
	GX_COLOR1A1,

	GX_COLOR_ZERO,

	GX_ALPHA_BUMP,
	GX_ALPHA_BUMPN,

	GX_COLOR_NULL = 0xff
} GXChannelID;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x401562
typedef enum _GXColorSrc
{
	GX_SRC_REG,
	GX_SRC_VTX
} GXColorSrc;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x45d66e
typedef enum _GXCompare
{
	GX_NEVER,

	GX_LESS,
	GX_EQUAL,
	GX_LEQUAL,
	GX_GREATER,
	GX_NEQUAL,
	GX_GEQUAL,

	GX_ALWAYS
} GXCompare;

// [SPQE7T]/ISpyD.elf:.debug_info::0x268cce
typedef enum _GXCompCnt
{
	GX_POS_XY = 0,
	GX_POS_XYZ,

	GX_NRM_XYZ = 0,
	GX_NRM_NBT,
	GX_NRM_NBT3,

	GX_CLR_RGB = 0,
	GX_CLR_RGBA,

	GX_TEX_S = 0,
	GX_TEX_ST
} GXCompCnt;

// [SPQE7T]/ISpyD.elf:.debug_info::0x25e1b3
typedef enum _GXCompType
{
	GX_U8,
	GX_S8,
	GX_U16,
	GX_S16,
	GX_F32,

	GX_RGB565 = 0,
	GX_RGB8,
	GX_RGBX8,
	GX_RGBA4,
	GX_RGBA6,
	GX_RGBA8
} GXCompType;

// [SPQE7T]/ISpyD.elf:.debug_info::0x286e6d
typedef enum _GXCullMode
{
	GX_CULL_NONE,
	GX_CULL_FRONT,
	GX_CULL_BACK,
	GX_CULL_ALL
} GXCullMode;

// [SJXE41]/LynWiiRetail.elf:.debug_info::0xd838
typedef enum _GXDiffuseFn
{
	GX_DF_NONE,

	GX_DF_SIGN,
	GX_DF_CLAMP
} GXDiffuseFn;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48fc70
typedef enum _GXIndTexAlphaSel
{
	GX_ITBA_OFF,

	GX_ITBA_S,
	GX_ITBA_T,
	GX_ITBA_U,

	GX_MAX_ITBALPHA
} GXIndTexAlphaSel;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48fb29
typedef enum _GXIndTexBiasSel
{
	GX_ITB_NONE,

	GX_ITB_S,
	GX_ITB_T,
	GX_ITB_ST,
	GX_ITB_U,
	GX_ITB_SU,
	GX_ITB_TU,
	GX_ITB_STU,

	GX_MAX_ITBIAS
} GXIndTexBiasSel;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48fac0
typedef enum _GXIndTexFormat
{
	GX_ITF_8,
	GX_ITF_5,
	GX_ITF_4,
	GX_ITF_3,

	GX_MAX_ITFORMAT
} GXIndTexFormat;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48ef62
typedef enum _GXIndTexMtxID
{
	GX_ITM_OFF,

	GX_ITM_0,
	GX_ITM_1,
	GX_ITM_2,

	GX_ITM_S0 = 5,
	GX_ITM_S1,
	GX_ITM_S2,

	GX_ITM_T0 = 9,
	GX_ITM_T1,
	GX_ITM_T2
} GXIndTexMtxID;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x490652
typedef enum _GXIndTexScale
{
	GX_ITS_1,
	GX_ITS_2,
	GX_ITS_4,
	GX_ITS_8,
	GX_ITS_16,
	GX_ITS_32,
	GX_ITS_64,
	GX_ITS_128,
	GX_ITS_256,

	GX_MAX_ITSCALE
} GXIndTexScale;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48fa37
typedef enum _GXIndTexStageID
{
	GX_INDTEXSTAGE0,
	GX_INDTEXSTAGE1,
	GX_INDTEXSTAGE2,
	GX_INDTEXSTAGE3,

	GX_MAX_INDTEXSTAGE
} GXIndTexStageID;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48fbd5
typedef enum _GXIndTexWrap
{
	GX_ITW_OFF,

	GX_ITW_256,
	GX_ITW_128,
	GX_ITW_64,
	GX_ITW_32,
	GX_ITW_16,
	GX_ITW_0,

	GX_MAX_ITWRAP
} GXIndTexWrap;

// [SPQE7T]/ISpyD.elf:.debug_info::0x1eeb49
typedef enum _GXLightID
{
	GX_LIGHT0		= 1 << 0,
	GX_LIGHT1		= 1 << 1,
	GX_LIGHT2		= 1 << 2,
	GX_LIGHT3		= 1 << 3,
	GX_LIGHT4		= 1 << 4,
	GX_LIGHT5		= 1 << 5,
	GX_LIGHT6		= 1 << 6,
	GX_LIGHT7		= 1 << 7,

	GX_MAX_LIGHT	= 1 << 8,
	GX_LIGHT_NULL	= 0
} GXLightID;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48f6dc
typedef enum _GXLogicOp
{
	GX_LO_CLEAR,

	GX_LO_AND,
	GX_LO_REVAND,
	GX_LO_COPY,
	GX_LO_INVAND,
	GX_LO_NOOP,
	GX_LO_XOR,
	GX_LO_OR,
	GX_LO_NOR,
	GX_LO_EQUIV,
	GX_LO_INV,
	GX_LO_REVOR,
	GX_LO_INVCOPY,
	GX_LO_INVOR,
	GX_LO_NAND,

	GX_LO_SET
} GXLogicOp;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x455fb4
typedef enum _GXPrimitive
{
	GX_POINTS			= 0xb8,	// 0b1011'1000

	GX_LINES			= 0xa8,	// 0b1010'1000
	GX_LINESTRIP		= 0xb0,	// 0b1011'0000

	GX_TRIANGLES		= 0x90,	// 0x1001'0000
	GX_TRIANGLESTRIP	= 0x98,	// 0x1001'1000
	GX_TRIANGLEFAN		= 0xa0,	// 0x1010'0000

	GX_QUADS			= 0x80	// 0x1000'0000
} GXPrimitive;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x490518
typedef enum _GXTevAlphaArg
{
	GX_CA_APREV,

	GX_CA_A0,
	GX_CA_A1,
	GX_CA_A2,

	GX_CA_TEXA,
	GX_CA_RASA,

	GX_CA_KONST,
	GX_CA_ZERO,
	GX_CA_ONE
} GXTevAlphaArg;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48fe17
typedef enum _GXTevBias
{
	GX_TB_ZERO,
	GX_TB_ADDHALF,
	GX_TB_SUBHALF,

	GX_MAX_TEVBIAS
} GXTevBias;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x162cf3
typedef enum _GXTevColorArg
{
	GX_CC_CPREV,
	GX_CC_APREV,

	GX_CC_C0,
	GX_CC_A0,
	GX_CC_C1,
	GX_CC_A1,
	GX_CC_C2,
	GX_CC_A2,

	GX_CC_TEXC,
	GX_CC_TEXA,
	GX_CC_RASC,
	GX_CC_RASA,

	GX_CC_ONE,
	GX_CC_HALF,
	GX_CC_KONST,
	GX_CC_ZERO,

	GX_CC_TEXRRR,
	GX_CC_TEXGGG,
	GX_CC_TEXBBB,

	GX_CC_QUARTER	= GX_CC_KONST
} GXTevColorArg;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48f87f
typedef enum _GXTevColorChan
{
	GX_CH_RED,
	GX_CH_GREEN,
	GX_CH_BLUE,
	GX_CH_ALPHA
} GXTevColorChan;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x490211
typedef enum _GXTevKColorSel
{
	GX_TEV_KCSEL_8_8,
	GX_TEV_KCSEL_7_8,
	GX_TEV_KCSEL_6_8,
	GX_TEV_KCSEL_5_8,
	GX_TEV_KCSEL_4_8,
	GX_TEV_KCSEL_3_8,
	GX_TEV_KCSEL_2_8,
	GX_TEV_KCSEL_1_8,

	GX_TEV_KCSEL_1 = GX_TEV_KCSEL_8_8,
	GX_TEV_KCSEL_3_4 = GX_TEV_KCSEL_6_8,
	GX_TEV_KCSEL_1_2 = GX_TEV_KCSEL_4_8,
	GX_TEV_KCSEL_1_4 = GX_TEV_KCSEL_2_8,

	GX_TEV_KCSEL_K0 = 12,
	GX_TEV_KCSEL_K1,
	GX_TEV_KCSEL_K2,
	GX_TEV_KCSEL_K3,

	GX_TEV_KCSEL_K0_R,
	GX_TEV_KCSEL_K1_R,
	GX_TEV_KCSEL_K2_R,
	GX_TEV_KCSEL_K3_R,
	GX_TEV_KCSEL_K0_G,
	GX_TEV_KCSEL_K1_G,
	GX_TEV_KCSEL_K2_G,
	GX_TEV_KCSEL_K3_G,
	GX_TEV_KCSEL_K0_B,
	GX_TEV_KCSEL_K1_B,
	GX_TEV_KCSEL_K2_B,
	GX_TEV_KCSEL_K3_B,
	GX_TEV_KCSEL_K0_A,
	GX_TEV_KCSEL_K1_A,
	GX_TEV_KCSEL_K2_A,
	GX_TEV_KCSEL_K3_A
} GXTevKColorSel;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48ff5c2
typedef enum _GXTevKAlphaSel
{
	GX_TEV_KASEL_8_8,
	GX_TEV_KASEL_7_8,
	GX_TEV_KASEL_6_8,
	GX_TEV_KASEL_5_8,
	GX_TEV_KASEL_4_8,
	GX_TEV_KASEL_3_8,
	GX_TEV_KASEL_2_8,
	GX_TEV_KASEL_1_8,

	GX_TEV_KASEL_1 = GX_TEV_KASEL_8_8,
	GX_TEV_KASEL_3_4 = GX_TEV_KASEL_6_8,
	GX_TEV_KASEL_1_2 = GX_TEV_KASEL_4_8,
	GX_TEV_KASEL_1_4 = GX_TEV_KASEL_2_8,

	GX_TEV_KASEL_K0_R = 16,
	GX_TEV_KASEL_K1_R,
	GX_TEV_KASEL_K2_R,
	GX_TEV_KASEL_K3_R,
	GX_TEV_KASEL_K0_G,
	GX_TEV_KASEL_K1_G,
	GX_TEV_KASEL_K2_G,
	GX_TEV_KASEL_K3_G,
	GX_TEV_KASEL_K0_B,
	GX_TEV_KASEL_K1_B,
	GX_TEV_KASEL_K2_B,
	GX_TEV_KASEL_K3_B,
	GX_TEV_KASEL_K0_A,
	GX_TEV_KASEL_K1_A,
	GX_TEV_KASEL_K2_A,
	GX_TEV_KASEL_K3_A
} GXTevKAlphaSel;

// NOTE: older GX version (Feb 24 2009 00:57:49)
// [R89JEL]:/bin/RVL/Debug/mainD.elf:.debug::0x107e0
typedef enum _GXTevKColorID
{
	GX_KCOLOR0,
	GX_KCOLOR1,
	GX_KCOLOR2,
	GX_KCOLOR3,

	GX_MAX_KCOLOR
} GXTevKColorID;

// [SPQE7T]/ISpyD.elf:.debug_info::0x1f105e
typedef enum _GXTevMode
{
	GX_MODULATE,
	GX_DECAL,
	GX_BLEND,
	GX_REPLACE,
	GX_PASSCLR
} GXTevMode;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48fce7
typedef enum _GXTevOp
{
	GX_TEV_ADD,
	GX_TEV_SUB,

	GX_TEV_COMP_R8_GT = 8,
	GX_TEV_COMP_R8_EQ,
	GX_TEV_COMP_GR16_GT,
	GX_TEV_COMP_GR16_EQ,
	GX_TEV_COMP_BGR24_GT,
	GX_TEV_COMP_BGR24_EQ,
	GX_TEV_COMP_RGB8_GT,
	GX_TEV_COMP_RGB8_EQ,

	GX_TEV_COMP_A8_GT = GX_TEV_COMP_RGB8_GT,
	GX_TEV_COMP_A8_EQ = GX_TEV_COMP_RGB8_EQ
} GXTevOp;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48fef1
typedef enum _GXTevRegID
{
	GX_TEVPREV,

	GX_TEVREG0,
	GX_TEVREG1,
	GX_TEVREG2,

	GX_MAX_TEVREG
} GXTevRegID;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x48fe77
typedef enum _GXTevScale
{
	GX_TEV_SCALE_0,
	GX_TEV_SCALE_1,
	GX_TEV_SCALE_2,
	GX_TEV_SCALE_3
} GXTevScale;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x163019
typedef enum _GXTevStageID
{
	GX_TEVSTAGE0,
	GX_TEVSTAGE1,
	GX_TEVSTAGE2,
	GX_TEVSTAGE3,
	GX_TEVSTAGE4,
	GX_TEVSTAGE5,
	GX_TEVSTAGE6,
	GX_TEVSTAGE7,
	GX_TEVSTAGE8,
	GX_TEVSTAGE9,
	GX_TEVSTAGE10,
	GX_TEVSTAGE11,
	GX_TEVSTAGE12,
	GX_TEVSTAGE13,
	GX_TEVSTAGE14,
	GX_TEVSTAGE15,

	GX_MAX_TEVSTAGE
} GXTevStageID;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x4905c4
typedef enum _GXTevSwapSel
{
	GX_TEV_SWAP0,
	GX_TEV_SWAP1,
	GX_TEV_SWAP2,
	GX_TEV_SWAP3,

	GX_MAX_TEVSWAP
} GXTevSwapSel;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x14a955
typedef enum _GXTexCoordID
{
	GX_TEXCOORD0,
	GX_TEXCOORD1,
	GX_TEXCOORD2,
	GX_TEXCOORD3,
	GX_TEXCOORD4,
	GX_TEXCOORD5,
	GX_TEXCOORD6,
	GX_TEXCOORD7,

	GX_MAX_TEXCOORD,
	GX_TEXCOORD_NULL = 0xff
} GXTexCoordID;

// [SPQE7T]/ISpyD.elf:.debug_info::0x1176fc
typedef enum _GXTexFilter
{
	GX_NEAR,
	GX_LINEAR,

	GX_NEAR_MIP_NEAR,
	GX_LIN_MIP_NEAR,

	GX_NEAR_MIP_LIN,
	GX_LIN_MIP_LIN
} GXTexFilter;

// [SPQE7T]/ISpyD.elf:.debug_info::0x28a5e5
typedef enum _GXTexFmt
{
	GX_TF_I4		=  0,
	GX_TF_I8		=  1,
	GX_TF_IA4		=  2,
	GX_TF_IA8		=  3,

	GX_TF_RGB565	=  4,
	GX_TF_RGB5A3	=  5,
	GX_TF_RGBA8		=  6,

	GX_TF_CMPR		= 14,

	GX_CTF_R4		= 32,
	GX_CTF_RA4		= 34,
	GX_CTF_RA8		= 35,
	GX_CTF_YUVA8	= 38,

	GX_CTF_A8		= 39,
	GX_CTF_R8		= 40,
	GX_CTF_G8		= 41,
	GX_CTF_B8		= 42,
	GX_CTF_RG8		= 43,
	GX_CTF_GB8		= 44,

	GX_TF_Z8		= 17,
	GX_TF_Z16		= 19,
	GX_TF_Z24X8		= 22,

	GX_CTF_Z4		= 48,
	GX_CTF_Z8M		= 57,
	GX_CTF_Z8L		= 58,
	GX_CTF_Z16L		= 60,

	GX_TF_A8		= GX_CTF_A8
} GXTexFmt;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x14ab0b
typedef enum _GXTexGenSrc
{
	GX_TG_POS,

	GX_TG_NRM,
	GX_TG_BINRM,
	GX_TG_TANGENT,

	GX_TG_TEX0,
	GX_TG_TEX1,
	GX_TG_TEX2,
	GX_TG_TEX3,
	GX_TG_TEX4,
	GX_TG_TEX5,
	GX_TG_TEX6,
	GX_TG_TEX7,
	GX_TG_TEXCOORD0,
	GX_TG_TEXCOORD1,
	GX_TG_TEXCOORD2,
	GX_TG_TEXCOORD3,
	GX_TG_TEXCOORD4,
	GX_TG_TEXCOORD5,
	GX_TG_TEXCOORD6,

	GX_TG_COLOR0,
	GX_TG_COLOR1
} GXTexGenSrc;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x14aa2f
typedef enum _GXTexGenType
{
	GX_TG_MTX3x4,
	GX_TG_MTX2x4,

	GX_TG_BUMP0,
	GX_TG_BUMP1,
	GX_TG_BUMP2,
	GX_TG_BUMP3,
	GX_TG_BUMP4,
	GX_TG_BUMP5,
	GX_TG_BUMP6,
	GX_TG_BUMP7,

	GX_TG_SRTG
} GXTexGenType;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x2b93e8
typedef enum _GXTexMapID
{
	GX_TEXMAP0,
	GX_TEXMAP1,
	GX_TEXMAP2,
	GX_TEXMAP3,
	GX_TEXMAP4,
	GX_TEXMAP5,
	GX_TEXMAP6,
	GX_TEXMAP7,

	GX_MAX_TEXMAP,
	GX_TEXMAP_NULL	= 0xff,
	GX_TEX_DISABLE	= 256
} GXTexMapID;

// [SPQE7T]/ISpyD.elf:.debug_info::0x2122a1
typedef enum _GXTexWrapMode
{
	GX_CLAMP,
	GX_REPEAT,
	GX_MIRROR,

	GX_MAX_TEXWRAPMODE
} GXTexWrapMode;

// [SPQE7T]/ISpyD.elf:.debug_info::0x288777
typedef enum _GXTlutFmt
{
	GX_TL_IA8,
	GX_TL_RGB565,
	GX_TL_RGB5A3,

	GX_MAX_TLUTFMT
} GXTlutFmt;

// [SPQE7T]/ISpyD.elf:.debug_info::0x2689ea
typedef enum _GXVtxFmt
{
	GX_VTXFMT0,
	GX_VTXFMT1,
	GX_VTXFMT2,
	GX_VTXFMT3,
	GX_VTXFMT4,
	GX_VTXFMT5,
	GX_VTXFMT6,
	GX_VTXFMT7,

	GX_MAX_VTXFMT
} GXVtxFmt;

/* Patent US 6,664,958 B1
 * [2] https://patents.google.com/patent/US6664958B1
 * section 15, lines 20-25
 */
typedef enum _GXZTexOp
{
	GX_ZT_DISABLE,
	GX_ZT_ADD,
	GX_ZT_REPLACE, // NOTE: misspelled as GZ_ZT_REPLACE in patent

	GX_MAX_ZTEXOP
} GXZTexOp;

// [SGLEA4]/GormitiDebug.elf:.debug_info::0xba05c
typedef struct _GXColor
{
	u8	r;	// size 0x01, offset 0x00
	u8	g;	// size 0x01, offset 0x01
	u8	b;	// size 0x01, offset 0x02
	u8	a;	// size 0x01, offset 0x03
} GXColor; // size 0x04

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x4764f4
typedef struct _GXColorS10
{
	s16	r;	// size 0x02, offset 0x00
	s16	g;	// size 0x02, offset 0x02
	s16	b;	// size 0x02, offset 0x04
	s16	a;	// size 0x02, offset 0x06
} GXColorS10; // size 0x08

// [SPQE7T]/ISpyD.elf:.debug_info::0x11894d
typedef struct _GXTexObj
{
	// i guess they didn't want to expose the internals
	byte4_t	dummy[8];
} GXTexObj; // size 0x20

// [SPQE7T]/ISpyD.elf:.debug_info::0x1eb04c
typedef struct _GXTlutObj
{
	byte4_t	dummy[3];
} GXTlutObj; // size 0x0c

void GXClearVtxDesc(void);
int GXGetTexObjFmt(GXTexObj *);
u16 GXGetTexObjHeight(GXTexObj *);
u16 GXGetTexObjWidth(GXTexObj *);
GXTexWrapMode GXGetTexObjWrapS(GXTexObj *);
GXTexWrapMode GXGetTexObjWrapT(GXTexObj *);
void *GXGetTexObjUserData(GXTexObj *);
void GXInitTexObj(GXTexObj *, void *, u16, u16, GXTexFmt, GXTexWrapMode, GXTexWrapMode, GXBool);
void GXInitTexObjCI(GXTexObj *, void *, u16, u16, GXTexFmt, GXTexWrapMode,
                    GXTexWrapMode, GXBool, u32);
void GXInitTexObjLOD(GXTexObj *, GXTexFilter, GXTexFilter,
                     f32, f32, f32, GXBool,
                     GXBool, GXAnisotropy);
void GXInitTexObjTlut(GXTexObj *, u32);
void GXInitTexObjWrapMode(GXTexObj *, GXTexWrapMode, GXTexWrapMode);
void GXInitTexObjUserData(GXTexObj *, void *);
void GXInitTlutObj(GXTlutObj *, void *, GXTlutFmt, u16);
void GXLoadPosMtxImm(Mtx, int);
void GXLoadTexMtxImm(const Mtx, u32, GXTexGenType);
void GXLoadTexObj(GXTexObj *, int);
void GXLoadTlut(GXTlutObj *, u32);
void GXSetAlphaCompare(GXCompare, u8, GXAlphaOp, GXCompare, u8);
void GXSetAlphaUpdate(GXBool);
void GXSetBlendMode(GXBlendMode, GXBlendFactor, GXBlendFactor, GXLogicOp);
void GXSetChanCtrl(GXChannelID, GXBool, GXColorSrc, GXColorSrc, GXLightID, GXDiffuseFn, GXAttnFn);
void GXSetChanMatColor(GXChannelID, GXColor);
void GXSetColorUpdate(GXBool);
void GXSetCullMode(GXCullMode);
void GXSetCurrentMtx(int);
void GXSetDispCopyGamma(int);
void GXSetFog(int, f32, f32, f32, f32, GXColor);
void GXSetIndTexCoordScale(GXIndTexStageID, GXIndTexScale, GXIndTexScale);
void GXSetIndTexMtx(GXIndTexMtxID, const Mtx23, s8);
void GXSetIndTexOrder(GXIndTexStageID, GXTexCoordID, GXTexMapID);
void GXSetLineWidth(u8, int);
void GXSetNumChans(u8);
void GXSetNumIndStages(u8);
void GXSetNumTevStages(u8);
void GXSetNumTexGens(u8);
void GXSetTevAlphaIn(GXTevStageID, GXTevAlphaArg, GXTevAlphaArg, GXTevAlphaArg, GXTevAlphaArg);
void GXSetTevAlphaOp(GXTevStageID, GXTevOp, GXTevBias, GXTevScale, u8, GXTevRegID);
void GXSetTevColor(GXTevRegID, GXColor);
void GXSetTevColorIn(GXTevStageID, GXTevColorArg, GXTevColorArg, GXTevColorArg, GXTevColorArg);
void GXSetTevColorOp(GXTevStageID, GXTevOp, GXTevBias, GXTevScale, u8, GXTevRegID);
void GXSetTevColorS10(GXTevRegID, GXColorS10);
void GXSetTevDirect(GXTevStageID);
void GXSetTevIndirect(GXTevStageID, GXIndTexStageID, GXIndTexFormat, GXIndTexBiasSel, GXIndTexMtxID, GXIndTexWrap, GXIndTexWrap, GXBool, GXBool, GXIndTexAlphaSel);
void GXSetTevKAlphaSel(GXTevStageID, GXTevKAlphaSel);
void GXSetTevKColor(GXTevKColorID, GXColor);
void GXSetTevKColorSel(GXTevStageID, GXTevKColorSel);
void GXSetTevOp(GXTevStageID, GXTevMode);
void GXSetTevOrder(GXTevStageID, GXTexCoordID, GXTexMapID, GXChannelID);
void GXSetTevSwapMode(GXTevStageID, GXTevSwapSel, GXTevSwapSel);
void GXSetTevSwapModeTable(GXTevSwapSel, GXTevColorChan, GXTevColorChan, GXTevColorChan, GXTevColorChan);
void GXSetTexCoordCylWrap(int, u8, u8);
void GXSetTexCoordGen2(GXTexCoordID, GXTexGenType, GXTexGenSrc, u32, GXBool, u32);
void GXSetTexCoordScaleManually(int, u8, u16, u16);
void GXSetVtxAttrFmt(GXVtxFmt, GXAttr, GXCompCnt, GXCompType, u8);
void GXSetVtxDesc(GXAttr, GXAttrType);
void GXSetZCompLoc(u8);
void GXSetZMode(GXBool, GXCompare, GXBool);
void GXSetZTexture(GXZTexOp op, GXTexFmt fmt, u32 bias);
void GXInitTexObjFilter(GXTexObj *, u32, u32);

// ---

static inline void GXSetTexCoordGen(GXTexCoordID dst_coord, GXTexGenType func,
                                    GXTexGenSrc src_param, u32 mtx)
{
	GXSetTexCoordGen2(dst_coord, func, src_param, mtx, FALSE, 0x7d);
}

void GXBegin(GXPrimitive, GXVtxFmt, u16);

static inline void GXEnd(void)
{
#if !defined(NDEBUG)
	extern GXBool __GXinBegin;

	if (!__GXinBegin)
	{
		OSError_FileLine("GXGeometry.h", 127,
		                 "GXEnd: called without a GXBegin");
	}

	__GXinBegin = FALSE;
#endif // !defined(NDEBUG)
}

union WGPipe
{
	u16	u16;	// size 0x02
	u32	u32;	// size 0x04
	f32	f32;	// size 0x04
}; // size 0x04

extern volatile union WGPipe __WGPipe AT_ADDRESS(0xcc008000);

extern void GXPosition2f32(f32 x, f32 y);
#if defined(NDEBUG)
inline void GXPosition2f32(f32 x, f32 y)
{
	__WGPipe.f32 = x;
	__WGPipe.f32 = y;
}
#endif

extern void GXPosition3f32(f32 x, f32 y, f32 z);
#if defined(NDEBUG)
inline void GXPosition3f32(f32 x, f32 y, f32 z)
{
	__WGPipe.f32 = x;
	__WGPipe.f32 = y;
	__WGPipe.f32 = z;
}
#endif

extern void GXColor1u32(u32 color);
#if defined(NDEBUG)
inline void GXColor1u32(u32 color)
{
	__WGPipe.u32 = color;
}
#endif

extern void GXTexCoord2u16(u16 u, u16 v);
#if defined(NDEBUG)
inline void GXTexCoord2u16(u16 u, u16 v)
{
	__WGPipe.u16 = u;
	__WGPipe.u16 = v;
}
#endif

extern void GXTexCoord2f32(f32 u, f32 v);
#if defined(NDEBUG)
inline void GXTexCoord2f32(f32 u, f32 v)
{
	__WGPipe.f32 = u;
	__WGPipe.f32 = v;
}
#endif

// IPC

extern u32 volatile __IPCRegs[] AT_ADDRESS(0xcd000000);

inline u32 ACRReadReg(u32 reg)
{
	return __IPCRegs[reg >> 2];
}

inline void ACRWriteReg(u32 reg, u32 val)
{
	__IPCRegs[reg >> 2] = val;
}

// MEM

typedef struct MEMiHeapHead MEMiHeapHead;
typedef struct MEMAllocator MEMAllocator;

// [SPQE7T]/ISpyD.elf:.debug_info::0x113386
typedef void *MEMAllocatorAllocFunc(MEMAllocator *pAllocator, u32 size);

// [SPQE7T]/ISpyD.elf:.debug_info::0x1133bb
typedef void MEMAllocatorFreeFunc(MEMAllocator *pAllocator, void *memBlock);

typedef struct MEMLink
{
	void	*prevObject;	// size 0x04, offset 0x00
	void	*nextObject;	// size 0x04, offset 0x04
} MEMLink; // size 0x08

typedef struct MEMList
{
	void	*headObject;	// size 0x04, offset 0x00
	void	*tailObject;	// size 0x04, offset 0x04
	u16		numObjects;		// size 0x02, offset 0x08
	u16		offset;			// size 0x02, offset 0x0a
} MEMList; // size 0x0c

// TODO(DWARF)
struct MEMiHeapHead
{
	u32		signature;	// size 0x04, offset 0x00
	MEMLink	link;		// size 0x08? offset 0x04
	MEMList	childList;	// size 0x0c? offset 0x0c
	void	*heapStart;	// size 0x04, offset 0x18
	void	*heapEnd;	// size 0x04, offset 0x1c
	OSMutex	mutex;		// size 0x18, offset 0x20

	// TODO(DWARF)
	union /* explicitly untagged */
	{
		u32	val;	// size 0x04

		// TODO(DWARF)
		struct /* explicitly untagged */
		{
			u32	_reserved	: 24;	// size 24, offset  0
			u32	optFlag		: 8;	// size 8,  offset 24
		} fields; // size 0x04
	} attribute;
};

// [SPQE7T]/ISpyD.elf:.debug_info::0x113345
typedef struct MEMAllocatorFunc
{
	MEMAllocatorAllocFunc	*pfAlloc;	// size 0x04, offset 0x00
	MEMAllocatorFreeFunc	*pfFree;	// size 0x04, offset 0x04
} MEMAllocatorFunc; // size 0x08

// TODO(DWARF)
struct MEMAllocator
{
	MEMAllocatorFunc	const *pFunc;	// size 0x04, offset 0x00
	void				*pHeap;			// size 0x04, offset 0x04
	u32					heapParam1;		// size 0x04, offset 0x08
	u32					heapParam2;		// size 0x04, offset 0x0c
}; // size 0x10

void *MEMAllocFromAllocator(MEMAllocator *allocator, u32 size);
void MEMFreeToAllocator(MEMAllocator *allocator, void *block);
void MEMInitAllocatorForExpHeap(MEMAllocator *allocator, MEMiHeapHead *heap,
                                s32 align);

MEMiHeapHead *MEMCreateExpHeapEx(void *start, u32 size, u16 opt);
MEMiHeapHead *MEMDestroyExpHeap(void *heap);
static inline MEMiHeapHead *MEMCreateExpHeap(void *startAddress, u32 size)
{
	return MEMCreateExpHeapEx(startAddress, size, 0);
}

// NAND

#define FS_MAX_PATH	64

typedef s32 NANDResult;
enum NANDResult_et
{
	NAND_RESULT_OK				= 0,

	NAND_RESULT_FATAL_ERROR		= -128,
	NAND_RESULT_UNKNOWN			= -64,

	NAND_RESULT_ACCESS			= -1,
	NAND_RESULT_ALLOC_FAILED	= -2,
	NAND_RESULT_BUSY			= -3,
	NAND_RESULT_CORRUPT			= -4,
	NAND_RESULT_ECC_CRIT		= -5,
	NAND_RESULT_EXISTS			= -6,

	NAND_RESULT_INVALID			= -8,
	NAND_RESULT_MAXBLOCKS		= -9,
	NAND_RESULT_MAXFD			= -10,
	NAND_RESULT_MAXFILES		= -11,
	NAND_RESULT_NOEXISTS		= -12,
	NAND_RESULT_NOTEMPTY		= -13,
	NAND_RESULT_OPENFD			= -14,
	NAND_RESULT_AUTHENTICATION	= -15,
	NAND_RESULT_MAXDEPTH		= -16,

#define NAND_ESUCCESS	NAND_RESULT_OK
#define NAND_EFATAL		NAND_RESULT_FATAL_ERROR
};

typedef u8 NANDOpenMode;
enum NANDOpenMode_et
{
	NAND_OPEN_NONE,

	NAND_OPEN_READ,
	NAND_OPEN_WRITE,
	NAND_OPEN_RW,
};

typedef enum NANDSeekMode
{
	NAND_SEEK_SET,
	NAND_SEEK_CUR,
	NAND_SEEK_END
} NANDSeekMode;

typedef struct NANDCommandBlock NANDCommandBlock;
typedef void NANDAsyncCallback(NANDResult result, NANDCommandBlock *block);

// [SPQE7T]/ISpyD.elf:.debug_info::0x283b92
typedef struct NANDFileInfo
{
	s32		fileDescriptor;			// size 0x04, offset 0x00
	s32		origFd;					// size 0x04, offset 0x04
	char	origPath[FS_MAX_PATH];	// size 0x40, offset 0x08
	char	tmpPath[FS_MAX_PATH];	// size 0x40, offset 0x48
	u8		accType;				// size 0x01, offset 0x88
	u8		stage;					// size 0x01, offset 0x89
	u8		mark;					// size 0x01, offset 0x8a
	// 1 byte padding
} NANDFileInfo; // size 0x8c

// [SPQE7T]/ISpyD.elf:.debug_info::0x2837aa
struct NANDCommandBlock
{
	void		*userData;				// size 0x04, offset 0x00
	void		*callback;				// size 0x04, offset 0x04 // not NANDAsyncCallback *?
	void		*fileInfo;				// size 0x04, offset 0x08 // not NANDFileInfo *?
	void		*bytes;					// size 0x04, offset 0x0c
	void		*inodes;				// size 0x04, offset 0x10
	void		*status;				// size 0x04, offset 0x14
	u16			ownerId;				// size 0x02, offset 0x18
	u16			groupId;				// size 0x02, offset 0x1a
	u8			nextStage;				// size 0x01, offset 0x1c
	/* 3 bytes padding */
	u32			attr;					// size 0x04, offset 0x20
	u32			ownerAcc;				// size 0x04, offset 0x24
	u32			groupAcc;				// size 0x04, offset 0x28
	u32			othersAcc;				// size 0x04, offset 0x2c
	u32			num;					// size 0x04, offset 0x30
	char		absPath[FS_MAX_PATH];	// size 0x40, offset 0x34
	u32			*length;				// size 0x04, offset 0x74
	u32			*pos;					// size 0x04, offset 0x78
	int			state;					// size 0x04, offset 0x7c
	void		*copyBuf;				// size 0x04, offset 0x80
	u32			bufLength;				// size 0x04, offset 0x84
	u8			*type;					// size 0x04, offset 0x88
	u32			uniqNo;					// size 0x04, offset 0x8c
	u32			reqBlocks;				// size 0x04, offset 0x90
	u32			reqInodes;				// size 0x04, offset 0x94
	u32			*answer;				// size 0x04, offset 0x98
	u32			homeBlocks;				// size 0x04, offset 0x9c
	u32			homeInodes;				// size 0x04, offset 0xa0
	u32			userBlocks;				// size 0x04, offset 0xa4
	u32			userInodes;				// size 0x04, offset 0xa8
	u32			workBlocks;				// size 0x04, offset 0xac
	u32			workInodes;				// size 0x04, offset 0xb0
	char		const **dir;			// size 0x04, offset 0xb4
	int			simpleFlag;				// size 0x04, offset 0xb8
}; // size 0xbc

NANDResult NANDReadAsync(NANDFileInfo *info, void *buf, u32 len,
                         NANDAsyncCallback *callback, NANDCommandBlock *block);
NANDResult NANDWriteAsync(NANDFileInfo *info, void const *buf, u32 len,
                          NANDAsyncCallback *callback, NANDCommandBlock *block);
NANDResult NANDSeekAsync(NANDFileInfo *info, s32 offset, NANDSeekMode whence,
                         NANDAsyncCallback *callback, NANDCommandBlock *block);

NANDResult NANDInit(void);

NANDResult NANDOpenAsync(char const *path, NANDFileInfo *info, NANDOpenMode mode,
                         NANDAsyncCallback *callback, NANDCommandBlock *block);
NANDResult NANDCloseAsync(NANDFileInfo *info, NANDAsyncCallback *callback,
                          NANDCommandBlock *block);

// SC

typedef u32 SCStatus;
enum SCStatus_et
{
	SC_STATUS_OK,	/* name known from asserts */
	SC_STATUS_BUSY,
	SC_STATUS_FATAL,
	SC_STATUS_PARSE,
};

typedef void SCFlushCallback(SCStatus status);

void SCInit(void);
SCStatus SCCheckStatus(void);

void SCFlushAsync(SCFlushCallback *cb);

typedef u8 SCSoundMode;
enum SCSoundMode_et
{
	SC_SND_MONO,
	SC_SND_STEREO,
	SC_SND_SURROUND
};

typedef u8 SCSensorBarPos;
enum SCSensorBarPos_et
{
	SC_SENSOR_BAR_BOTTOM,
	SC_SENSOR_BAR_TOP,
};

typedef struct SCBtDeviceInfo // basic dev info?
{
	BD_ADDR					devAddr;	// size 0x06, offset 0x00
	struct small_dev_info	small;		// size 0x40, offset 0x06
} SCBtDeviceInfo; // size 0x46

typedef struct SCBtDeviceInfoArray
{
	u8				num;			// size 0x001, offset 0x000	/* name known from asserts */

	/* NOTE: contents of the devices member are actually
	 *
	 * SCBtDeviceInfo	registered[WUD_MAX_DEV_ENTRY_FOR_STD];
	 * SCBtDeviceInfo	active[WUD_MAX_CHANNELS];
	 *
	 * but functions in WUD act as if it's only one buffer (see
	 * __wudSecurityEventStackCallback, case BTA_DM_LINK_DOWN_EVT)
	 */
	SCBtDeviceInfo	devices[14];	// size 0x3d4, offset 0x001

	SCBtDeviceInfo	wbc;			// size 0x046, offset 0x3d5
	SCBtDeviceInfo	at_0x41b;		// size 0x046, offset 0x41b
} SCBtDeviceInfoArray; // size 0x461

typedef struct SCBtCmpDevInfo // complex dev info?
{
	BD_ADDR					devAddr;	// size 0x06, offset 0x00
	struct small_dev_info	small;		// size 0x40, offset 0x06
	LINK_KEY				linkKey;	// size 0x10, offset 0x30 // ? or just a buffer
} SCBtCmpDevInfo; // size 0x56

typedef struct SCBtCmpDevInfoArray
{
	u8				num;			// size 0x001, offset 0x000 /* name known from asserts */
	SCBtCmpDevInfo	devices[6];		// size 0x204, offset 0x001
} SCBtCmpDevInfoArray; // size 0x205

SCSoundMode SCGetSoundMode(void);
void SCGetBtDeviceInfoArray(SCBtDeviceInfoArray *array);
BOOL SCSetBtDeviceInfoArray(SCBtDeviceInfoArray const *array);
void SCGetBtCmpDevInfoArray(SCBtCmpDevInfoArray *array);
BOOL SCSetBtCmpDevInfoArray(SCBtCmpDevInfoArray const *array);
u32 SCGetBtDpdSensibility(void);
BOOL SCSetBtDpdSensibility(u8 sens);
u8 SCGetWpadMotorMode(void);
BOOL SCSetWpadMotorMode(u8 mode);
SCSensorBarPos SCGetWpadSensorBarPosition(void);
u8 SCGetWpadSpeakerVolume(void);
BOOL SCSetWpadSpeakerVolume(u8 vol);

typedef s8 SCProductGameRegion;
enum SCProductGameRegion_et
{
	SC_PRD_GAME_REG_JP,
	SC_PRD_GAME_REG_US,
	SC_PRD_GAME_REG_EU,
	SC_PRD_GAME_REG_DEBUG,

	SC_PRD_GAME_REG_KR,
	SC_PRD_GAME_REG_CN,

	SC_PRD_GAME_REG_MAX,
	SC_PRD_GAME_REG_NULL	= -1
};

SCProductGameRegion SCGetProductGameRegion(void);

// VI

void __VIResetRFIdle(void);
void VISetBlack(BOOL black);
void VIFlush(void);

#ifdef __cplusplus
	}
#endif

#endif // CONTEXT_RVL_H
