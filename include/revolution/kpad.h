#ifndef RVL_SDK_KPAD_H
#define RVL_SDK_KPAD_H

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

#if 0
#include <revolution/mtx/vec.h>
#else
#include <context_rvl.h>
#endif

#include <revolution/wpad.h>

/*******************************************************************************
 * types
 */

#ifdef __cplusplus
	extern "C" {
#endif

typedef s32 KPADResult;
enum KPADResult_et
{
	KPAD_ERR_OK	= 0,
};

#define KPAD_ESUCCESS	KPAD_ERR_OK

typedef WPADChannel KPADChannel;
enum KPADChannel_et
{
	KPAD_CHAN0 = WPAD_CHAN0,
	KPAD_CHAN1 = WPAD_CHAN1,
	KPAD_CHAN2 = WPAD_CHAN2,
	KPAD_CHAN3 = WPAD_CHAN3,

	KPAD_MAX_CONTROLLERS = WPAD_MAX_CONTROLLERS,

	KPAD_CHAN_INVALID = WPAD_CHAN_INVALID
};

typedef WPADMplsCommand KPADMplsCommand;
enum KPADMplsCommand_et
{
	KPAD_MPLS_DISABLE	= WPAD_MPLS_DISABLE,

	KPAD_MPLS_MAIN		= WPAD_MPLS_MAIN,
	KPAD_MPLS_FS		= WPAD_MPLS_FS,
	KPAD_MPLS_CLASSIC	= WPAD_MPLS_CLASSIC,

	KPAD_MPLS_CMD_80	= WPAD_MPLS_CMD_80,
};

typedef enum KPADMplsZeroDriftMode
{
	KPAD_MPLS_ZERO_DRIFT_LOOSE,
	KPAD_MPLS_ZERO_DRIFT_STANDARD,
	KPAD_MPLS_ZERO_DRIFT_TIGHT,
} KPADMplsZeroDriftMode;

typedef WPADCallback KPADCallback;

typedef WPADSamplingCallback KPADSamplingCallback;
typedef WPADConnectCallback KPADConnectCallback;
typedef WPADExtensionCallback KPADExtensionCallback;

typedef WPADSimpleSyncCallback KPADSimpleSyncCallback;

// [SPQE7T]/ISpyD.elf:.debug_info::0xd66f9
typedef struct KPADMPDir
{
	Vec	X;	// offset 0x00, size 0x0c
	Vec	Y;	// offset 0x0c, size 0x0c
	Vec	Z;	// offset 0x18, size 0x0c
} KPADMPDir; // size 0x24

// [SPQE7T]/ISpyD.elf:.debug_info::0xd66b8
typedef struct KPADMPStatus
{
	Vec			mpls;	// offset 0x00, size 0x0c
	Vec			angle;	// offset 0x0c, size 0x0c
	KPADMPDir	dir;	// offset 0x18, size 0x24
} KPADMPStatus; // size 0x3c

// [SPQE7T]/ISpyD.elf:.debug_info::0xd650e
typedef union KPADEXStatus
{
	// [SPQE7T]/ISpyD.elf:.debug_info::0xd6549
	struct
	{
		Vec2	stick;		// offset 0x00, size 0x08
		Vec		acc;		// offset 0x08, size 0x0c
		f32		acc_value;	// offset 0x14, size 0x04
		f32		acc_speed;	// offset 0x18, size 0x04
	} fs; // size 0x1c

	// [SPQE7T]/ISpyD.elf:.debug_info::0xd659d
	struct
	{
		u32		hold;		// offset 0x00, size 0x04
		u32		trig;		// offset 0x04, size 0x04
		u32		release;	// offset 0x08, size 0x04
		Vec2	lstick;		// offset 0x0c, size 0x08
		Vec2	rstick;		// offset 0x14, size 0x08
		f32		ltrigger;	// offset 0x1c, size 0x04
		f32		rtrigger;	// offset 0x20, size 0x04
	} cl; // size 0x24

	// [SPQE7T]/ISpyD.elf:.debug_info::0xd6623
	struct
	{
		f64	tgc_weight;		// offset 0x00, size 0x08
		f64	weight[4];		// offset 0x08, size 0x20
		f64	weight_ave[4];	// offset 0x28, size 0x20
		s32	weight_err;		// offset 0x48, size 0x04
		s32	tgc_weight_err;	// offset 0x4c, size 0x04
	} bl; // size 0x50
} KPADEXStatus;

// [SPQE7T]/ISpyD.elf:.debug_info::0xd6300
typedef struct KPADStatus
{
	byte4_t			hold;			// offset 0x00, size 0x04
	byte4_t			trig;			// offset 0x04, size 0x04
	byte4_t			release;		// offset 0x08, size 0x04
	Vec				acc;			// offset 0x0c, size 0x0c
	f32				acc_value;		// offset 0x18, size 0x04
	f32				acc_speed;		// offset 0x1c, size 0x04
	Vec2			pos;			// offset 0x20, size 0x08
	Vec2			vec;			// offset 0x28, size 0x08
	f32				speed;			// offset 0x30, size 0x04
	Vec2			horizon;		// offset 0x34, size 0x08
	Vec2			hori_vec;		// offset 0x3c, size 0x08
	f32				hori_speed;		// offset 0x44, size 0x04
	f32				dist;			// offset 0x48, size 0x04
	f32				dist_vec;		// offset 0x4c, size 0x04
	f32				dist_speed;		// offset 0x50, size 0x04
	Vec2			acc_vertical;	// offset 0x54, size 0x08
	u8				dev_type;		// offset 0x5c, size 0x01
	s8				wpad_err;		// offset 0x5d, size 0x01
	s8				dpd_valid_fg;	// offset 0x5e, size 0x01
	u8				data_format;	// offset 0x5f, size 0x01
	KPADEXStatus	ex_status;		// offset 0x60, size 0x50
	KPADMPStatus	mpls;			// offset 0xb0, size 0x3c
	byte1_t			__paddings__[4]; // ??? is this the compiler?
} KPADStatus; // size 0xf0

typedef KPADStatus KPADStatusSamplingBuf[16];

// [SGLEA4]/GormitiDebug.elf:.debug_info::0x7aa65
typedef struct KPADUnifiedWpadStatus
{
	// [SGLEA4]/GormitiDebug.elf:.debug_info::0x7aaae
	union /* explicitly untagged */
	{
		WPADStatus		core;	// size 0x2a
		WPADFSStatus	fs;		// size 0x32
		WPADCLStatus	cl;		// size 0x36
		WPADTRStatus	tr;		// size 0x2e
		WPADBLStatus	bl;		// size 0x34
		WPADMPStatus	mp;		// size 0x3e
		WPADBKStatus	bk;		// size 0x40
	} u; // size 0x40, offset 0x00
	u8					fmt;	// size 0x01, offset 0x40
	byte1_t				padding;
} KPADUnifiedWpadStatus; // size 0x42

typedef struct KPADConfig
{
	byte1_t	pad0_[4]; // ?
} KPADConfig;

/*******************************************************************************
 * functions
 */
void KPADSetFSStickClamp(s8 min, s8 max);
void KPADSetBtnRepeat(KPADChannel chan, f32, f32);
void KPADSetObjInterval(f32);

void KPADSetPosParam(KPADChannel chan, f32 x, f32 y);
void KPADSetHoriParam(KPADChannel chan, f32 x, f32 y);
void KPADSetDistParam(KPADChannel chan, f32 x, f32 y);
void KPADSetAccParam(KPADChannel chan, f32 x, f32 y);
void KPADGetPosParam(KPADChannel chan, f32 *x, f32 *y);
void KPADGetHoriParam(KPADChannel chan, f32 *x, f32 *y);
void KPADGetDistParam(KPADChannel chan, f32 *x, f32 *y);
void KPADGetAccParam(KPADChannel chan, f32 *x, f32 *y);

void KPADGetProjectionPos(f32, f32 *, f32 const *, f32 const *);
s32 KPADCalibrateDPD(KPADChannel chan);
void KPADDisableAimingMode(KPADChannel chan);
void KPADEnableAimingMode(KPADChannel chan);
u8 KPADIsEnableAimingMode(KPADChannel chan);
void KPADSetSensorHeight(KPADChannel chan, f32 sensorHeight);
f32 KPADGetSensorHeight(KPADChannel chan);

s32 KPADRead(KPADChannel chan, KPADStatus *status, s32 count);
s32 KPADReadEx(KPADChannel chan, KPADStatus *status, s32 count,
               KPADResult *result);

void KPADInit(void);
void KPADInitEx(KPADUnifiedWpadStatus *uwStatus, u32 length);
void KPADShutdown(void);
void KPADReset(void);

KPADConnectCallback *KPADSetConnectCallback(KPADChannel chan,
                                            KPADConnectCallback *cb);

void KPADDisableDPD(KPADChannel chan);
void KPADEnableDPD(KPADChannel chan);
void KPADSetControlDpdCallback(KPADChannel chan, KPADCallback *cb);
void KPADResetWbcZeroPoint(void);
void KPADResetWbcTgcWeight(void);

void KPADDisableMpls(KPADChannel chan);
void KPADEnableMpls(KPADChannel chan, KPADMplsCommand cmd);
u8 KPADGetMplsStatus(KPADChannel chan);
void KPADSetControlMplsCallback(KPADChannel chan, KPADCallback *cb);

u32 KPADGetMplsWorkSize(void);
void KPADSetMplsWorkarea(void *work);

void KPADSetMplsAngle(KPADChannel chan, f32 x, f32 y, f32 z);
void KPADSetMplsDirection(KPADChannel chan, void *);
void KPADSetMplsDirectionMag(KPADChannel chan, f32 mag);

void KPADStartMplsCalibration(KPADChannel chan);
f32 KPADWorkMplsCalibration(KPADChannel chan);
void KPADStopMplsCalibration(KPADChannel chan);

void KPADEnableMplsZeroPlay(KPADChannel chan);
void KPADEnableMplsDirRevise(KPADChannel chan);
void KPADEnableMplsAccRevise(KPADChannel chan);
void KPADEnableMplsDpdRevise(KPADChannel chan);

void KPADDisableMplsZeroPlay(KPADChannel chan);
void KPADDisableMplsDirRevise(KPADChannel chan);
void KPADDisableMplsAccRevise(KPADChannel chan);
void KPADDisableMplsDpdRevise(KPADChannel chan);

f32 KPADIsEnableMplsZeroPlay(KPADChannel chan);
f32 KPADIsEnableMplsZeroDrift(KPADChannel chan);
f32 KPADIsEnableMplsDirRevise(KPADChannel chan);
f32 KPADIsEnableMplsAccRevise(KPADChannel chan);
f32 KPADIsEnableMplsDpdRevise(KPADChannel chan);

void KPADSetMplsZeroPlayParam(KPADChannel chan, f32 param1);
void KPADSetMplsDirReviseParam(KPADChannel chan, f32 param1);
void KPADSetMplsAccReviseParam(KPADChannel chan, f32 param1, f32 param2);
void KPADSetMplsDpdReviseParam(KPADChannel chan, f32 param1);
void KPADSetMplsDirReviseBase(KPADChannel chan, KPADMPDir *base);

void KPADGetMplsZeroPlayParam(KPADChannel chan, f32 *param1);
void KPADGetMplsDirReviseParam(KPADChannel chan, f32 *param1);
void KPADGetMplsAccReviseParam(KPADChannel chan, f32 *param1, f32 *param2);
void KPADGetMplsDpdReviseParam(KPADChannel chan, f32 *param1);

void KPADInitMplsZeroPlayParam(KPADChannel chan);
void KPADInitMplsDirReviseParam(KPADChannel chan);
void KPADInitMplsAccReviseParam(KPADChannel chan);
void KPADInitMplsDpdReviseParam(KPADChannel chan);
void KPADInitMplsZeroDriftMode(KPADChannel chan);

void KPADSetMplsZeroDriftMode(KPADChannel chan, KPADMplsZeroDriftMode mode);
void KPADGetMplsZeroDriftMode(KPADChannel chan, KPADMplsZeroDriftMode *mode);

void KPADResetMpls(KPADChannel chan);

KPADSamplingCallback *KPADSetSamplingCallback(KPADChannel chan,
                                              KPADSamplingCallback *cb);
void KPADGetUnifiedWpadStatus(KPADChannel chan, KPADUnifiedWpadStatus *uwStatus,
                              u32 count);
void KPADEnableStickCrossClamp(void);
void KPADDisableStickCrossClamp(void);
void KPADSetReviseMode(KPADChannel chan, BOOL enabled);
f32 KPADReviseAcc(Vec *acc);
f32 KPADGetReviseAngle(void);
void KPADSetPosPlayMode(KPADChannel chan, BOOL enabled);
void KPADSetDistPlayMode(KPADChannel chan, BOOL enabled);
void KPADSetHoriPlayMode(KPADChannel chan, BOOL enabled);
void KPADSetAccPlayMode(KPADChannel chan, BOOL enabled);
BOOL KPADGetPosPlayMode(KPADChannel chan);
BOOL KPADGetHoriPlayMode(KPADChannel chan);
BOOL KPADGetDistPlayMode(KPADChannel chan);
BOOL KPADGetAccPlayMode(KPADChannel chan);
void KPADSetButtonProcMode(KPADChannel chan, u8 mode);
u8 KPADGetButtonProcMode(KPADChannel chan);
BOOL KPADSetConfig(KPADChannel chan, KPADConfig *config);
void KPADSetMplsMagnification(KPADChannel chan, f32 x, f32 y, f32 z);

#ifdef __cplusplus
	}
#endif

#endif // RVL_SDK_KPAD_H
