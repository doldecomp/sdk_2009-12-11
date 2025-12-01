#include <revolution/kpad.h>

/*******************************************************************************
 * headers
 */

#include <math.h>
#include <string.h> // memset

#include <macros.h>

#include "buildstamp.h"
#include <revolution/types.h>

#include "kmpls_main.h"

#include <revolution/wpad.h>
#include "../wpad/WPAD.h"

#if 0
#include <revolution/os/OSAssert.h>
#include <revolution/os/OSInterrupt.h>
#include <revolution/os/OSTime.h>
#include <revolution/mtx/mtxvec.h>
#include <revolution/wpad.h>
#else
#include <context_rvl.h>
#endif

/*******************************************************************************
 * macros
 */

#undef NULL
#define NULL ((void *)0) // for asserts on debug

#define RVL_SDK_KPAD_DEBUG_BUILD_DATE	"Dec 11 2009"
#define RVL_SDK_KPAD_DEBUG_BUILD_TIME	"15:53:49"

#define RVL_SDK_KPAD_RELEASE_BUILD_DATE	"Dec 11 2009"
#define RVL_SDK_KPAD_RELEASE_BUILD_TIME	"15:58:25"

#define MAX_WBC_FAILURE_COUNT	3

/*******************************************************************************
 * types
 */

typedef s8 WBCEnableState;
enum WBCEnableState_et
{
	WBCEnableFailed		= -1,
	WBCEnableNotStarted	= 0,
	WBCEnableStage1		= 1,
	WBCEnableStage2		= 2,
	WBCEnableSucceeded	= 3
};

typedef void ClampFunc(Vec2 *stick, s32 x, s32 y, s32 max, s32 min);

struct kobj
{
	Vec2	pos;		// size 0x08, offset 0x00
	char	flags;		// size 0x01, offset 0x08
	char	inactive;	// size 0x01, offset 0x09
	/* 2 bytes padding */
}; // size 0x0c

// No tag name: [SGLEA4]/MAP/GormitiDebug.MAP:101005-101007
typedef struct /* explicitly untagged */
{
	KPADStatus				status;					// size 0x0f0, offset 0x000
	Vec2					posParam;				// size 0x008, offset 0x0f0
	Vec2					horiParam;				// size 0x008, offset 0x0f8
	Vec2					distParam;				// size 0x008, offset 0x100
	Vec2					accParam;				// size 0x008, offset 0x108
	f32						idist;					// size 0x004, offset 0x110
	Vec2					iaccXY_nrm_hori;		// size 0x008, offset 0x114
	Vec2					isec_nrm_hori;			// size 0x008, offset 0x11c
	Vec2					sensorPos;				// size 0x008, offset 0x124
	f32						dpd2pos_scale;			// size 0x004, offset 0x12c
	struct kobj				kobjs[4];				// size 0x030, offset 0x130
	struct kobj				selected[2];			// size 0x018, offset 0x160
	s16						at_0x178;				// size 0x002, offset 0x178
	u8						localSamplingBufIndex;	// size 0x001, offset 0x17a
	u8						at_0x17b;				// size 0x001, offset 0x17b
	KPADStatus				*appSamplingBufPtr;		// size 0x004, offset 0x17c /* name known from asserts */
	KPADUnifiedWpadStatus	localSamplingBuf[16];	// size 0x420, offset 0x180
	KPADUnifiedWpadStatus	*appSamplingBuf;		// size 0x004, offset 0x5a0
	u32						appSamplingBufIndex;	// size 0x004, offset 0x5a4
	f32						at_0x5a8;				// size 0x004, offset 0x5a8
	Vec2					at_0x5ac;				// size 0x008, offset 0x5ac
	f32						at_0x5b4;				// size 0x004, offset 0x5b4
	f32						at_0x5b8;				// size 0x004, offset 0x5b8
	Vec						acc;					// size 0x00c, offset 0x5bc
	Vec2					horizon;				// size 0x008, offset 0x5c8
	Vec2					at_0x5d0;				// size 0x008, offset 0x5d0
	Vec2					pos;					// size 0x008, offset 0x5d8
	Vec2					at_0x5e0;				// size 0x008, offset 0x5e0
	u16						ah_circle_ct;			// size 0x002, offset 0x5e8
	u8						at_0x5ea;				// size 0x001, offset 0x5ea
	u8						currentlyReading;		// size 0x001, offset 0x5eb
	u16						at_0x5ec;				// size 0x002, offset 0x5ec
	u16						at_0x5ee;				// size 0x002, offset 0x5ee
	u16						at_0x5f0;				// size 0x002, offset 0x5f0
	u16						at_0x5f2;				// size 0x002, offset 0x5f2
	u16						at_0x5f4;				// size 0x002, offset 0x5f4
	u16						at_0x5f6;				// size 0x002, offset 0x5f6
	WPADCallback			*controlDpdCb;			// size 0x004, offset 0x5f8
	Vec						coreGravityUnit;		// size 0x00c, offset 0x5fc
	Vec						fsGravityUnit;			// size 0x00c, offset 0x608
	f32						frameBorderLeft;		// size 0x008, offset 0x614
	f32						frameBorderTop;			// size 0x008, offset 0x618
	f32						frameBorderRight;		// size 0x008, offset 0x61c
	f32						frameBorderBottom;		// size 0x008, offset 0x620
	f32						err_dist_speed_max;		// size 0x004, offset 0x624
	f32						err_dist_speed_min;		// size 0x004, offset 0x628
	f32						ah_circle_r2;			// size 0x004, offset 0x62c
	f32						dist_vv1;				// size 0x004, offset 0x630
	f32						err_dist_min;			// size 0x004, offset 0x634
	WPADSamplingCallback	*samplingCb;			// size 0x004, offset 0x638
	WPADConnectCallback		*connectCb;				// size 0x004, offset 0x63c
	u8						needsReset;				// size 0x001, offset 0x640
	u8						isNewDevType;			// size 0x001, offset 0x641
	u8						dpdEnabled;				// size 0x001, offset 0x642
	u8						wpadDpdEnabled;			// size 0x001, offset 0x643
	u8						controllingDpd;			// size 0x001, offset 0x644
	u8						wpadDpdCmd;				// size 0x001, offset 0x645
	u8						at_0x646;				// size 0x001, offset 0x646
	u8						at_0x647;				// size 0x001, offset 0x647
	u8						aimingModeChanged;		// size 0x001, offset 0x648
	u8						aimingModeEnabled;		// size 0x001, offset 0x649
	u8						reviseEnabled;			// size 0x001, offset 0x64a
	u8						buttonProcMode;			// size 0x001, offset 0x64b
	BOOL					posPlayMode;			// size 0x004, offset 0x64c
	BOOL					horiPlayMode;			// size 0x004, offset 0x650
	BOOL					distPlayMode;			// size 0x004, offset 0x654
	BOOL					accPlayMode;			// size 0x004, offset 0x658
	WPADDeviceType			devType;				// size 0x004, offset 0x65c
	OSTick					tickQueue[5];			// size 0x014, offset 0x660
	u8						at_0x674;				// size 0x001, offset 0x674
	u8						at_0x675;				// size 0x001, offset 0x675
	u8						mplsDevMode1;			// size 0x001, offset 0x676
	u8						activeMplsCmd;			// size 0x001, offset 0x677
	u8						mplsDevMode3;			// size 0x001, offset 0x678
	u8						pendingMplsCmd;			// size 0x001, offset 0x679
	u8						mplsWasSampled;			// size 0x001, offset 0x67a
	u8						at_0x67b;				// size 0x001, offset 0x67b
	u8						at_0x67c;				// size 0x001, offset 0x67c
	u8						at_0x67d;				// size 0x001, offset 0x67d
	u8						needsNewGravityUnit;	// size 0x001, offset 0x67e
	/* 1 byte padding */
	KPADCallback			*controlMplsCb;			// size 0x004, offset 0x680
	/* 4 bytes padding */
} inside_kpads_st; // size 0x688

/*******************************************************************************
 * local function declarations
 */

static void set_obj_interval(f32);

static void calc_dpd2pos_scale(inside_kpads_st *kp);
static void reset_kpad(KPADChannel chan);
static void calc_button_repeat(inside_kpads_st *kp, WPADDeviceType devType,
                               s32);
static void read_kpad_button(inside_kpads_st *kp, WPADDeviceType devType, s32,
                             byte4_t coreButton, byte4_t fsButton,
                             byte4_t clButton);
static void calc_acc(inside_kpads_st *kp, f32 *acc, f32);
static void calc_acc_horizon(inside_kpads_st *kp);
static void calc_acc_vertical(inside_kpads_st *kp);
static f32 clamp_acc(f32 value, f32 mag);
static void read_kpad_acc(inside_kpads_st *kp, KPADUnifiedWpadStatus *uwStatus);
static void get_kobj(inside_kpads_st *kp, DPDObject *dpdObj);
static void check_kobj_outside_frame(inside_kpads_st *kp, struct kobj kobj[]);
static void check_kobj_same_position(struct kobj kobjs[]);
static f32 calc_horizon(inside_kpads_st *kp, struct kobj *k1, struct kobj *k2,
                        f32 out[2]);
static s8 select_2obj_first(inside_kpads_st *kp);
static s8 select_2obj_continue(inside_kpads_st *kp);
static s8 select_1obj_first(inside_kpads_st *kp);
static s8 select_1obj_continue(inside_kpads_st *kp);
static void calc_obj_horizon(inside_kpads_st *kp);
static void calc_dpd_variable(inside_kpads_st *kp, s8);
static void read_kpad_dpd(inside_kpads_st *kp, KPADUnifiedWpadStatus *uwStatus);
static void clamp_trigger(f32 *out, s32 val, s32 min, s32 max);
static void clamp_stick_circle(Vec2 *stick, s32 x, s32 y, s32 min, s32 max);
static void clamp_stick_cross(Vec2 *stick, s32 x, s32 y, s32 min, s32 max);
static void read_kpad_ext(inside_kpads_st *kp, KPADUnifiedWpadStatus *uwStatus);
static void read_kpad_mpls(KPADChannel chan, s32 offset, s32 count, f64);
static void KPADiMplsSamplingCallback(KPADChannel chan, KPADMPStatus *mpStatus);

static s32 KPADiRead(KPADChannel chan, KPADStatus *status, s32 count,
                     KPADResult *result, BOOL exRead);

static void KPADiConnectCallback(KPADChannel chan, WPADResult result);

static void KPADiControlWbcCallback(KPADChannel chan, WPADResult result);
static void KPADiUpdateTempWbcCallback(KPADChannel chan, WPADResult result);

static void KPADiControlDpdCallback(KPADChannel chan, WPADResult result);

static void KPADiControlMplsCallback(KPADChannel chan, WPADResult result);

static void KPADiSamplingCallback(KPADChannel chan);

/*******************************************************************************
 * variables
 */

// .data, .sdata
const char *__KPADVersion = RVL_SDK_LIB_VERSION_STRING(KPAD);

// .bss
static Vec Vec_0 ATTR_UNUSED; // Literally how did this get emitted
inside_kpads_st inside_kpads[WPAD_MAX_CONTROLLERS]; // why is this global?
static Mtx kp_fs_rot;
static f64 kp_wbc_weight_ave[4];
static f64 kp_wbc_ave_sample[4];
static KPADConfig kp_config[KPAD_MAX_CONTROLLERS];

// .sdata
static f32 idist_org = 1.0f;
static Vec2 iaccXY_nrm_hori = {0.0f, -1.0f};
static Vec2 isec_nrm_hori = {1.0f, 0.0f};

f32 kp_obj_interval = 0.2f;
f32 kp_acc_horizon_pw = 0.05f;

f32 kp_ah_circle_radius = 0.07f;
f32 kp_ah_circle_pw = 0.06f;
u16 kp_ah_circle_ct = 100;

f32 kp_err_outside_frame = 0.05f;
/* f32 kp_err_dist_min = 0.0f; */ // see .sbss
f32 kp_err_dist_max = 3.0f;
f32 kp_err_dist_speed = 0.04f;

f32 kp_err_first_inpr = 0.9f;
f32 kp_err_next_inpr = 0.9f;
f32 kp_err_acc_inpr = 0.9f;
f32 kp_err_up_inpr = 0.7f;
f32 kp_err_near_pos = 0.1f;

static u32 kp_fs_fstick_min = 15;
static u32 kp_fs_fstick_max = 71;

static u32 kp_cl_stick_min = 60;
static u32 kp_cl_stick_max = 308;

static u32 kp_cl_trigger_min = 30;
static u32 kp_cl_trigger_max = 180;

static f32 kp_rm_acc_max = 3.4f;
static f32 kp_fs_acc_max = 2.1f;

static u32 kp_ex_trigger_max = 256;
static u32 kp_ex_analog_max = 1024;

static f32 kp_wbc_ave_count = 400.0f;
static u8 kp_wbc_calib_count = 200;
static f32 kp_fs_revise_deg = 24.0f;

// .sbss
static f32 kp_err_dist_min; // see above
static f32 kp_dist_vv1;
static u8 kp_wbc_issued;
static WBCEnableState kp_wbc_enabled;
static s8 kp_wbc_tgc_weight_err;
static s16 kp_wbc_tgc_weight_wait;
static u8 kp_wbc_tgc_weight_done;
static u8 kp_wbc_zero_point_wait;
static s8 kp_wbc_zero_point_done;
static u8 kp_wbc_failure_count;
static u16 kp_wbc_ave_sample_count;
static f64 kp_wbc_tgc_weight;
static f64 kp_wbc_min_weight;
static f64 kp_wbc_max_weight;
static Vec2 Vec2_0;
static u8 kp_initialized;
static s32 kp_ex_analog_min;
static s32 kp_ex_trigger_min;
BOOL kp_stick_clamp_cross;
static Vec2 icenter_org;

/*******************************************************************************
 * functions
 */

void KPADSetFSStickClamp(s8 min, s8 max)
{
#if !defined(NDEBUG)
	// just needs to be anywhere before the first reference to inside_kpads
	(void)Vec_0; // ????????????????????
#endif

	kp_fs_fstick_min = min;
	kp_fs_fstick_max = max;
}

void KPADSetBtnRepeat(KPADChannel chan, f32 param_2, f32 param_3)
{
	// ERRATUM: uses chan before assertion
	inside_kpads_st *kp = &inside_kpads[chan];

	OSAssert_Line(149, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	if (param_3)
	{
		kp->at_0x5f0 = (int)(param_2 * 200.0f + 0.5f);
		kp->at_0x5f2 = (int)(param_3 * 200.0f + 0.5f);
	}
	else
	{
		kp->at_0x5f0 = 40000;
		kp->at_0x5f2 = 0;
	}

	kp->at_0x5ec = 0;
	kp->at_0x5ee = kp->at_0x5f0;

	kp->at_0x5f4 = 0;
	kp->at_0x5f6 = kp->at_0x5f0;
}

void KPADSetObjInterval(f32 param_1 ATTR_UNUSED)
{
	/* ... */
}

static void set_obj_interval(f32 interval)
{
	BOOL intrStatus = OSDisableInterrupts();

	// TODO: how to derive constant
	kp_obj_interval	= interval;
	kp_err_dist_min	= interval / 0.383864f;
	kp_dist_vv1		= interval / 0.383864f;

	OSRestoreInterrupts(intrStatus);
}

void KPADSetPosParam(KPADChannel chan, f32 x, f32 y)
{
	OSAssert_Line(200, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	inside_kpads[chan].posParam.x	= x;
	inside_kpads[chan].posParam.y	= y;
}

void KPADSetHoriParam(KPADChannel chan, f32 x, f32 y)
{
	OSAssert_Line(207, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	inside_kpads[chan].horiParam.x	= x;
	inside_kpads[chan].horiParam.y	= y;
}

void KPADSetDistParam(KPADChannel chan, f32 x, f32 y)
{
	OSAssert_Line(214, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	inside_kpads[chan].distParam.x	= x;
	inside_kpads[chan].distParam.y	= y;
}

void KPADSetAccParam(KPADChannel chan, f32 x, f32 y)
{
	OSAssert_Line(221, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	inside_kpads[chan].accParam.x	= x;
	inside_kpads[chan].accParam.y	= y;
}

void KPADGetPosParam(KPADChannel chan, f32 *x, f32 *y)
{
	OSAssert_Line(231, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	*x = inside_kpads[chan].posParam.x;
	*y = inside_kpads[chan].posParam.y;
}

void KPADGetHoriParam(KPADChannel chan, f32 *x, f32 *y)
{
	OSAssert_Line(238, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	*x = inside_kpads[chan].horiParam.x;
	*y = inside_kpads[chan].horiParam.y;
}

void KPADGetDistParam(KPADChannel chan, f32 *x, f32 *y)
{
	OSAssert_Line(245, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	*x = inside_kpads[chan].distParam.x;
	*y = inside_kpads[chan].distParam.y;
}

void KPADGetAccParam(KPADChannel chan, f32 *x, f32 *y)
{
	OSAssert_Line(252, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	*x = inside_kpads[chan].accParam.x;
	*y = inside_kpads[chan].accParam.y;
}

static void calc_dpd2pos_scale(inside_kpads_st *kp)
{
	// configurable macro constants?
	f32 x = 1.0f;
	f32 y = 0.75f;
	f32 hypot = sqrtf(x * x + y * y);

	if (kp->sensorPos.x < 0.0f)
		x += kp->sensorPos.x;
	else
		x -= kp->sensorPos.x;

	if (kp->sensorPos.y < 0.0f)
		y += kp->sensorPos.y;
	else
		y -= kp->sensorPos.y;

	kp->dpd2pos_scale = hypot / MIN(x, y);
}

static void reset_kpad(KPADChannel chan)
{
	inside_kpads_st *kp;
	struct kobj *kobj;
	KPADStatus *kpStatus;
	KPADEXStatus *kpExStatus ATTR_UNUSED;

	kp = &inside_kpads[chan];
	kpStatus = &kp->status;
	kpExStatus = &kp->status.ex_status;

	kp->needsReset				= false;

	kp->frameBorderLeft			= -1.00f + kp_err_outside_frame;
	kp->frameBorderRight		= 1.00f - kp_err_outside_frame;

	kp->frameBorderTop			= -0.75f + kp_err_outside_frame;
	kp->frameBorderBottom		= 0.75f - kp_err_outside_frame;

	kp->err_dist_speed_max		= 1.00f / kp_err_dist_speed;
	kp->err_dist_speed_min		= -1.00f / kp_err_dist_speed;

	kp->ah_circle_r2			= kp_ah_circle_radius * kp_ah_circle_radius;
	kp->err_dist_min			= kp_err_dist_min;
	kp->dist_vv1				= kp_dist_vv1;

	kpStatus->hold = kpStatus->trig = kpStatus->release = WPAD_BUTTON_NONE;

	kp->at_0x5ec				= 0;
	kp->at_0x5ee				= kp->at_0x5f0;

	kpStatus->dpd_valid_fg		= 0;
	kp->at_0x5ea				= 0;

	kpStatus->pos = kpStatus->vec = Vec2_0;
	kpStatus->speed				= 0.0f;

	kpStatus->horizon.x = kp->at_0x5d0.x = kp->horizon.x = 1.0f;
	kpStatus->horizon.y = kp->at_0x5d0.y = kp->horizon.y = 0.0f;
	kpStatus->hori_vec			= Vec2_0;
	kpStatus->hori_speed 		= 0.0f;

	kpStatus->acc_vertical.x	= 1.0f;
	kpStatus->acc_vertical.y	= 0.0f;
	kpStatus->dist				= kp->idist;
	kpStatus->dist_vec = kpStatus->dist_speed = 0.0f;

	kp->at_0x5b4				= kpStatus->dist;
	kp->at_0x5a8 = kp->at_0x5b8 = kp->dist_vv1 / kp->at_0x5b4;
	kp->at_0x5ac				= kp->isec_nrm_hori;

	kpStatus->acc.x = kpStatus->acc.z = 0.0f;
	kpStatus->acc.y				= -1.0f;
	kpStatus->acc_value			= 1.0f;
	kpStatus->acc_speed			= 0.0f;

	kp->acc						= kpStatus->acc;

	kp->at_0x5e0				= kp->at_0x5d0;
	kp->ah_circle_ct			= kp_ah_circle_ct;
	kp->at_0x178				= 0;

	kobj = kp->kobjs + ARRAY_LENGTH(kp->kobjs) - 1;
	do
		kobj->flags	= -1;
	while (--kobj >= kp->kobjs);

	kobj = kp->selected + ARRAY_LENGTH(kp->selected) - 1;
	do
		kobj->flags	= -1;
	while (--kobj >= kp->selected);

	kp->at_0x17b				= 0;
	kp->isNewDevType			= true;
}

void KPADGetProjectionPos(f32 param_1, f32 *param_2, f32 const *param_3,
                          f32 const *param_4)
{
	f32 x = param_4[3] - param_4[1];

	param_2[0] = param_3[0] * (x / 2.0f) * 1.2f;
	param_2[1] = param_3[1] * (x / 2.0f) * 1.2f;

	// note the double constant
	param_2[0] *= param_1 * 0.908;
}

s32 KPADCalibrateDPD(KPADChannel chan)
{
	// ERRATUM: uses chan before assertion
	inside_kpads_st *kp = &inside_kpads[chan];
	KPADStatus *status = &kp->status;

	OSAssert_Line(400, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	if (kp->at_0x178 != 2)
		return kp->at_0x178;

	f32 hypot;
	f32 x = kp->acc.x;
	f32 y = kp->acc.y;
	hypot = sqrtf(x * x + y * y);

	if (hypot <= 0.5f)
		return -1;

	kp->iaccXY_nrm_hori.x = x / hypot;
	kp->iaccXY_nrm_hori.y = y / hypot;

	struct kobj *left = kp->kobjs;
	while (left->flags != 0)
		++left;

	struct kobj *right = left + 1;
	while (right->flags != 0)
		++right;

	// hm?
	if (left->pos.x < right->pos.x
	    || (!(right->pos.x < left->pos.x) && !(right->pos.y < left->pos.y)))
	{
		kp->selected[0] = *left;
		kp->selected[1] = *right;
	}
	else
	{
		kp->selected[0] = *right;
		kp->selected[1] = *left;
	}

	calc_dpd2pos_scale(kp);

	x = kp->selected[1].pos.x - kp->selected[0].pos.x;
	y = kp->selected[1].pos.y - kp->selected[0].pos.y;

	// now invhypot
	hypot = 1.0f / sqrtf(x * x + y * y);

	kp->isec_nrm_hori.x = x * hypot;
	kp->isec_nrm_hori.y = y * hypot;
	kp->idist = kp->dist_vv1 * hypot;

	status->dpd_valid_fg = 0;
	return kp->at_0x178;
}

void KPADDisableAimingMode(KPADChannel chan)
{
	OSAssert_Line(480, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	inside_kpads[chan].aimingModeChanged = true;
	inside_kpads[chan].aimingModeEnabled = false;
}

void KPADEnableAimingMode(KPADChannel chan)
{
	OSAssert_Line(469, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	inside_kpads[chan].aimingModeChanged = true;
	inside_kpads[chan].aimingModeEnabled = true;
}

u8 KPADIsEnableAimingMode(KPADChannel chan)
{
	OSAssert_Line(491, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	return inside_kpads[chan].aimingModeEnabled;
}

void KPADSetSensorHeight(KPADChannel chan, f32 sensorHeight)
{
	// ERRATUM: uses chan before assertion
	inside_kpads_st *kp = &inside_kpads[chan];

	OSAssert_Line(502, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	kp->sensorPos.x = 0.0f;
	kp->sensorPos.y = -sensorHeight;

	calc_dpd2pos_scale(kp);
}

f32 KPADGetSensorHeight(KPADChannel chan)
{
	// ERRATUM: uses chan before assertion
	inside_kpads_st *kp = &inside_kpads[chan];

	OSAssert_Line(516, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	return kp->sensorPos.y;
}

static void calc_button_repeat(inside_kpads_st *kp, WPADDeviceType devType,
                               s32 ms)
{
	KPADStatus *kpStatus = &kp->status;
	KPADEXStatus *kpExStatus = &kp->status.ex_status;

	if (kpStatus->trig || kpStatus->release)
	{
		kp->at_0x5ec = 0;
		kp->at_0x5ee = kp->at_0x5f0;

		if (kpStatus->trig && kp->at_0x5f2)
			kpStatus->hold |= 1 << 31; // ?
	}
	else if (kpStatus->hold)
	{
		kp->at_0x5ec += ms;

		if (kp->at_0x5ec >= 40000)
			kp->at_0x5ec -= 40000;

		if (kp->at_0x5ec >= kp->at_0x5ee)
		{
			kpStatus->hold |= 1 << 31;

			kp->at_0x5ee += kp->at_0x5f2;

			if (kp->at_0x5ec >= 20000)
			{
				kp->at_0x5ec -= 20000;
				kp->at_0x5ee -= 20000;
			}
		}
	}

	if (devType == WPAD_DEV_CLASSIC || devType == WPAD_DEV_GUITAR
	    || devType == WPAD_DEV_DRUM || devType == WPAD_DEV_TRAIN
	    || devType == WPAD_DEV_TAIKO || devType == WPAD_DEV_MPLS_PT_CLASSIC)
	{
		if (kpExStatus->cl.trig || kpExStatus->cl.release)
		{
			kp->at_0x5f4 = 0;
			kp->at_0x5f6 = kp->at_0x5f0;

			if (kpExStatus->cl.trig && kp->at_0x5f2)
				kpExStatus->cl.hold |= 1 << 31;
		}
		else if (kpExStatus->cl.hold)
		{
			kp->at_0x5f4 += ms;

			if (kp->at_0x5f4 >= 40000)
				kp->at_0x5f4 -= 40000;

			if (kp->at_0x5f4 >= kp->at_0x5f6)
			{
				kpExStatus->cl.hold |= 1 << 31;

				kp->at_0x5f6 += kp->at_0x5f2;

				if (kp->at_0x5f4 >= 20000)
				{
					kp->at_0x5f4 -= 20000;
					kp->at_0x5f6 -= 20000;
				}
			}
		}
	}
}

static void read_kpad_button(inside_kpads_st *kp, WPADDeviceType devType,
                             s32 param_3, byte4_t coreButton, byte4_t fsButton,
                             byte4_t clButton)
{
	// TODO: name these things
	u32 a, b, c, d, e;

	KPADStatus *kpStatus = &kp->status;
	KPADEXStatus *kpExStatus = &kp->status.ex_status;

	a = kpStatus->hold & 0xffff;
	kpStatus->hold = coreButton & 0x9fff;

	if (devType == WPAD_DEV_FS || devType == WPAD_DEV_MPLS_PT_FS)
	{
		if (kp->isNewDevType)
			c = WPAD_BUTTON_NONE;
		else
			c = fsButton;

		kpStatus->hold |= c & WPAD_BUTTON_FS_ALL;
	}

	b = kpStatus->hold ^ a;
	kpStatus->trig = b & kpStatus->hold;
	kpStatus->release = b & a;

	if (devType == WPAD_DEV_CLASSIC || devType == WPAD_DEV_GUITAR
	    || devType == WPAD_DEV_DRUM || devType == WPAD_DEV_TRAIN
	    || devType == WPAD_DEV_TAIKO || devType == WPAD_DEV_MPLS_PT_CLASSIC)
	{
		if (kp->isNewDevType)
			c = WPAD_BUTTON_NONE;
		else
			c = clButton;

		d = kpExStatus->cl.hold & 0xffff;
		kpExStatus->cl.hold = c & 0xffff;

		e = kpExStatus->cl.hold ^ d;
		kpExStatus->cl.trig = e & kpExStatus->cl.hold;
		kpExStatus->cl.release = e & d;
	}

	calc_button_repeat(kp, devType, param_3);
}

static void calc_acc(inside_kpads_st *kp, f32 *acc, f32 param_3)
{
	f32 a = param_3 - *acc;
	f32 b;

	if (!kp->accPlayMode)
	{
		if (a < 0.0f)
			b = -a;
		else
			b = a;

		if (b >= kp->accParam.x)
		{
			b = 1.0f;
		}
		else
		{
			b /= kp->accParam.x;
			b *= b;
			b *= b;
		}

		b *= kp->accParam.y;
		*acc += b * a;
	}
	else
	{
		if (a < -kp->accParam.x)
			*acc += kp->accParam.y * (a + kp->accParam.x);
		else if (a > kp->accParam.x)
			*acc += kp->accParam.y * (a - kp->accParam.x);
	}
}

static void calc_acc_horizon(inside_kpads_st *kp)
{
	// TODO: name these things
	f32 a, b, c, d, e;

	a = sqrtf(kp->acc.x * kp->acc.x + kp->acc.y * kp->acc.y);

	if (a == 0.0f || a >= 2.0f)
		return;

	b = kp->acc.x / a;
	c = kp->acc.y / a;

	if (a > 1.0f)
		a = 2.0f - a;

	a *= a * kp_acc_horizon_pw;

	d = kp->iaccXY_nrm_hori.x * b + kp->iaccXY_nrm_hori.y * c;
	e = kp->iaccXY_nrm_hori.y * b - kp->iaccXY_nrm_hori.x * c;

	b = kp->at_0x5d0.x + a * (d - kp->at_0x5d0.x);
	c = kp->at_0x5d0.y + a * (e - kp->at_0x5d0.y);

	a = sqrtf(b * b + c * c);

	if (a == 0.0f)
		return;

	kp->at_0x5d0.x = b / a;
	kp->at_0x5d0.y = c / a;

	kp->at_0x5e0.x += (kp->at_0x5d0.x - kp->at_0x5e0.x) * kp_ah_circle_pw;
	kp->at_0x5e0.y += (kp->at_0x5d0.y - kp->at_0x5e0.y) * kp_ah_circle_pw;

	d = kp->at_0x5d0.x - kp->at_0x5e0.x;
	e = kp->at_0x5d0.y - kp->at_0x5e0.y;

	if (d * d + e * e <= kp->ah_circle_r2)
	{
		if (kp->ah_circle_ct)
			--kp->ah_circle_ct;
	}
	else
	{
		kp->ah_circle_ct = kp_ah_circle_ct;
	}
}

static void calc_acc_vertical(inside_kpads_st *kp)
{
	KPADStatus *kpStatus = &kp->status;

	// TODO: What
	// Is this fake
	f32 sqrd;
	f32 x = sqrtf(sqrd = kp->acc.x * kp->acc.x + kp->acc.y * kp->acc.y);

	f32 y = -kp->acc.z;
	f32 invhypot = sqrtf(sqrd + y * y);

	if (invhypot == 0.0f || invhypot >= 2.0f)
		return;

	x /= invhypot;
	y /= invhypot;

	if (invhypot > 1.0f)
		invhypot = 2.0f - invhypot;

	invhypot *= invhypot * kp_acc_horizon_pw;

	x = kpStatus->acc_vertical.x + (x - kpStatus->acc_vertical.x) * invhypot;
	y = kpStatus->acc_vertical.y + (y - kpStatus->acc_vertical.y) * invhypot;

	invhypot = sqrtf(x * x + y * y);
	if (invhypot != 0.0f)
	{
		kpStatus->acc_vertical.x = x / invhypot;
		kpStatus->acc_vertical.y = y / invhypot;
	}
}

static f32 clamp_acc(f32 value, f32 mag)
{
	if (value < 0.0f)
	{
		if (value < -mag)
			return -mag;
	}
	else
	{
		if (value > mag)
			return mag;
	}

	return value;
}

static void read_kpad_acc(inside_kpads_st *kp, KPADUnifiedWpadStatus *uwStatus)
{
	KPADStatus *kpStatus = &kp->status;

	Vec vecA, vecB;

	switch (uwStatus->fmt)
	{
	default:
		return;

	case WPAD_FMT_CORE_BTN_ACC:
	case WPAD_FMT_CORE_BTN_ACC_DPD:
	case WPAD_FMT_FS_BTN_ACC:
	case WPAD_FMT_FS_BTN_ACC_DPD:
	case WPAD_FMT_CLASSIC_BTN_ACC:
	case WPAD_FMT_CLASSIC_BTN_ACC_DPD:
	case WPAD_FMT_GUITAR:
	case WPAD_FMT_DRUM:
	case WPAD_FMT_MOTION_PLUS:
	case WPAD_FMT_TAIKO:
	case WPAD_FMT_BULK:
		// accZ and accY are swapped? is this intended?
		kp->acc.x = clamp_acc(-uwStatus->u.core.accX * kp->coreGravityUnit.x,
		                      kp_rm_acc_max);
		kp->acc.y = clamp_acc(-uwStatus->u.core.accZ * kp->coreGravityUnit.z,
		                      kp_rm_acc_max);
		kp->acc.z = clamp_acc(uwStatus->u.core.accY * kp->coreGravityUnit.y,
		                      kp_rm_acc_max);

		vecB = kpStatus->acc;

		calc_acc(kp, &kpStatus->acc.x, kp->acc.x);
		calc_acc(kp, &kpStatus->acc.y, kp->acc.y);
		calc_acc(kp, &kpStatus->acc.z, kp->acc.z);

		kpStatus->acc_value = sqrtf(kpStatus->acc.x * kpStatus->acc.x
		                            + kpStatus->acc.y * kpStatus->acc.y
		                            + kpStatus->acc.z * kpStatus->acc.z);

		vecB.x -= kpStatus->acc.x;
		vecB.y -= kpStatus->acc.y;
		vecB.z -= kpStatus->acc.z;

		kpStatus->acc_speed =
			sqrtf(vecB.x * vecB.x + vecB.y * vecB.y + vecB.z * vecB.z);

		calc_acc_horizon(kp);
		calc_acc_vertical(kp);

		if ((uwStatus->u.core.err == WPAD_ESUCCESS
		     && uwStatus->u.core.dev == WPAD_DEV_FS
		     && (uwStatus->fmt == WPAD_FMT_FS_BTN_ACC
		         || uwStatus->fmt == WPAD_FMT_FS_BTN_ACC_DPD))
		    || (uwStatus->u.core.dev == WPAD_DEV_MPLS_PT_FS
		        && uwStatus->fmt == WPAD_FMT_MOTION_PLUS
		        && uwStatus->u.mp.stat & WPAD_MPLS_STATUS_EXTENSION_DATA_VALID))
		{
			if (uwStatus->u.core.dev == WPAD_DEV_FS)
			{
				vecA.x = clamp_acc(-uwStatus->u.fs.fsAccX * kp->fsGravityUnit.x,
				                   kp_fs_acc_max);
				vecA.y = clamp_acc(-uwStatus->u.fs.fsAccZ * kp->fsGravityUnit.z,
				                   kp_fs_acc_max);
				vecA.z = clamp_acc(uwStatus->u.fs.fsAccY * kp->fsGravityUnit.y,
				                   kp_fs_acc_max);
			}
			else
			{
				vecA.x = clamp_acc(-uwStatus->u.mp.ext.fs.fsAccX
				                       * kp->fsGravityUnit.x,
				                   kp_fs_acc_max);
				vecA.y = clamp_acc(-uwStatus->u.mp.ext.fs.fsAccZ
				                       * kp->fsGravityUnit.z,
				                   kp_fs_acc_max);
				vecA.z = clamp_acc(uwStatus->u.mp.ext.fs.fsAccY
				                       * kp->fsGravityUnit.y,
				                   kp_fs_acc_max);
			}

			if (kp->reviseEnabled)
				MTXMultVec(kp_fs_rot, &vecA, &vecA);

			vecB = kpStatus->ex_status.fs.acc;

			calc_acc(kp, &kpStatus->ex_status.fs.acc.x, vecA.x);
			calc_acc(kp, &kpStatus->ex_status.fs.acc.y, vecA.y);
			calc_acc(kp, &kpStatus->ex_status.fs.acc.z, vecA.z);

			kpStatus->ex_status.fs.acc_value = sqrtf(
				kpStatus->ex_status.fs.acc.x * kpStatus->ex_status.fs.acc.x
				+ kpStatus->ex_status.fs.acc.y * kpStatus->ex_status.fs.acc.y
				+ kpStatus->ex_status.fs.acc.z * kpStatus->ex_status.fs.acc.z);

			vecB.x -= kpStatus->ex_status.fs.acc.x;
			vecB.y -= kpStatus->ex_status.fs.acc.y;
			vecB.z -= kpStatus->ex_status.fs.acc.z;

			kpStatus->ex_status.fs.acc_speed =
				sqrtf(vecB.x * vecB.x + vecB.y * vecB.y + vecB.z * vecB.z);
		}

		break;
	}
}

static void get_kobj(inside_kpads_st *kp, DPDObject *dpd_obj)
{
	f32 const dpd_scale	= 0x1.8p-9f - 0x1.0p-10f;
	f32 const dpd_cx	= 1.00f - 0x1.0p-10f;
	f32 const dpd_cy	= 0.75f - 0x1.0p-10f;

	struct kobj *kobj = kp->kobjs + ARRAY_LENGTH(kp->kobjs) - 1;
	do
	{
		if (dpd_obj->size)
		{
			kobj->pos.x		= dpd_obj->x * dpd_scale - dpd_cx;
			kobj->pos.y		= dpd_obj->y * dpd_scale - dpd_cy;
			kobj->flags		= 0;
			kobj->inactive	= 0;
		}
		else
		{
			kobj->flags = -1;
		}

		--dpd_obj;
	} while (--kobj >= kp->kobjs);
}

static void check_kobj_outside_frame(inside_kpads_st *kp, struct kobj kobjs[])
{
	struct kobj *kobj;

	kobj = kobjs + 3;
	do
	{
		if (kobj->flags < 0)
			continue;

		if (kobj->pos.x <= kp->frameBorderLeft
		    || kobj->pos.x >= kp->frameBorderRight
		    || kobj->pos.y <= kp->frameBorderTop
		    || kobj->pos.y >= kp->frameBorderBottom)
		{
			kobj->flags |= 1;
		}
	} while (--kobj >= kobjs);
}

static void check_kobj_same_position(struct kobj kobjs[])
{
	struct kobj *k1, *k2;

	k1 = kobjs;
	do
	{
		if (k1->flags != 0)
			continue;

		k2 = k1 + 1;
		do
		{
			if (k2->flags != 0)
				continue;

			if (k1->pos.x == k2->pos.x && k1->pos.y == k2->pos.y)
				k2->flags |= 2;
		} while (++k2 <= kobjs + 3);
	} while (++k1 < kobjs + 3);
}

static f32 calc_horizon(inside_kpads_st *kp, struct kobj *k1, struct kobj *k2,
                        f32 out[2])
{
	f32 x = k2->pos.x - k1->pos.x;
	f32 y = k2->pos.y - k1->pos.y;
	f32 invSqrt = 1.0f / sqrtf(x * x + y * y);

	x *= invSqrt;
	y *= invSqrt;
	out[0] = kp->isec_nrm_hori.x * x + kp->isec_nrm_hori.y * y;
	out[1] = kp->isec_nrm_hori.y * x - kp->isec_nrm_hori.x * y;

	return kp->dist_vv1 * invSqrt;
}

static s8 select_2obj_first(inside_kpads_st *kp)
{
	struct kobj *left, *right, *first, *second;
	f32 a, b;
	f32 x[2];

	a = kp_err_first_inpr;

	left = kp->kobjs;
	do
	{
		if (left->flags != 0)
			continue;

		right = left + 1;
		do
		{
			if (right->flags != 0)
				continue;

			b = calc_horizon(kp, left, right, x);

			if (b <= kp->err_dist_min || b >= kp_err_dist_max)
				continue;

			b = kp->at_0x5d0.x * x[0] + kp->at_0x5d0.y * x[1];
			if (b < 0.0f)
			{
				if (-b > a)
				{
					a = -b;

					first = right;
					second = left;
				}
			}
			else if (b > a)
			{
				a = b;

				first = left;
				second = right;
			}
		} while (++right <= kp->kobjs + ARRAY_LENGTH(kp->kobjs) - 1);
	} while (++left < kp->kobjs + ARRAY_LENGTH(kp->kobjs) - 1);

	if (a == kp_err_first_inpr)
		return 0;

	kp->selected[0] = *first;
	kp->selected[1] = *second;

	return 2;
}

// ew
static s8 select_2obj_continue(inside_kpads_st *kp)
{
	f32 a, b, c, d, e;
	f32 x[2];
	struct kobj *left, *right, *first, *second;
	BOOL isNegative;

	// configurable macro constant?
	e = 2.0f;

	left = kp->kobjs;
	do
	{
		if (left->flags != 0)
			continue;

		right = left + 1;
		do
		{
			if (right->flags != 0)
				continue;

			a = right->pos.x - left->pos.x;
			c = right->pos.y - left->pos.y;
			b = 1.0f / sqrtf(a * a + c * c);

			x[0] = a * b;
			x[1] = c * b;
			b *= kp->dist_vv1;

			if (b <= kp->err_dist_min || b >= kp_err_dist_max)
				continue;

			b -= kp->at_0x5b4;

			if (b < 0.0f)
				b *= kp->err_dist_speed_min;
			else
				b *= kp->err_dist_speed_max;

			if (b >= 1.0f)
				continue;

			d = kp->at_0x5ac.x * x[0] + kp->at_0x5ac.y * x[1];
			if (d < 0.0f)
			{
				d = -d;
				isNegative = true;
			}
			else
			{
				isNegative = false;
			}

			if (d <= kp_err_next_inpr)
				continue;

			d = (1.0f - d) / (1.0f - kp_err_next_inpr);
			b += d;

			if (b < e)
			{
				e = b;

				if (isNegative)
				{
					first = right;
					second = left;
				}
				else
				{
					first = left;
					second = right;
				}
			}

		} while (++right <= kp->kobjs + ARRAY_LENGTH(kp->kobjs) - 1);
	} while (++left < kp->kobjs + ARRAY_LENGTH(kp->kobjs) - 1);

	if (e == 2.0f)
		return 0;

	kp->selected[0] = *first;
	kp->selected[1] = *second;

	return 2;
}

static s8 select_1obj_first(inside_kpads_st *kp)
{
	f32 a = kp->isec_nrm_hori.x * kp->at_0x5d0.x
	      + kp->isec_nrm_hori.y * kp->at_0x5d0.y;

	f32 b = kp->isec_nrm_hori.y * kp->at_0x5d0.x
	      - kp->isec_nrm_hori.x * kp->at_0x5d0.y;

	a *= kp->at_0x5b8;
	b *= kp->at_0x5b8;

	struct kobj *kobj = kp->kobjs;
	Vec2 vecA, vecB;
	do
	{
		if (kobj->flags != 0)
			continue;

		vecA.x = kobj->pos.x - a;
		vecA.y = kobj->pos.y - b;
		vecB.x = kobj->pos.x + a;
		vecB.y = kobj->pos.y + b;

		if (vecA.x <= kp->frameBorderLeft || vecA.x >= kp->frameBorderRight
		    || vecA.y <= kp->frameBorderTop || vecA.y >= kp->frameBorderBottom)
		{
			if (vecB.x > kp->frameBorderLeft && vecB.x < kp->frameBorderRight
			    && vecB.y > kp->frameBorderTop
			    && vecB.y < kp->frameBorderBottom)
			{
				kp->selected[1] = *kobj;

				kp->selected[0].pos = vecA;
				kp->selected[0].flags = 0;
				kp->selected[0].inactive = -1;

				return -1;
			}
		}
		else
		{
			if (vecB.x <= kp->frameBorderLeft || vecB.x >= kp->frameBorderRight
			    || vecB.y <= kp->frameBorderTop
			    || vecB.y >= kp->frameBorderBottom)
			{
				kp->selected[0] = *kobj;

				kp->selected[1].pos = vecB;
				kp->selected[1].flags = 0;
				kp->selected[1].inactive = -1;

				return -1;
			}
		}
	} while (++kobj < kp->kobjs + ARRAY_LENGTH(kp->kobjs));

	return 0;
}

static s8 select_1obj_continue(inside_kpads_st *kp)
{
	f32 x, y, c, d;
	struct kobj *selected, *main, *kdst, *ksrc;

	c = kp_err_near_pos * kp_err_near_pos;

	selected = kp->selected;
	do
	{
		if (selected->flags != 0 || selected->inactive != 0)
			continue;

		main = kp->kobjs;
		do
		{
			if (main->flags != 0)
				continue;

			x = selected->pos.x - main->pos.x;
			y = selected->pos.y - main->pos.y;

			d = x * x + y * y;

			if (d < c)
			{
				c = d;
				kdst = selected;
				ksrc = main;
			}

		} while (++main < kp->kobjs + ARRAY_LENGTH(kp->kobjs));
	} while (++selected < kp->selected + ARRAY_LENGTH(kp->selected));

	if (c == kp_err_near_pos * kp_err_near_pos)
		return 0;

	*kdst = *ksrc;

	kp->at_0x5ac.x = kp->isec_nrm_hori.x * kp->at_0x5d0.x
	               + kp->isec_nrm_hori.y * kp->at_0x5d0.y;

	kp->at_0x5ac.y = kp->isec_nrm_hori.y * kp->at_0x5d0.x
	               - kp->isec_nrm_hori.x * kp->at_0x5d0.y;

	x = kp->at_0x5a8 * kp->at_0x5ac.x;
	y = kp->at_0x5a8 * kp->at_0x5ac.y;

	if (kdst == &kp->selected[0])
	{
		kp->selected[1].pos.x = kdst->pos.x + x;
		kp->selected[1].pos.y = kdst->pos.y + y;
		kp->selected[1].flags = 0;
		kp->selected[1].inactive = -1;
	}
	else // if (kdst == &kp->selected[1])
	{
		kp->selected[0].pos.x = kdst->pos.x - x;
		kp->selected[0].pos.y = kdst->pos.y - y;
		kp->selected[0].flags = 0;
		kp->selected[0].inactive = -1;
	}

	if (kp->status.dpd_valid_fg < 0)
		return -1;
	else
		return 1;
}

static void calc_obj_horizon(inside_kpads_st *kp)
{
	f32 x = kp->selected[1].pos.x - kp->selected[0].pos.x;
	f32 y = kp->selected[1].pos.y - kp->selected[0].pos.y;

	kp->at_0x5a8 = sqrtf(x * x + y * y);
	f32 invSqrt = 1.0f / kp->at_0x5a8;

	kp->at_0x5b4 = kp->dist_vv1 * invSqrt;
	x *= invSqrt;
	kp->at_0x5ac.x = x;
	y *= invSqrt;
	kp->at_0x5ac.y = y;

	kp->horizon.x = kp->isec_nrm_hori.x * x + kp->isec_nrm_hori.y * y;
	kp->horizon.y = kp->isec_nrm_hori.y * x - kp->isec_nrm_hori.x * y;
}

static void calc_dpd_variable(inside_kpads_st *kp, s8 param_2)
{
	KPADStatus *kpStatus = &kp->status;
	Vec2 vecA, vecB;
	f32 a, b, dist;

	if (!param_2)
	{
		kpStatus->dpd_valid_fg = 0;
		return;
	}

	vecA.x = kp->isec_nrm_hori.x * kp->at_0x5ac.x
	       + kp->isec_nrm_hori.y * kp->at_0x5ac.y;

	vecA.y = kp->isec_nrm_hori.y * kp->at_0x5ac.x
	       - kp->isec_nrm_hori.x * kp->at_0x5ac.y;

	if (!kpStatus->dpd_valid_fg)
	{
		kpStatus->horizon = vecA;
		kpStatus->hori_vec = Vec2_0;
		kpStatus->hori_speed = 0.0f;
	}
	else
	{
		vecB.x = vecA.x - kpStatus->horizon.x;
		vecB.y = vecA.y - kpStatus->horizon.y;

		a = sqrtf(vecB.x * vecB.x + vecB.y * vecB.y);

		if (!kp->horiPlayMode)
		{
			if (a >= kp->horiParam.x)
			{
				a = 1.0f;
			}
			else
			{
				a /= kp->horiParam.x;
				a *= a;
				a *= a;
			}

			a *= kp->horiParam.y;
			vecB.x = kpStatus->horizon.x + a * vecB.x;
			vecB.y = kpStatus->horizon.y + a * vecB.y;

			a = sqrtf(vecB.x * vecB.x + vecB.y * vecB.y);
			vecB.x /= a;
			vecB.y /= a;

			kpStatus->hori_vec.x = vecB.x - kpStatus->horizon.x;
			kpStatus->hori_vec.y = vecB.y - kpStatus->horizon.y;

			kpStatus->hori_speed =
				sqrtf(kpStatus->hori_vec.x * kpStatus->hori_vec.x
			          + kpStatus->hori_vec.y * kpStatus->hori_vec.y);

			kpStatus->horizon = vecB;
		}
		else if (a > kp->horiParam.x)
		{
			a = (a - kp->horiParam.x) / a * kp->horiParam.y;
			vecB.x = vecB.x * a + kpStatus->horizon.x;
			vecB.y = vecB.y * a + kpStatus->horizon.y;

			a = sqrtf(vecB.x * vecB.x + vecB.y * vecB.y);
			vecB.x /= a;
			vecB.y /= a;

			kpStatus->hori_vec.x = vecB.x - kpStatus->horizon.x;
			kpStatus->hori_vec.y = vecB.y - kpStatus->horizon.y;

			kpStatus->hori_speed =
				sqrtf(kpStatus->hori_vec.x * kpStatus->hori_vec.x
			          + kpStatus->hori_vec.y * kpStatus->hori_vec.y);

			kpStatus->horizon = vecB;
		}
		else
		{
			kpStatus->hori_vec = Vec2_0;
			kpStatus->hori_speed = 0.0f;
		}
	}

	dist = kp->dist_vv1 / kp->at_0x5a8;

	if (!kpStatus->dpd_valid_fg)
	{
		kpStatus->dist = dist;
		kpStatus->dist_vec = 0.0f;
		kpStatus->dist_speed = 0.0f;
	}
	else
	{
		b = dist - kpStatus->dist;

		if (b < 0.0f)
			a = -b;
		else
			a = b;

		if (!kp->distPlayMode)
		{
			if (a >= kp->distParam.x)
			{
				a = 1.0f;
			}
			else
			{
				a /= kp->distParam.x;
				a *= a;
				a *= a;
			}

			a *= kp->distParam.y;

			kpStatus->dist_vec = a * b;
			if (kpStatus->dist_vec < 0.0f)
				kpStatus->dist_speed = -kpStatus->dist_vec;
			else
				kpStatus->dist_speed = kpStatus->dist_vec;

			kpStatus->dist += kpStatus->dist_vec;
		}
		else if (a > kp->distParam.x)
		{
			a = (a - kp->distParam.x) / a * kp->distParam.y;
			kpStatus->dist_vec = a * b;

			if (kpStatus->dist_vec < 0.0f)
				kpStatus->dist_speed = -kpStatus->dist_vec;
			else
				kpStatus->dist_speed = kpStatus->dist_vec;

			kpStatus->dist += kpStatus->dist_vec;
		}
		else
		{
			kpStatus->dist_vec = 0.0f;
			kpStatus->dist_speed = 0.0f;
		}
	}

	vecA.x = (kp->selected[0].pos.x + kp->selected[1].pos.x) * 0.5f;
	vecA.y = (kp->selected[0].pos.y + kp->selected[1].pos.y) * 0.5f;

	a = kp->at_0x5ac.x * kp->isec_nrm_hori.x
	  + kp->at_0x5ac.y * kp->isec_nrm_hori.y;
	b = -kp->at_0x5ac.y * kp->isec_nrm_hori.x
	  + kp->at_0x5ac.x * kp->isec_nrm_hori.y;

	vecB.x = a * vecA.x - b * vecA.y;
	vecB.y = b * vecA.x + a * vecA.y;

	vecB.x = (kp->sensorPos.x - vecB.x) * kp->dpd2pos_scale;
	vecB.y = (kp->sensorPos.y - vecB.y) * kp->dpd2pos_scale;

	vecA.x = -kp->iaccXY_nrm_hori.y * vecB.x + kp->iaccXY_nrm_hori.x * vecB.y;
	vecA.y = -kp->iaccXY_nrm_hori.x * vecB.x - kp->iaccXY_nrm_hori.y * vecB.y;

	kp->pos = vecA;

	if (!kpStatus->dpd_valid_fg)
	{
		kpStatus->pos = vecA;
		kpStatus->vec = Vec2_0;
		kpStatus->speed = 0.0f;
	}
	else
	{
		vecB.x = vecA.x - kpStatus->pos.x;
		vecB.y = vecA.y - kpStatus->pos.y;

		a = sqrtf(vecB.x * vecB.x + vecB.y * vecB.y);

		if (!kp->posPlayMode)
		{
			if (a >= kp->posParam.x)
			{
				a = 1.0f;
			}
			else
			{
				a /= kp->posParam.x;
				a *= a;
				a *= a;
			}

			a *= kp->posParam.y;
			kpStatus->vec.x = a * vecB.x;
			kpStatus->vec.y = a * vecB.y;

			kpStatus->speed = sqrtf(kpStatus->vec.x * kpStatus->vec.x
			                        + kpStatus->vec.y * kpStatus->vec.y);

			kpStatus->pos.x += kpStatus->vec.x;
			kpStatus->pos.y += kpStatus->vec.y;
		}
		else if (a > kp->posParam.x)
		{
			a = (a - kp->posParam.x) / a * kp->posParam.y;
			kpStatus->vec.x = a * vecB.x;
			kpStatus->vec.y = a * vecB.y;

			kpStatus->speed = sqrtf(kpStatus->vec.x * kpStatus->vec.x
			                        + kpStatus->vec.y * kpStatus->vec.y);

			kpStatus->pos.x += kpStatus->vec.x;
			kpStatus->pos.y += kpStatus->vec.y;
		}
		else
		{
			kpStatus->vec = Vec2_0;
			kpStatus->speed = 0.0f;
		}
	}

	kpStatus->dpd_valid_fg = param_2;
}

static void read_kpad_dpd(inside_kpads_st *kp, KPADUnifiedWpadStatus *uwStatus)
{
	KPADStatus *kpStatus = &kp->status;
	struct kobj *k1;
	s8 a;

	if ((uwStatus->fmt == WPAD_FMT_CORE_BTN_ACC_DPD)
	    || (uwStatus->fmt == WPAD_FMT_FS_BTN_ACC_DPD)
	    || (uwStatus->fmt == WPAD_FMT_CLASSIC_BTN_ACC_DPD)
	    || (uwStatus->fmt == WPAD_FMT_GUITAR && kp->wpadDpdEnabled)
	    || (uwStatus->fmt == WPAD_FMT_DRUM && kp->wpadDpdEnabled)
	    || (uwStatus->fmt == WPAD_FMT_TAIKO && kp->wpadDpdEnabled)
	    || (uwStatus->fmt == WPAD_FMT_MOTION_PLUS && kp->wpadDpdEnabled))
	{
		get_kobj(kp,
		         uwStatus->u.core.obj + ARRAY_LENGTH(uwStatus->u.core.obj) - 1);
	}
	else
	{
		k1 = kp->kobjs + ARRAY_LENGTH(kp->kobjs) - 1;
		do
			k1->flags = -1;
		while (--k1 >= kp->kobjs);
	}

	check_kobj_outside_frame(kp, kp->kobjs);
	check_kobj_same_position(kp->kobjs);
	kp->at_0x178 = 0;

	k1 = kp->kobjs + ARRAY_LENGTH(kp->kobjs) - 1;
	do
	{
		if (k1->flags == 0)
			++kp->at_0x178;
	} while (--k1 >= kp->kobjs);

	if (!(kpStatus->acc_vertical.x <= kp_err_up_inpr))
	{
		if (kpStatus->dpd_valid_fg == 2 || kpStatus->dpd_valid_fg == -2)
		{
			if (kp->at_0x178 >= 2)
			{
				a = select_2obj_continue(kp);

				if (a)
					goto skip_set_var_a;
			}

			if (kp->at_0x178 >= 1)
			{
				a = select_1obj_continue(kp);

				if (a)
					goto skip_set_var_a;
			}
		}
		else if (kpStatus->dpd_valid_fg == 1 || kpStatus->dpd_valid_fg == -1)
		{
			if (kp->at_0x178 >= 2)
			{
				a = select_2obj_first(kp);

				if (a)
					goto skip_set_var_a;
			}

			if (kp->at_0x178 >= 1)
			{
				a = select_1obj_continue(kp);

				if (a)
					goto skip_set_var_a;
			}
		}
		else
		{
			if (kp->at_0x178 >= 2)
			{
				a = select_2obj_first(kp);

				if (a)
					goto skip_set_var_a;
			}

			if (kp->at_0x178 == 1)
			{
				a = select_1obj_first(kp);

				if (a)
					goto skip_set_var_a;
			}
		}
	}

	// What
	a = 0;

skip_set_var_a:
	if (a)
	{
		calc_obj_horizon(kp);

		if (!kp->ah_circle_ct)
		{
			if (kp->horizon.x * kp->at_0x5d0.x + kp->horizon.y * kp->at_0x5d0.y
			    <= kp_err_acc_inpr)
			{
				a = 0;
				kp->selected[0].flags = kp->selected[1].flags = 1;
			}
		}

		if (kpStatus->dpd_valid_fg == 2 && a == 2)
		{
			if (kp->at_0x5ea == 200)
				kp->at_0x5b8 = kp->at_0x5a8;
			else
				++kp->at_0x5ea;
		}
		else
		{
			kp->at_0x5ea = 0;
		}
	}
	else
	{
		kp->at_0x5ea = 0;
	}

	calc_dpd_variable(kp, a);
}

static void clamp_trigger(f32 *out, s32 val, s32 min, s32 max)
{
	if (val <= min)
		*out = 0.0f;
	else if (val >= max)
		*out = 1.0f;
	else
		*out = (f32)(val - min) / (f32)(max - min);
}

static void clamp_stick_circle(Vec2 *stick, s32 x_, s32 y_, s32 min_,
                               s32 max_)
{
	f32 x = (f32)x_;
	f32 y = (f32)y_;
	f32 min = (f32)min_;
	f32 max = (f32)max_;

	f32 sqrt = sqrtf(x * x + y * y);

	if (sqrt <= min)
	{
		stick->x = stick->y = 0.0f;
	}
	else if (sqrt >= max)
	{
		stick->x = x / sqrt;
		stick->y = y / sqrt;
	}
	else
	{
		sqrt = (sqrt - min) / (max - min) / sqrt;
		stick->x = x * sqrt;
		stick->y = y * sqrt;
	}
}

static void clamp_stick_cross(Vec2 *stick, s32 x, s32 y, s32 min, s32 max)
{
	if (x < 0)
	{
		clamp_trigger(&stick->x, -x, min, max);
		stick->x = -stick->x;
	}
	else
	{
		clamp_trigger(&stick->x, x, min, max);
	}

	if (y < 0)
	{
		clamp_trigger(&stick->y, -y, min, max);
		stick->y = -stick->y;
	}
	else
	{
		clamp_trigger(&stick->y, y, min, max);
	}

	f32 a = stick->x * stick->x + stick->y * stick->y;
	if (a > 1.0f)
	{
		a = sqrtf(a);
		stick->x /= a;
		stick->y /= a;
	}
}

static void read_kpad_ext(inside_kpads_st *kp, KPADUnifiedWpadStatus *uwStatus)
{
	KPADStatus *kpStatus ATTR_UNUSED;
	KPADEXStatus *kpExStatus;
	ClampFunc *clampFunc;
	f64 wbc_weight;

	kpExStatus = &kp->status.ex_status;
	kpStatus = &kp->status;
	clampFunc = kp_stick_clamp_cross ? &clamp_stick_cross : &clamp_stick_circle;

	if (uwStatus->u.core.dev == WPAD_DEV_FS
	    && (uwStatus->fmt == WPAD_FMT_FS_BTN
	        || uwStatus->fmt == WPAD_FMT_FS_BTN_ACC
	        || uwStatus->fmt == WPAD_FMT_FS_BTN_ACC_DPD))
	{
		if (kp->isNewDevType)
		{
			kp->isNewDevType = false;

			kpExStatus->fs.stick = Vec2_0;
			kpExStatus->fs.acc.x = kpExStatus->fs.acc.z = 0.0f;
			kpExStatus->fs.acc.y = -1.0f;
			kpExStatus->fs.acc_value = 1.0f;
			kpExStatus->fs.acc_speed = 0.0f;
		}

		(*clampFunc)(&kpExStatus->fs.stick, uwStatus->u.fs.fsStickX,
		             uwStatus->u.fs.fsStickY, kp_fs_fstick_min,
		             kp_fs_fstick_max);
	}
	else if (uwStatus->u.core.dev == WPAD_DEV_CLASSIC
	         && (uwStatus->fmt == WPAD_FMT_CLASSIC_BTN
	             || uwStatus->fmt == WPAD_FMT_CLASSIC_BTN_ACC
	             || uwStatus->fmt == WPAD_FMT_CLASSIC_BTN_ACC_DPD))
	{
		if (kp->isNewDevType)
		{
			kp->isNewDevType = false;

			kpExStatus->cl.lstick = Vec2_0;
			kpExStatus->cl.rstick = Vec2_0;
			kpExStatus->cl.ltrigger = kpExStatus->cl.rtrigger = 0.0f;
			kpExStatus->cl.hold = kpExStatus->cl.trig = kpExStatus->cl.release =
				0;

			kp->at_0x5f4 = 0;
			kp->at_0x5f6 = kp->at_0x5f0;
		}

		(*clampFunc)(&kpExStatus->cl.lstick, uwStatus->u.cl.clLStickX,
		             uwStatus->u.cl.clLStickY, kp_cl_stick_min,
		             kp_cl_stick_max);

		(*clampFunc)(&kpExStatus->cl.rstick, uwStatus->u.cl.clRStickX,
		             uwStatus->u.cl.clRStickY, kp_cl_stick_min,
		             kp_cl_stick_max);

		clamp_trigger(&kpExStatus->cl.ltrigger, uwStatus->u.cl.clTriggerL,
		              kp_cl_trigger_min, kp_cl_trigger_max);

		clamp_trigger(&kpExStatus->cl.rtrigger, uwStatus->u.cl.clTriggerR,
		              kp_cl_trigger_min, kp_cl_trigger_max);
	}
	else if ((uwStatus->u.core.dev == WPAD_DEV_GUITAR
	          && uwStatus->fmt == WPAD_FMT_GUITAR)
	         || (uwStatus->u.core.dev == WPAD_DEV_DRUM
	             && uwStatus->fmt == WPAD_FMT_DRUM))
	{
		if (kp->isNewDevType)
		{
			kp->isNewDevType = false;

			kpExStatus->cl.lstick = Vec2_0;
			kpExStatus->cl.rstick = Vec2_0;
			kpExStatus->cl.ltrigger = kpExStatus->cl.rtrigger = 0.0f;
			kpExStatus->cl.hold = kpExStatus->cl.trig = kpExStatus->cl.release =
				0;

			kp->at_0x5f4 = 0;
			kp->at_0x5f6 = kp->at_0x5f0;
		}

		(*clampFunc)(&kpExStatus->cl.lstick, uwStatus->u.cl.clLStickX,
		             uwStatus->u.cl.clLStickY, kp_cl_stick_min,
		             kp_cl_stick_max);

		clamp_trigger(&kpExStatus->cl.rstick.x, uwStatus->u.mp.ext.cl.clRStickX,
		              kp_ex_analog_min, kp_ex_analog_max);

		clamp_trigger(&kpExStatus->cl.rstick.y, uwStatus->u.mp.ext.cl.clRStickY,
		              kp_ex_analog_min, kp_ex_analog_max);

		clamp_trigger(&kpExStatus->cl.ltrigger,
		              uwStatus->u.mp.ext.cl.clTriggerL, kp_ex_trigger_min,
		              kp_ex_trigger_max);

		clamp_trigger(&kpExStatus->cl.rtrigger,
		              uwStatus->u.mp.ext.cl.clTriggerR, kp_ex_trigger_min,
		              kp_ex_trigger_max);
	}
	else if (uwStatus->u.core.dev == WPAD_DEV_TRAIN
	         && uwStatus->fmt == WPAD_FMT_TRAIN)
	{
		if (kp->isNewDevType)
		{
			kp->isNewDevType = false;

			kpExStatus->cl.lstick = Vec2_0;
			kpExStatus->cl.rstick = Vec2_0;

			kpExStatus->cl.ltrigger = kpExStatus->cl.rtrigger = 0.0f;
			kpExStatus->cl.hold = kpExStatus->cl.trig = kpExStatus->cl.release =
				0;

			kp->at_0x5f4 = 0;
			kp->at_0x5f6 = kp->at_0x5f0;
		}

		kpExStatus->cl.lstick.x = kpExStatus->cl.lstick.y = 0.0f;
		kpExStatus->cl.rstick.x = kpExStatus->cl.rstick.y = 0.0f;

		clamp_trigger(&kpExStatus->cl.ltrigger, uwStatus->u.tr.brake,
		              kp_ex_trigger_min, kp_ex_trigger_max);

		clamp_trigger(&kpExStatus->cl.rtrigger, uwStatus->u.tr.mascon,
		              kp_ex_trigger_min, kp_ex_trigger_max);
	}
	else if (uwStatus->u.core.dev == WPAD_DEV_TAIKO
	         && uwStatus->fmt == WPAD_FMT_TAIKO)
	{
		if (kp->isNewDevType)
		{
			kp->isNewDevType = false;

			kpExStatus->cl.lstick = Vec2_0;
			kpExStatus->cl.rstick = Vec2_0;
			kpExStatus->cl.ltrigger = kpExStatus->cl.rtrigger = 0.0f;
			kpExStatus->cl.hold = kpExStatus->cl.trig = kpExStatus->cl.release =
				0;

			kp->at_0x5f4 = 0;
			kp->at_0x5f6 = kp->at_0x5f0;
		}

		clamp_trigger(&kpExStatus->cl.lstick.x, uwStatus->u.mp.ext.cl.clLStickX,
		              kp_ex_analog_min, kp_ex_analog_max);

		clamp_trigger(&kpExStatus->cl.lstick.y, uwStatus->u.mp.ext.cl.clLStickY,
		              kp_ex_analog_min, kp_ex_analog_max);

		clamp_trigger(&kpExStatus->cl.rstick.x, uwStatus->u.mp.ext.cl.clRStickX,
		              kp_ex_analog_min, kp_ex_analog_max);

		clamp_trigger(&kpExStatus->cl.rstick.y, uwStatus->u.mp.ext.cl.clRStickY,
		              kp_ex_analog_min, kp_ex_analog_max);

		clamp_trigger(&kpExStatus->cl.ltrigger,
		              uwStatus->u.mp.ext.cl.clTriggerL, kp_ex_trigger_min,
		              kp_ex_trigger_max);

		clamp_trigger(&kpExStatus->cl.rtrigger,
		              uwStatus->u.mp.ext.cl.clTriggerR, kp_ex_trigger_min,
		              kp_ex_trigger_max);
	}
	else if (uwStatus->u.core.dev == WPAD_DEV_BALANCE_CHECKER
	         && uwStatus->fmt == WPAD_FMT_BALANCE_CHECKER)
	{
		kpExStatus->bl.weight_err =
			WBCRead(&uwStatus->u.bl, kpExStatus->bl.weight, 4);

		if (!WBCGetBatteryLevel(uwStatus->u.bl.battery))
		{
			kpExStatus->bl.weight_err = -1;
		}
		else if (kp_wbc_enabled < WBCEnableNotStarted) // WBCEnableFailed
		{
			kpExStatus->bl.weight_err = -6;
		}
		else if (!kp_wbc_zero_point_done)
		{
			kpExStatus->bl.weight_err = -7;
		}
		else if (kp_wbc_zero_point_done <= 3)
		{
			if (kpExStatus->bl.weight_err == 1)
			{
				kp_wbc_zero_point_done = -1;
				kp_wbc_tgc_weight_done = 0;
			}

			if (kp_wbc_zero_point_done < 0)
				kpExStatus->bl.weight_err = -4;
			else
				kpExStatus->bl.weight_err = -8;
		}
		else if (kpExStatus->bl.weight_err == -2)
		{
			// ? is this two different enums?
			kpExStatus->bl.weight_err = -2;
		}
		else if (kpExStatus->bl.weight_err >= 0)
		{
			int i;

			wbc_weight = kpExStatus->bl.weight[0] + kpExStatus->bl.weight[1]
			      + kpExStatus->bl.weight[2] + kpExStatus->bl.weight[3];

			for (i = 0; i < (int)ARRAY_LENGTH(kp_wbc_weight_ave); ++i)
			{
				kp_wbc_weight_ave[i] = (kp_wbc_ave_count * kp_wbc_weight_ave[i]
				                        + kpExStatus->bl.weight[i])
				                     / (1.0f + kp_wbc_ave_count);

				kpExStatus->bl.weight_ave[i] = kp_wbc_weight_ave[i];
			}

			// Note the change from float to double constants
			if (wbc_weight > 150.0)
			{
				kpExStatus->bl.weight_err = -5;
			}
			else if (kp_wbc_tgc_weight_done == 1 || kp_wbc_tgc_weight_done == 2)
			{
				kp_wbc_tgc_weight_err = 1;

				if (++kp_wbc_tgc_weight_wait > 2000)
				{
					if (kp_wbc_tgc_weight_done == 1)
						kp_wbc_tgc_weight_err = -3;
					else
						kp_wbc_tgc_weight_err = -1;

					kp_wbc_tgc_weight_done = 0;
				}
				else if (kp_wbc_tgc_weight_done == 1)
				{
					if (wbc_weight >= 2.0)
					{
						kp_wbc_tgc_weight_done = 2;
						kp_wbc_ave_sample_count = 0;
						kp_wbc_max_weight = wbc_weight;
						kp_wbc_min_weight = wbc_weight;
					}
				}
				else if (wbc_weight < 2.0)
				{
					kp_wbc_tgc_weight_done = 1;
				}
				else
				{
					kp_wbc_max_weight = wbc_weight - kp_wbc_min_weight;

					if (kp_wbc_max_weight < 0.0)
						kp_wbc_max_weight *= -1.0;

					if (kp_wbc_max_weight < 0.3)
					{
						++kp_wbc_ave_sample_count;
					}
					else
					{
						kp_wbc_min_weight += (wbc_weight - kp_wbc_min_weight) * 0.5;
						kp_wbc_ave_sample_count = 0;
					}

					if (kp_wbc_ave_sample_count > 100)
					{
						kp_wbc_tgc_weight_done = 3;
						kp_wbc_ave_sample_count = 0;
						kp_wbc_max_weight = wbc_weight;
						kp_wbc_min_weight = wbc_weight;
					}
				}
			}
			else if (kp_wbc_tgc_weight_done == 3)
			{
				kp_wbc_tgc_weight_err = -4;
				++kp_wbc_ave_sample_count;

				for (i = 0; i < (int)ARRAY_LENGTH(kp_wbc_ave_sample); ++i)
				{
					kp_wbc_ave_sample[i] =
						((kp_wbc_ave_sample_count - 1) * kp_wbc_ave_sample[i]
					     + kpExStatus->bl.weight[i])
						/ kp_wbc_ave_sample_count;
				}

				if (kp_wbc_min_weight > wbc_weight)
					kp_wbc_min_weight = wbc_weight;

				if (kp_wbc_max_weight < wbc_weight)
					kp_wbc_max_weight = wbc_weight;

				if (kp_wbc_ave_sample_count == kp_wbc_ave_count)
				{
					kp_wbc_tgc_weight_err = 0;
					wbc_weight = kp_wbc_ave_sample[0] + kp_wbc_ave_sample[1]
					      + kp_wbc_ave_sample[2] + kp_wbc_ave_sample[3];

					WBCGetTGCWeight(wbc_weight, &kp_wbc_tgc_weight,
					                &uwStatus->u.bl);

					if (wbc_weight - kp_wbc_min_weight > 0.3
					    || kp_wbc_max_weight - wbc_weight > 0.3)
					{
						kp_wbc_tgc_weight_err = -1;
					}

					kp_wbc_tgc_weight_done = 0;
				}
			}
		}

		kpExStatus->bl.tgc_weight = kp_wbc_tgc_weight;
		kpExStatus->bl.tgc_weight_err = kp_wbc_tgc_weight_err;
	}
	else if ((uwStatus->u.core.dev == WPAD_DEV_MOTION_PLUS
	          || uwStatus->u.core.dev == WPAD_DEV_MPLS_PT_FS
	          || uwStatus->u.core.dev == WPAD_DEV_MPLS_PT_CLASSIC
	          || uwStatus->u.core.dev == WPAD_DEV_MPLS_PT_UNKNOWN)
	         && uwStatus->fmt == WPAD_FMT_MOTION_PLUS)
	{
		if (kp->isNewDevType)
		{
			kp->isNewDevType = false;

			if (uwStatus->u.core.dev == WPAD_DEV_MPLS_PT_FS
			    && uwStatus->u.mp.stat & WPAD_MPLS_STATUS_EXTENSION_DATA_VALID)
			{
				kpExStatus->fs.stick = Vec2_0;
				kpExStatus->fs.acc.x = kpExStatus->fs.acc.z = 0.0f;
				kpExStatus->fs.acc.y = -1.0f;
				kpExStatus->fs.acc_value = 1.0f;
				kpExStatus->fs.acc_speed = 0.0f;
			}

			if (uwStatus->u.core.dev == WPAD_DEV_MPLS_PT_CLASSIC
			    && uwStatus->u.mp.stat & WPAD_MPLS_STATUS_EXTENSION_DATA_VALID)
			{
				kpExStatus->cl.lstick = Vec2_0;
				kpExStatus->cl.rstick = Vec2_0;
				kpExStatus->cl.ltrigger = kpExStatus->cl.rtrigger = 0.0f;
				kpExStatus->cl.hold = kpExStatus->cl.trig =
					kpExStatus->cl.release = 0;

				kp->at_0x5f4 = 0;
				kp->at_0x5f6 = kp->at_0x5f0;
			}
		}

		if (uwStatus->u.mp.stat & WPAD_MPLS_STATUS_EXTENSION_CONNECTED
		    && uwStatus->u.mp.stat & WPAD_MPLS_STATUS_EXTENSION_DATA_VALID)
		{
			if (uwStatus->u.core.dev == WPAD_DEV_MPLS_PT_FS)
			{
				(*clampFunc)(&kpExStatus->fs.stick, uwStatus->u.fs.fsStickX,
				             uwStatus->u.fs.fsStickY, kp_fs_fstick_min,
				             kp_fs_fstick_max);
			}
			else if (uwStatus->u.core.dev == WPAD_DEV_MPLS_PT_CLASSIC)
			{
				(*clampFunc)(&kpExStatus->cl.lstick, uwStatus->u.cl.clLStickX,
				             uwStatus->u.cl.clLStickY, kp_cl_stick_min,
				             kp_cl_stick_max);

				(*clampFunc)(&kpExStatus->cl.rstick, uwStatus->u.cl.clRStickX,
				             uwStatus->u.cl.clRStickY, kp_cl_stick_min,
				             kp_cl_stick_max);

				clamp_trigger(&kpExStatus->cl.ltrigger,
				              uwStatus->u.cl.clTriggerL, kp_cl_trigger_min,
				              kp_cl_trigger_max);

				clamp_trigger(&kpExStatus->cl.rtrigger,
				              uwStatus->u.cl.clTriggerR, kp_cl_trigger_min,
				              kp_cl_trigger_max);
			}
		}
	}
}

static void read_kpad_mpls(KPADChannel chan, s32 offset, s32 count, f64 param_4)
{
	inside_kpads_st *kp = &inside_kpads[chan];
	KPADStatus *kpStatus = &kp->status;

	KPADStatus localStatus;
	localStatus.acc				= kp->acc;
	localStatus.pos				= kp->pos;
	localStatus.horizon			= kp->horizon;
	localStatus.dpd_valid_fg	= kp->status.dpd_valid_fg;

	KMPLSSetKpadRingBuffer(chan, kp->localSamplingBuf, kp->appSamplingBuf,
	                       kp->appSamplingBufIndex);
	KMPLSSetSamplingCallback(&KPADiMplsSamplingCallback);
	KMPLSRead(chan, &kpStatus->mpls, offset, count, &localStatus, param_4);
}

static void KPADiMplsSamplingCallback(KPADChannel chan, KPADMPStatus *mpStatus)
{
	inside_kpads_st *kp = &inside_kpads[chan];

	OSAssert_Line(1989, kp->appSamplingBufPtr != NULL);

	if (kp->appSamplingBufPtr != NULL)
	{
		--kp->appSamplingBufPtr;
		kp->appSamplingBufPtr->mpls = *mpStatus;
	}
}

s32 KPADRead(KPADChannel chan, KPADStatus *status, s32 count)
{
	return KPADiRead(chan, status, count, nullptr, false);
}

s32 KPADReadEx(KPADChannel chan, KPADStatus *status, s32 count,
               KPADResult *result)
{
	return KPADiRead(chan, status, count, result, true);
}

static s32 KPADiRead(KPADChannel chan, KPADStatus *status, s32 count,
                     KPADResult *result, BOOL exRead)
{
	inside_kpads_st *kp;

	KPADStatus localStatus;
	OSTime stack_0x50;
	OSTime stack_0x48;
	KPADStatus *stack_0x40;
	WPADResult wpadRet;
	KPADResult ret;
	s32 offset;
	s32 stack_0x30;
	s32 stack_0x2c;
	u32 stack_0x28;
	BOOL intrStatus;
	u32 coreButton;
	u32 fsButton;
	u32 clButton;
	WPADDeviceType devType;
	f64 f31;
	KPADUnifiedWpadStatus *uwStatus;

	stack_0x40 = status;
	kp = &inside_kpads[chan];

	ret = KPAD_ESUCCESS;
	stack_0x2c = 0;

	OSAssert_Line(2033, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	if (!kp_initialized)
	{
		ret = -5;
		goto end;
	}

	if (WPADGetStatus() != WPAD_LIB_STATUS_3)
	{
		ret = -3;
		goto end;
	}

	intrStatus = OSDisableInterrupts();

	if (kp->currentlyReading)
	{
		OSRestoreInterrupts(intrStatus);
		ret = -4;
		goto end;
	}

	kp->currentlyReading = true;

	wpadRet = WPADProbe(chan, nullptr);
	if (wpadRet == WPAD_ENODEV)
	{
		reset_kpad(chan);

		if (kp->controlDpdCb && kp->at_0x646 && !kp->at_0x647)
			KPADiControlDpdCallback(chan, WPAD_ESUCCESS);

		if (kp->controlMplsCb && kp->at_0x67b && !kp->at_0x67c)
			KPADiControlMplsCallback(chan, WPAD_ESUCCESS);

		kp->currentlyReading = false;

		OSRestoreInterrupts(intrStatus);
		ret = -2;
		goto end;
	}

	OSRestoreInterrupts(intrStatus);

	if (kp->needsReset)
	{
		kp->status.wpad_err = WPAD_EINVAL;
		reset_kpad(chan);
	}

	localStatus = *status;

	if (kp->at_0x17b != 0 && status && count != 0)
	{
		intrStatus = OSDisableInterrupts();

		stack_0x28 = kp->at_0x17b;
		if (stack_0x28 > count)
			stack_0x28 = count;

		stack_0x30 = stack_0x28;
		kp->at_0x17b = 0;

		stack_0x50 = kp->tickQueue[0];
		kp->tickQueue[0] = kp->tickQueue[1];
		kp->tickQueue[1] = kp->tickQueue[2];
		kp->tickQueue[2] = kp->tickQueue[3];

		stack_0x48 = kp->tickQueue[3];

		// ?
		f31 = (f64)((stack_0x48 + 0x100000000ll - stack_0x50) & 0xffffffff)
		      / OS_TIMER_CLOCK;

		if (f31 > 0.3)
			f31 = 0.3;

		// NOTE: not a compound assignment
		f31 = f31 / (kp->at_0x674 + kp->at_0x675 + stack_0x28);

		kp->at_0x674 = kp->at_0x675;
		kp->at_0x675 = stack_0x28;
		stack_0x2c = stack_0x30;
		stack_0x40 += stack_0x30;

		offset = kp->localSamplingBufIndex - stack_0x30;

		if (offset < 0)
		{
			offset +=
				kp->appSamplingBufIndex + ARRAY_LENGTH(kp->localSamplingBuf);
		}

		do
		{
			if (offset >= (int)ARRAY_LENGTH(kp->localSamplingBuf))
			{
				uwStatus = kp->appSamplingBuf
				    + (offset - ARRAY_LENGTH(kp->localSamplingBuf));
			}
			else
			{
				uwStatus = kp->localSamplingBuf + offset;
			}

			--stack_0x40;
			*(KPADUnifiedWpadStatus *)stack_0x40 = *uwStatus;
			++offset;

			if (offset
			    >= kp->appSamplingBufIndex + ARRAY_LENGTH(kp->localSamplingBuf))
			{
				offset = 0;
			}
		} while (--stack_0x30 > 0);

		OSRestoreInterrupts(intrStatus);
		stack_0x30 = stack_0x2c;
		stack_0x40 = &status[stack_0x30];

		if (kp->buttonProcMode == 1)
		{
			do
			{
				--stack_0x40;
				uwStatus = (KPADUnifiedWpadStatus *)stack_0x40;

				if (kp->status.dev_type != uwStatus->u.core.dev)
				{
					kp->status.dev_type = uwStatus->u.core.dev;
					kp->isNewDevType = true;
				}

				kp->status.wpad_err = uwStatus->u.core.err;
				kp->status.data_format = uwStatus->fmt;
				devType = kp->status.dev_type;

				coreButton = fsButton = clButton = 0xffff;

				switch (uwStatus->u.core.err)
				{
				case WPAD_ESUCCESS:
					if (devType == WPAD_DEV_FS)
					{
						fsButton = uwStatus->u.core.button;
						clButton = WPAD_BUTTON_NONE;
					}
					else if (devType == WPAD_DEV_CLASSIC
					         || devType == WPAD_DEV_GUITAR
					         || devType == WPAD_DEV_DRUM
					         || devType == WPAD_DEV_TAIKO
					         || devType == WPAD_DEV_TRAIN)
					{
						fsButton = WPAD_BUTTON_NONE;
						clButton = uwStatus->u.cl.clButton;
					}
					else if (devType == WPAD_DEV_MPLS_PT_FS
					         && uwStatus->u.mp.stat
					                & WPAD_MPLS_STATUS_EXTENSION_DATA_VALID
					         && uwStatus->u.mp.stat
					                & WPAD_MPLS_STATUS_EXTENSION_CONNECTED)
					{
						fsButton = uwStatus->u.core.button;
						clButton = WPAD_BUTTON_NONE;
					}
					else if (devType == WPAD_DEV_MPLS_PT_CLASSIC
					         && uwStatus->u.mp.stat
					                & WPAD_MPLS_STATUS_EXTENSION_DATA_VALID
					         && uwStatus->u.mp.stat
					                & WPAD_MPLS_STATUS_EXTENSION_CONNECTED)
					{
						fsButton = WPAD_BUTTON_NONE;
						clButton = uwStatus->u.cl.clButton;
					}
					else
					{
						fsButton = clButton = WPAD_BUTTON_NONE;
					}

					ATTR_FALLTHROUGH;

				case WPAD_EBADE:
				case WPAD_EBUSY:
					coreButton = uwStatus->u.core.button & WPAD_BUTTON_ALL;
				}

				if (coreButton == 0xffff)
					coreButton = kp->status.hold & WPAD_BUTTON_ALL;

				if (fsButton == 0xffff)
					fsButton = kp->status.hold;

				if (clButton == 0xffff)
					clButton = kp->status.ex_status.cl.hold;

				read_kpad_button(kp, devType, 1, coreButton, fsButton,
				                 clButton);

				switch (uwStatus->u.core.err)
				{
				case WPAD_ESUCCESS:
					read_kpad_ext(kp, uwStatus);
					ATTR_FALLTHROUGH;

				case WPAD_EBADE:
					read_kpad_acc(kp, uwStatus);
					read_kpad_dpd(kp, uwStatus);
					break;

				default:
					if (!exRead)
						kp->status.dpd_valid_fg = 0;
				}

				*stack_0x40 = kp->status;
			} while (--stack_0x30 > 0);
		}
		else
		{
			devType = WPAD_DEV_NONE;
			coreButton = fsButton = clButton = 0xffff;

			do
			{
				--stack_0x40;
				uwStatus = (KPADUnifiedWpadStatus *)stack_0x40;

				if (kp->status.dev_type != uwStatus->u.core.dev)
				{
					kp->status.dev_type = uwStatus->u.core.dev;
					kp->isNewDevType = true;
				}

				devType = uwStatus->u.core.dev;

				switch (uwStatus->u.core.err)
				{
				case WPAD_ESUCCESS:
					if (devType == WPAD_DEV_FS)
					{
						fsButton = uwStatus->u.core.button;
						clButton = WPAD_BUTTON_NONE;
					}
					else if (devType == WPAD_DEV_CLASSIC
					         || devType == WPAD_DEV_GUITAR
					         || devType == WPAD_DEV_DRUM
					         || devType == WPAD_DEV_TAIKO
					         || devType == WPAD_DEV_TRAIN)
					{
						fsButton = WPAD_BUTTON_NONE;
						clButton = uwStatus->u.cl.clButton;
					}
					else if (devType == WPAD_DEV_MPLS_PT_FS
					         && uwStatus->u.mp.stat
					                & WPAD_MPLS_STATUS_EXTENSION_DATA_VALID
					         && uwStatus->u.mp.stat
					                & WPAD_MPLS_STATUS_EXTENSION_CONNECTED)
					{
						fsButton = uwStatus->u.core.button;
						clButton = WPAD_BUTTON_NONE;
					}
					else if (devType == WPAD_DEV_MPLS_PT_CLASSIC
					         && uwStatus->u.mp.stat
					                & WPAD_MPLS_STATUS_EXTENSION_DATA_VALID
					         && uwStatus->u.mp.stat
					                & WPAD_MPLS_STATUS_EXTENSION_CONNECTED)
					{
						fsButton = WPAD_BUTTON_NONE;
						clButton = uwStatus->u.cl.clButton;
					}
					else
					{
						fsButton = clButton = WPAD_BUTTON_NONE;
					}

					ATTR_FALLTHROUGH;

				case WPAD_EBADE:
				case WPAD_EBUSY:
					coreButton = uwStatus->u.core.button & WPAD_BUTTON_ALL;
				}
			} while (--stack_0x30 > 0);

			if (coreButton == 0xffff)
				coreButton = kp->status.hold & WPAD_BUTTON_ALL;

			if (fsButton == 0xffff)
				fsButton = kp->status.hold;

			if (clButton == 0xffff)
				clButton = kp->status.ex_status.cl.hold;

			read_kpad_button(kp, devType, stack_0x2c, coreButton, fsButton,
			                 clButton);

			stack_0x30 = stack_0x2c;
			stack_0x40 = status + stack_0x30;

			do
			{
				--stack_0x40;
				uwStatus = (KPADUnifiedWpadStatus *)stack_0x40;
				kp->status.wpad_err = uwStatus->u.core.err;
				kp->status.data_format = uwStatus->fmt;

				switch (uwStatus->u.core.err)
				{
				case WPAD_ESUCCESS:
					read_kpad_ext(kp, uwStatus);
					ATTR_FALLTHROUGH;

				case WPAD_EBADE:
					read_kpad_acc(kp, uwStatus);
					read_kpad_dpd(kp, uwStatus);
					break;

				default:
					if (!exRead)
						kp->status.dpd_valid_fg = 0;
				}

				*stack_0x40 = kp->status;
			} while (--stack_0x30 > 0);
		}

		if (KMPLSIsInit())
		{
			stack_0x30 = stack_0x2c;
			kp->appSamplingBufPtr = status + stack_0x30;

			read_kpad_mpls(chan, offset, stack_0x30, f31);

			stack_0x40 = status;
			*stack_0x40 = kp->status;
		}
	}

	kp->currentlyReading = false;

end:
	if (!stack_0x2c)
	{
		if (ret == 0)
		{
			if (exRead)
				*status = localStatus;

			ret = -1;
		}
		else if (ret == -2)
		{
			if (exRead)
			{
				status->dev_type = WPAD_DEV_NONE;
				status->data_format = WPAD_FMT_CORE_BTN;
				status->wpad_err = WPAD_ENODEV;
			}
		}
	}

	if (result)
		*result = ret;

	return stack_0x2c;
}

void KPADInit(void)
{
	KPADInitEx(nullptr, 0);
}

void KPADInitEx(KPADUnifiedWpadStatus *uwStatus, u32 length)
{
	inside_kpads_st *kp;

	KPADChannel chan;
	// What are these
	int stack_0x18 ATTR_UNUSED = 0;
	u8 stack_0x1c[4] ATTR_UNUSED = {0xff, 0xff, 0xff, 0xff};
	u32 i;

	if (kp_initialized)
		return;

	WPADInit();

	BOOL intrStatus = OSDisableInterrupts();

	memset(inside_kpads, 0, sizeof inside_kpads);

	kp_err_dist_max = WPADGetDpdSensitivity() + 1.0f;

	chan = 0;
	do
	{
		kp = &inside_kpads[chan];

		kp->connectCb = WPADSetConnectCallback(chan, &KPADiConnectCallback);
		kp->samplingCb =
			WPADSetSamplingCallback(chan, &KPADiSamplingCallback);
		kp->wpadDpdEnabled = false;
		kp->dpdEnabled = true;
		kp->wpadDpdCmd = WPAD_DPD_DISABLE;
		kp->at_0x646 = false;
		kp->at_0x647 = true;
		kp->mplsDevMode1 = kp->pendingMplsCmd = WPAD_MPLS_DISABLE;
		kp->mplsWasSampled = false;
		kp->at_0x67d = 3;
		kp->activeMplsCmd = kp->mplsDevMode3 = WPADiGetMplsStatus(chan);
		kp->at_0x67b = false;
		kp->at_0x67c = true;

		kp->status.dev_type = WPAD_DEV_NONE;
		kp->status.data_format = WPAD_FMT_CORE_BTN;

		kp->idist = idist_org;
		kp->iaccXY_nrm_hori = iaccXY_nrm_hori;
		kp->isec_nrm_hori = isec_nrm_hori;
		kp->sensorPos = icenter_org;

		calc_dpd2pos_scale(kp);
		kp->posParam.x = kp->horiParam.x = kp->distParam.x = kp->accParam.x =
			0.0f;
		kp->posParam.y = kp->horiParam.y = kp->distParam.y = kp->accParam.y =
			1.0f;

		kp->posPlayMode = kp->horiPlayMode = kp->distPlayMode =
			kp->accPlayMode = false;
		KPADSetBtnRepeat(chan, 0.0f, 0.0f);
		kp->buttonProcMode = 0;
		KPADEnableAimingMode(chan);
		kp->needsNewGravityUnit = true;
		kp->reviseEnabled = false;

		if (length && uwStatus)
		{
			OSAssert_Line(2472, ( length % 4 ) == 0);
			kp->appSamplingBufIndex = length / 4;
			kp->appSamplingBuf = &uwStatus[chan * kp->appSamplingBufIndex];
		}
		else
		{
			kp->appSamplingBufIndex = 0;
			kp->appSamplingBuf = nullptr;
		}

		for (i = 0; i < (int)ARRAY_LENGTH(kp->localSamplingBuf); ++i)
			kp->localSamplingBuf[i].u.core.err = WPAD_ENODEV;

		for (i = 0; i < kp->appSamplingBufIndex; ++i)
			kp->appSamplingBuf[i].u.core.err = WPAD_ENODEV;
	} while (++chan < WPAD_MAX_CONTROLLERS);

	kp_wbc_issued	= 0;
	kp_wbc_enabled	= WBCEnableNotStarted;

	KPADResetWbcTgcWeight();
	kp_wbc_zero_point_done	= 0;

	WPADSetCallbackByKPAD(true);
	KPADReset();

	kp_fs_rot[0][0]	= 1.0f;
	kp_fs_rot[0][1]	= 0.0f;
	kp_fs_rot[0][2]	= 0.0f;
	kp_fs_rot[0][3]	= 0.0f;

	kp_fs_rot[1][0]	= 0.0f;
	kp_fs_rot[1][1]	= cos(DEG_TO_RAD(kp_fs_revise_deg));
	kp_fs_rot[1][2]	= -sin(DEG_TO_RAD(kp_fs_revise_deg));
	kp_fs_rot[1][3]	= 0.0f;

	kp_fs_rot[2][0]	= 0.0f;
	kp_fs_rot[2][1]	= sin(DEG_TO_RAD(kp_fs_revise_deg));
	kp_fs_rot[2][2]	= cos(DEG_TO_RAD(kp_fs_revise_deg));
	kp_fs_rot[2][3]	= 0.0f;

	kp_initialized = true;

	OSRestoreInterrupts(intrStatus);

	OSRegisterVersion(__KPADVersion);
}

void KPADShutdown(void)
{
	inside_kpads_st *kp;

	WPADSetCallbackByKPAD(false);

	KPADChannel chan;
	for (chan = KPAD_CHAN0; chan < WPAD_MAX_CONTROLLERS; ++chan)
	{
		kp = &inside_kpads[chan];

		if (kp->samplingCb)
			WPADSetSamplingCallback(chan, kp->samplingCb);
		else
			WPADSetSamplingCallback(chan, nullptr);

		if (kp->connectCb)
			WPADSetConnectCallback(chan, kp->connectCb);
		else
			WPADSetConnectCallback(chan, nullptr);
	}

	KMPLSShutdown();
	kp_initialized = false;
}

void KPADReset(void)
{
	set_obj_interval(kp_obj_interval);

	KPADChannel chan = WPAD_MAX_CONTROLLERS - 1;
	do
	{
		if (WPADGetStatus() == WPAD_LIB_STATUS_3)
			WPADControlMotor(chan, WPAD_MOTOR_STOP);

		inside_kpads[chan].needsReset = true;
	} while (--chan >= 0);
}

static void KPADiConnectCallback(KPADChannel chan, WPADResult result)
{
	inside_kpads_st *kp = &inside_kpads[chan];

	// ?
	WPADAccGravityUnit accGravity ATTR_UNUSED = {1, 1, 1};

	u32 i;

	if (result == WPAD_ESUCCESS)
	{
		kp->devType = WPAD_DEV_CORE;
		kp->needsNewGravityUnit = true;

		// We do a little trickery. A little tomfoolery
		WPADSetCallbackByKPAD(false);
		WPADSetSamplingCallback(chan, &KPADiSamplingCallback);
		WPADSetCallbackByKPAD(true);

		kp->wpadDpdEnabled = false;
		kp->wpadDpdCmd = WPAD_DPD_DISABLE;
		kp->controllingDpd = false;

		if (chan == WPAD_CHAN3)
		{
			kp_wbc_issued = false;
			kp_wbc_enabled = WBCEnableNotStarted;

			if (kp_wbc_zero_point_done != 4)
				kp_wbc_zero_point_done = 0;

			kp_wbc_tgc_weight_done = 0;
			kp_wbc_tgc_weight_err = -2;
		}

		kp->mplsWasSampled = false;
		kp->at_0x67d = 1;
		kp->mplsDevMode3 = kp->pendingMplsCmd;
		kp->activeMplsCmd = WPAD_MPLS_DISABLE;
		kp->at_0x67b = false;
		kp->at_0x67c = true;
	}
	else
	{
		for (i = 0; i < ARRAY_LENGTH(kp->localSamplingBuf); ++i)
			kp->localSamplingBuf[i].u.core.err = WPAD_ENODEV;

		for (i = 0; i < kp->appSamplingBufIndex; ++i)
			kp->appSamplingBuf[i].u.core.err = WPAD_ENODEV;

		KPADiControlDpdCallback(chan, WPAD_ESUCCESS);

		if (!kp->at_0x67c && kp->controlMplsCb)
			(*kp->controlMplsCb)(chan, 1);
	}

	if (kp->connectCb)
		(*kp->connectCb)(chan, result);
}

KPADConnectCallback *KPADSetConnectCallback(KPADChannel chan,
                                            KPADConnectCallback *cb)
{
	WPADConnectCallback *old;

	OSAssert_Line(2645, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	BOOL intrStatus = OSDisableInterrupts();

	old = inside_kpads[chan].connectCb;
	inside_kpads[chan].connectCb = cb;

	OSRestoreInterrupts(intrStatus);

	return old;
}

static void KPADiControlWbcCallback(KPADChannel chan, WPADResult result)
{
	// ERRATUM: uses chan before assertion
	inside_kpads_st *kp ATTR_UNUSED = &inside_kpads[chan];

	OSAssert_Line(2662, chan == WPAD_CHAN3);

	kp_wbc_enabled =
		result == WPAD_ESUCCESS ? WBCEnableStage1 : WBCEnableNotStarted;
	kp_wbc_failure_count = 0;
	kp_wbc_issued = false;
}

static void KPADiUpdateTempWbcCallback(KPADChannel chan, WPADResult result)
{
	// ERRATUM: uses chan before assertion
	inside_kpads_st *kp ATTR_UNUSED = &inside_kpads[chan];

	OSAssert_Line(2673, chan == WPAD_CHAN3);

	if (result == WPAD_ESUCCESS)
	{
		kp_wbc_zero_point_done = 2;
		kp_wbc_zero_point_wait = 100;
		kp_wbc_ave_sample_count = 0;
	}

	kp_wbc_issued = false;
}

void KPADDisableDPD(KPADChannel chan)
{
	OSAssert_Line(2714, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	inside_kpads[chan].dpdEnabled = false;
}

void KPADEnableDPD(KPADChannel chan)
{
	OSAssert_Line(2720, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	inside_kpads[chan].dpdEnabled = true;
}

void KPADSetControlDpdCallback(KPADChannel chan, KPADCallback *cb)
{
	// ERRATUM: uses chan before assertion
	inside_kpads_st *kp = &inside_kpads[chan];

	OSAssert_Line(2729, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	BOOL intrStatus = OSDisableInterrupts();

	kp->controlDpdCb = cb;

	OSRestoreInterrupts(intrStatus);
}

void KPADResetWbcZeroPoint(void)
{
	// is this like a macro or something? they keep doing this
	inside_kpads_st *kp ATTR_UNUSED = &inside_kpads[WPAD_CHAN3];

	kp_wbc_zero_point_done = 1;
	kp_wbc_tgc_weight_done = 0;
	kp_wbc_ave_sample_count = 0;

	int i;
	for (i = 0; i < (int)ARRAY_LENGTH(kp_wbc_ave_sample); ++i)
		kp_wbc_ave_sample[i] = 0.0;
}

void KPADResetWbcTgcWeight(void)
{
	inside_kpads_st *kp ATTR_UNUSED = &inside_kpads[WPAD_CHAN3];

	KPADResetWbcZeroPoint();
	kp_wbc_tgc_weight_err = -2;
	kp_wbc_tgc_weight_wait = 0;
	kp_wbc_tgc_weight_done = 1;
}

static void KPADiControlDpdCallback(KPADChannel chan, WPADResult result)
{
	inside_kpads_st *kp = &inside_kpads[chan];

	if (result == WPAD_ESUCCESS && kp->controlDpdCb && !kp->at_0x647)
	{
		kp->at_0x647 = true;
		(*kp->controlDpdCb)(chan, WPAD_DPD_BASIC);
		kp->at_0x646 = false;
	}

	kp->wpadDpdEnabled = WPADIsDpdEnabled(chan);
	kp->controllingDpd = false;
}

void KPADDisableMpls(KPADChannel chan)
{
	OSAssert_Line(2757, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	inside_kpads[chan].pendingMplsCmd = WPAD_MPLS_DISABLE;

	if (inside_kpads[chan].at_0x67c)
		inside_kpads[chan].mplsDevMode3 = WPAD_MPLS_DISABLE;
}

void KPADEnableMpls(KPADChannel chan, WPADMplsCommand mplsCmd)
{
	OSAssert_Line(2766, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	inside_kpads[chan].pendingMplsCmd = mplsCmd;

	if (inside_kpads[chan].at_0x67c)
		inside_kpads[chan].mplsDevMode3 = mplsCmd;
}

u8 KPADGetMplsStatus(KPADChannel chan)
{
	return WPADiGetMplsStatus(chan);
}

void KPADSetControlMplsCallback(KPADChannel chan, KPADCallback *cb)
{
	// ERRATUM: uses chan before assertion
	inside_kpads_st *kp = &inside_kpads[chan];

	OSAssert_Line(2783, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	BOOL intrStatus = OSDisableInterrupts();

	kp->controlMplsCb = cb;

	OSRestoreInterrupts(intrStatus);
}

static void KPADiControlMplsCallback(KPADChannel chan, WPADResult result)
{
	inside_kpads_st *kp = &inside_kpads[chan];

	WPADResult next;

	kp->mplsWasSampled = false;
	kp->at_0x67d = 1;

	if (result == WPAD_ESUCCESS)
	{
		kp->activeMplsCmd = kp->mplsDevMode1;
		return;
	}

	if (result == WPAD_ETRANSFER || result == WPAD_EINVAL)
	{
		if (!kp->at_0x67c && kp->mplsDevMode1
		    && kp->mplsDevMode1 == kp->mplsDevMode3)
		{
			switch (kp->mplsDevMode1)
			{
			case WPAD_DEV_MODE_MPLS_PT_FS:
				next = WPAD_EBUSY;
				break;

			case WPAD_DEV_MODE_MPLS_PT_CLASSIC:
				next = WPAD_ETRANSFER;
				break;

			default:
				next = WPAD_ENODEV;
				break;
			}

			kp->at_0x67c = false;

			if (kp->controlMplsCb)
				(*kp->controlMplsCb)(chan, next);

			kp->at_0x67b = true;
		}
	}

	kp->activeMplsCmd = WPAD_MPLS_DISABLE;
}

u32 KPADGetMplsWorkSize(void)
{
	return KMPLSGetLibWorkSize();
}

void KPADSetMplsWorkarea(void *work)
{
	if (work)
		KMPLSInit(work);
	else
		KMPLSShutdown();
}

void KPADSetMplsAngle(KPADChannel chan, f32 x, f32 y, f32 z)
{
	KMPLSSetAngle(chan, x, y, z);
}

void KPADSetMplsDirection(KPADChannel chan, void *param_2)
{
	KMPLSSetDirection(chan, param_2);
}

void KPADSetMplsDirectionMag(KPADChannel chan, f32 mag)
{
	KMPLSSetDirectionMag(chan, mag);
}

void KPADStartMplsCalibration(KPADChannel chan)
{
	KMPLSStartCalibration(chan);
}

f32 KPADWorkMplsCalibration(KPADChannel chan)
{
	return KMPLSWorkCalibration(chan);
}

void KPADStopMplsCalibration(KPADChannel chan)
{
	KMPLSStopCalibration(chan);
}

void KPADEnableMplsZeroPlay(KPADChannel chan)
{
	KMPLSEnableZeroPlay(chan);
}

void KPADEnableMplsDirRevise(KPADChannel chan)
{
	KMPLSEnableDirRevise(chan);
}

void KPADEnableMplsAccRevise(KPADChannel chan)
{
	KMPLSEnableAccRevise(chan);
}

void KPADEnableMplsDpdRevise(KPADChannel chan)
{
	KMPLSEnableDpdRevise(chan);
}

void KPADDisableMplsZeroPlay(KPADChannel chan)
{
	KMPLSDisableZeroPlay(chan);
}

void KPADDisableMplsDirRevise(KPADChannel chan)
{
	KMPLSDisableDirRevise(chan);
}

void KPADDisableMplsAccRevise(KPADChannel chan)
{
	KMPLSDisableAccRevise(chan);
}

void KPADDisableMplsDpdRevise(KPADChannel chan)
{
	KMPLSDisableDpdRevise(chan);
}

f32 KPADIsEnableMplsZeroPlay(KPADChannel chan)
{
	return KMPLSIsEnableZeroPlay(chan);
}

f32 KPADIsEnableMplsZeroDrift(KPADChannel chan)
{
	return KMPLSIsEnableZeroDrift(chan);
}

f32 KPADIsEnableMplsDirRevise(KPADChannel chan)
{
	return KMPLSIsEnableDirRevise(chan);
}

f32 KPADIsEnableMplsAccRevise(KPADChannel chan)
{
	return KMPLSIsEnableAccRevise(chan);
}

f32 KPADIsEnableMplsDpdRevise(KPADChannel chan)
{
	return KMPLSIsEnableDpdRevise(chan);
}

void KPADSetMplsZeroPlayParam(KPADChannel chan, f32 param1)
{
	KMPLSSetZeroPlayParam(chan, param1);
}

void KPADSetMplsDirReviseParam(KPADChannel chan, f32 param1)
{
	KMPLSSetDirReviseParam(chan, param1);
}

void KPADSetMplsAccReviseParam(KPADChannel chan, f32 param1, f32 param2)
{
	KMPLSSetAccReviseParam(chan, param1, param2);
}

void KPADSetMplsDpdReviseParam(KPADChannel chan, f32 param1)
{
	KMPLSSetDpdReviseParam(chan, param1);
}

void KPADSetMplsDirReviseBase(KPADChannel chan, KPADMPDir *base)
{
	KMPLSSetDirReviseBase(chan, base);
}

void KPADGetMplsZeroPlayParam(KPADChannel chan, f32 *param1)
{
	KMPLSGetZeroPlayParam(chan, param1);
}

void KPADGetMplsDirReviseParam(KPADChannel chan, f32 *param1)
{
	KMPLSGetDirReviseParam(chan, param1);
}

void KPADGetMplsAccReviseParam(KPADChannel chan, f32 *param1, f32 *param2)
{
	KMPLSGetAccReviseParam(chan, param1, param2);
}

void KPADGetMplsDpdReviseParam(KPADChannel chan, f32 *param1)
{
	KMPLSGetDpdReviseParam(chan, param1);
}

void KPADInitMplsZeroPlayParam(KPADChannel chan)
{
	KMPLSInitZeroPlayParam(chan);
}

void KPADInitMplsDirReviseParam(KPADChannel chan)
{
	KMPLSInitDirReviseParam(chan);
}

void KPADInitMplsAccReviseParam(KPADChannel chan)
{
	KMPLSInitAccReviseParam(chan);
}

void KPADInitMplsDpdReviseParam(KPADChannel chan)
{
	KMPLSInitDpdReviseParam(chan);
}

void KPADInitMplsZeroDriftMode(KPADChannel chan)
{
	KMPLSInitZeroDriftParam(chan);
}

void KPADSetMplsZeroDriftMode(KPADChannel chan, KPADMplsZeroDriftMode mode)
{
	OSAssert_Line(2876, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	if (mode == KPAD_MPLS_ZERO_DRIFT_LOOSE)
		KMPLSSetZeroDriftLoose(chan);
	else if (mode == KPAD_MPLS_ZERO_DRIFT_TIGHT)
		KMPLSSetZeroDriftTight(chan);
	else
		KMPLSSetZeroDriftStandard(chan);
}

void KPADGetMplsZeroDriftMode(KPADChannel chan, KPADMplsZeroDriftMode *mode)
{
	OSAssert_Line(2889, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	if (mode == nullptr)
		return;

	f32 param1;
	s32 param2 ATTR_UNUSED;
	f32 param3;
	KMPLSGetZeroDriftParam(chan, &param1, &param2, &param3);

	if (param1 == 0.005f && param3 == 0.01f)
		*mode = KPAD_MPLS_ZERO_DRIFT_TIGHT;
	else if (param1 == 0.02f && param3 == 0.04f)
		*mode = KPAD_MPLS_ZERO_DRIFT_LOOSE;
	else
		*mode = KPAD_MPLS_ZERO_DRIFT_STANDARD;
}

void KPADResetMpls(KPADChannel chan)
{
	inside_kpads_st *kp = &inside_kpads[chan];

	kp->tickQueue[0] =
	kp->tickQueue[1] =
	kp->tickQueue[2] =
	kp->tickQueue[3] =
	kp->tickQueue[4] = OSGetTick();

	kp->at_0x674 = kp->at_0x675 = 0;

	KMPLSReset(chan);
}

static void KPADiSamplingCallback(KPADChannel chan)
{
	// clang-format off
	static u8 table[][2] =
	{
		{WPAD_DPD_DISABLE,  WPAD_FMT_CORE_BTN_ACC},
		{WPAD_DPD_STANDARD, WPAD_FMT_CORE_BTN_ACC_DPD},

		{WPAD_DPD_DISABLE,  WPAD_FMT_FS_BTN_ACC},
		{WPAD_DPD_BASIC,    WPAD_FMT_FS_BTN_ACC_DPD},

		{WPAD_DPD_DISABLE,  WPAD_FMT_CLASSIC_BTN_ACC},
		{WPAD_DPD_BASIC,    WPAD_FMT_CLASSIC_BTN_ACC_DPD},

		{WPAD_DPD_DISABLE,  WPAD_FMT_GUITAR},
		{WPAD_DPD_BASIC,    WPAD_FMT_GUITAR},

		{WPAD_DPD_DISABLE,  WPAD_FMT_DRUM},
		{WPAD_DPD_BASIC,    WPAD_FMT_DRUM},

		{WPAD_DPD_DISABLE,  WPAD_FMT_BALANCE_CHECKER},
		{WPAD_DPD_DISABLE,  WPAD_FMT_BALANCE_CHECKER},

		{WPAD_DPD_DISABLE,  WPAD_FMT_TRAIN},
		{WPAD_DPD_DISABLE,  WPAD_FMT_TRAIN},

		{WPAD_DPD_DISABLE,  WPAD_FMT_TAIKO},
		{WPAD_DPD_BASIC,    WPAD_FMT_TAIKO},

		{WPAD_DPD_DISABLE,  WPAD_FMT_MOTION_PLUS},
		{WPAD_DPD_BASIC,    WPAD_FMT_MOTION_PLUS},

		{WPAD_DPD_DISABLE,  WPAD_FMT_BULK},
		{WPAD_DPD_DISABLE,  WPAD_FMT_BULK},
	};
	// clang-format on

	// ERRATUM: uses chan before assertion
	inside_kpads_st *kp = &inside_kpads[chan];

	WPADAccGravityUnit accGravityUnit = {1, 1, 1};
	KPADUnifiedWpadStatus *uwStatus;
	WPADDeviceType devType;
	WPADDpdCommand dpdCmd;
	WPADResult wpadErr;
	WPADMplsCommand mplsCmd;

	OSAssert_Line(2954, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	if (WPADProbe(chan, &devType) == WPAD_ENODEV)
		goto end;

	u32 index = kp->localSamplingBufIndex;
	if (index >= ARRAY_LENGTH(kp->localSamplingBuf) + kp->appSamplingBufIndex)
		index = 0;

	if (index >= ARRAY_LENGTH(kp->localSamplingBuf))
	{
		uwStatus =
			kp->appSamplingBuf + (index - ARRAY_LENGTH(kp->localSamplingBuf));
	}
	else
	{
		uwStatus = kp->localSamplingBuf + index;
	}

	WPADRead(chan, &uwStatus->u.core);
	uwStatus->fmt = WPADGetDataFormat(chan);

	kp->localSamplingBufIndex = index + 1;

	f32 sensorHeight;
	if (kp->at_0x17b
	    < ARRAY_LENGTH(kp->localSamplingBuf) + kp->appSamplingBufIndex)
	{
		++kp->at_0x17b;
	}

	kp->tickQueue[3] = kp->tickQueue[4];
	kp->tickQueue[4] = OSGetTick();

	if (devType != kp->devType && devType == WPAD_DEV_FS)
		kp->needsNewGravityUnit = true;

	if (kp->needsNewGravityUnit)
	{
		WPADGetAccGravityUnit(chan, WPAD_ACC_GRAVITY_UNIT_CORE,
		                      &accGravityUnit);

		if (accGravityUnit.x * accGravityUnit.y * accGravityUnit.z)
		{
			kp->coreGravityUnit.x = 1.0f / accGravityUnit.x;
			kp->coreGravityUnit.y = 1.0f / accGravityUnit.y;
			kp->coreGravityUnit.z = 1.0f / accGravityUnit.z;

			kp->needsNewGravityUnit = false;
		}
		else
		{
			kp->coreGravityUnit.x = 0.01f;
			kp->coreGravityUnit.y = 0.01f;
			kp->coreGravityUnit.z = 0.01f;
		}

		WPADGetAccGravityUnit(chan, WPAD_ACC_GRAVITY_UNIT_FS, &accGravityUnit);

		if (accGravityUnit.x * accGravityUnit.y * accGravityUnit.z)
		{
			kp->fsGravityUnit.x = 1.0f / accGravityUnit.x;
			kp->fsGravityUnit.y = 1.0f / accGravityUnit.y;
			kp->fsGravityUnit.z = 1.0f / accGravityUnit.z;
		}
		else
		{
			kp->fsGravityUnit.x = 0.005f;
			kp->fsGravityUnit.y = 0.005f;
			kp->fsGravityUnit.z = 0.005f;
		}
	}

	if (kp->aimingModeChanged)
	{
		if (kp->aimingModeEnabled)
			if (WPADGetSensorBarPosition() == 1)
				sensorHeight = 0.2f;
			else
				sensorHeight = -0.2f;
		else
			sensorHeight = 0.0f;

		KPADSetSensorHeight(chan, sensorHeight);

		kp->aimingModeChanged = false;
	}

	switch (devType)
	{
	case WPAD_DEV_CORE:
	case WPAD_DEV_251:
	case WPAD_DEV_252:
	case WPAD_DEV_INITIALIZING:
		index = 0;
		break;

	case WPAD_DEV_FS:
		index = 2;
		break;

	case WPAD_DEV_CLASSIC:
		index = 4;
		break;

	case WPAD_DEV_GUITAR:
		index = 6;
		break;

	case WPAD_DEV_DRUM:
		index = 8;
		break;

	case WPAD_DEV_BALANCE_CHECKER:
		index = 10;
		break;

	case WPAD_DEV_TRAIN:
		index = 12;
		break;

	case WPAD_DEV_TAIKO:
		index = 14;
		break;

	case WPAD_DEV_MOTION_PLUS:
	case WPAD_DEV_MPLS_PT_FS:
	case WPAD_DEV_MPLS_PT_CLASSIC:
	case WPAD_DEV_MPLS_PT_UNKNOWN:
		index = 16;
		break;

	case WPAD_DEV_BULK_1:
	case WPAD_DEV_BULK_2:
	case WPAD_DEV_BULK_3:
	case WPAD_DEV_BULK_4:
	case WPAD_DEV_BULK_5:
	case WPAD_DEV_BULK_6:
	case WPAD_DEV_BULK_7:
	case WPAD_DEV_BULK_8:
		index = 18;
		break;
	default:
		goto end;
	}

	if (kp->dpdEnabled)
		++index;

	dpdCmd = WPADIsDpdEnabled(chan) ? kp->wpadDpdCmd : WPAD_DPD_DISABLE;
	if (dpdCmd != table[index][0])
	{
		if (kp->controlDpdCb && !kp->at_0x646)
		{
			kp->at_0x646 = true;
			(*kp->controlDpdCb)(chan, WPAD_DPD_DISABLE);
			kp->at_0x647 = false;
		}

		if (!kp->controllingDpd)
		{
			kp->controllingDpd = true;

			if (WPADControlDpd(chan, table[index][0], &KPADiControlDpdCallback)
			    == WPAD_ESUCCESS)
			{
				kp->wpadDpdCmd = table[index][0];
			}
		}
	}
	else
	{
		if (uwStatus->fmt != table[index][1])
			WPADSetDataFormat(chan, table[index][1]);
	}

	if (devType == WPAD_DEV_BALANCE_CHECKER && !kp_wbc_issued)
	{
		if (kp_wbc_enabled > WBCEnableStage2)
		{
			if (kp_wbc_zero_point_done == 1)
			{
				if (WPADControlBLC(chan, WPAD_BLC_DISABLE,
				                   &KPADiUpdateTempWbcCallback)
				    == WPAD_ESUCCESS)
				{
					kp_wbc_issued = true;
				}
			}
			else if (kp_wbc_zero_point_done == 2)
			{
				if (!kp_wbc_zero_point_wait)
				{
					if (uwStatus->u.bl.temp == 127
					    || uwStatus->u.bl.temp == -128)
					{
						kp_wbc_zero_point_done = 1;
					}
					else
					{
						kp_wbc_zero_point_done = 3;
						kp_wbc_ave_sample_count = 0;
					}
				}
				else
				{
					--kp_wbc_zero_point_wait;
				}
			}
			else if (kp_wbc_zero_point_done == 3
			         && uwStatus->u.core.err == WPAD_ESUCCESS)
			{
				++kp_wbc_ave_sample_count;

				for (index = 0; index < ARRAY_LENGTH(kp_wbc_ave_sample);
				     ++index)
				{
					kp_wbc_ave_sample[index] = ((kp_wbc_ave_sample_count - 1)
					                                * kp_wbc_ave_sample[index]
					                            + uwStatus->u.bl.press[index])
					                         / kp_wbc_ave_sample_count;
				}

				if (kp_wbc_ave_sample_count == kp_wbc_ave_count)
				{
					kp_wbc_zero_point_done = 4;
					kp_wbc_ave_sample_count = 0;

					WBCRead(&uwStatus->u.bl, kp_wbc_weight_ave, 4);
					WBCSetZEROPoint(kp_wbc_ave_sample, 4);
				}
			}
		}
		else if (kp_wbc_enabled == WBCEnableNotStarted)
		{
			if (WPADControlBLC(chan, WPAD_BLC_ENABLE, &KPADiControlWbcCallback)
			    == WPAD_ESUCCESS)
			{
				kp_wbc_issued = true;
			}
		}
		else if (kp_wbc_enabled == WBCEnableStage1)
		{
			if (kp_wbc_failure_count < MAX_WBC_FAILURE_COUNT)
			{
				if (WBCSetupCalibration() == 1)
				{
					kp_wbc_enabled = WBCEnableStage2;
					kp_wbc_zero_point_wait = 0;
				}
				else
				{
					++kp_wbc_failure_count;
				}
			}
			else
			{
				kp_wbc_enabled = WBCEnableFailed;
			}
		}
		else if (kp_wbc_enabled == WBCEnableStage2)
		{
			if (WBCGetCalibrationStatus())
			{
				kp_wbc_enabled = WBCEnableSucceeded;
				kp_wbc_failure_count = 0;
			}
			else if (++kp_wbc_zero_point_wait > kp_wbc_calib_count)
			{
				++kp_wbc_failure_count;
				kp_wbc_enabled = WBCEnableStage1;
			}
		}
	}

	mplsCmd = WPADiGetMplsStatus(chan);

	if (!kp->mplsWasSampled)
	{
		if (kp->at_0x67b && mplsCmd == kp->activeMplsCmd
		    && mplsCmd == kp->mplsDevMode3)
		{
			if (kp->mplsDevMode3 == kp->pendingMplsCmd)
			{
				kp->at_0x67b = false;

				if (kp->controlMplsCb)
					(*kp->controlMplsCb)(chan, 1);

				kp->at_0x67c = true;
			}
			else
			{
				kp->mplsDevMode3 = kp->pendingMplsCmd;
			}
		}
		else
		{
			if (kp->at_0x67d)
			{
				--kp->at_0x67d;
				goto end;
			}

			kp->mplsDevMode1 = kp->mplsDevMode3;

			if (mplsCmd && mplsCmd != kp->mplsDevMode1)
				kp->mplsDevMode1 = WPAD_MPLS_DISABLE;

			if (mplsCmd != kp->mplsDevMode1)
			{
				if (!kp->at_0x67b)
					kp->activeMplsCmd = WPAD_MPLS_DISABLE;

				kp->mplsWasSampled = true;

				if (!kp->at_0x67b)
				{
					kp->at_0x67b = true;

					if (kp->controlMplsCb)
					{
						if (mplsCmd != WPAD_MPLS_DISABLE
						    || kp->mplsDevMode3 == WPAD_MPLS_DISABLE
						    || (kp->devType != WPAD_DEV_MOTION_PLUS
						        && kp->devType != WPAD_DEV_MPLS_PT_FS
						        && kp->devType != WPAD_DEV_MPLS_PT_CLASSIC
						        && kp->devType != WPAD_DEV_MPLS_PT_UNKNOWN))
						{
							wpadErr = WPAD_ESUCCESS;
						}
						else if (kp->mplsDevMode3 == WPAD_MPLS_FS)
						{
							wpadErr = WPAD_EBUSY;
						}
						else if (kp->mplsDevMode3 == WPAD_MPLS_CLASSIC)
						{
							wpadErr = WPAD_ETRANSFER;
						}
						else
						{
							wpadErr = WPAD_ENODEV;
						}

						(*kp->controlMplsCb)(chan, wpadErr);

						if (wpadErr < WPAD_ESUCCESS
						    && kp->pendingMplsCmd == WPAD_MPLS_DISABLE)
						{
							kp->mplsDevMode3 = kp->pendingMplsCmd;
							kp->mplsDevMode1 = kp->pendingMplsCmd;
						}
					}

					kp->at_0x67c = false;
				}

				WPADiControlMpls(chan, kp->mplsDevMode1,
				                 &KPADiControlMplsCallback);
			}
			else
			{
				kp->mplsDevMode3 = kp->pendingMplsCmd;
			}
		}

		if (!mplsCmd && !kp->pendingMplsCmd)
			kp->mplsDevMode3 = kp->pendingMplsCmd;
	}

end:
	kp->devType = devType;

	if (kp->samplingCb)
		(*kp->samplingCb)(chan);
}

KPADSamplingCallback *KPADSetSamplingCallback(KPADChannel chan,
                                              KPADSamplingCallback *cb)
{
	KPADSamplingCallback *old;

	OSAssert_Line(3244, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	BOOL intrStatus = OSDisableInterrupts();

	old = inside_kpads[chan].samplingCb;
	inside_kpads[chan].samplingCb = cb;

	OSRestoreInterrupts(intrStatus);

	return old;
}

void KPADGetUnifiedWpadStatus(KPADChannel chan, KPADUnifiedWpadStatus *uwStatus,
                              u32 count)
{
	// ERRATUM: uses chan before assertion
	inside_kpads_st *kp = &inside_kpads[chan];
	KPADUnifiedWpadStatus *uw;

	OSAssert_Line(3264, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	if (count > ARRAY_LENGTH(kp->localSamplingBuf) + kp->appSamplingBufIndex)
		count = ARRAY_LENGTH(kp->localSamplingBuf) + kp->appSamplingBufIndex;

	BOOL intrStatus = OSDisableInterrupts();

	int index = kp->localSamplingBufIndex;
	while (count--)
	{
		if (index == 0)
		{
			index = kp->appSamplingBufIndex + ARRAY_LENGTH(kp->localSamplingBuf)
			      - 1;
		}
		else
		{
			--index;
		}

		if (index >= ARRAY_LENGTH(kp->localSamplingBuf))
		{
			uw =
				&kp->appSamplingBuf[index - ARRAY_LENGTH(kp->localSamplingBuf)];
		}
		else
		{
			uw = &kp->localSamplingBuf[index];
		}

		if (WPADGetStatus() != WPAD_LIB_STATUS_3)
			uw->u.core.err = WPAD_EINVAL;

		memcpy(uwStatus, uw, sizeof *uwStatus);

		++uwStatus;
	}

	OSRestoreInterrupts(intrStatus);
}

void KPADEnableStickCrossClamp(void)
{
	kp_stick_clamp_cross = true;
}

void KPADDisableStickCrossClamp(void)
{
	kp_stick_clamp_cross = false;
}

void KPADSetReviseMode(KPADChannel chan, BOOL enabled)
{
	// ERRATUM: uses chan before assertion
	inside_kpads_st *kp = &inside_kpads[chan];

	OSAssert_Line(3316, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	kp->reviseEnabled = enabled;
}

f32 KPADReviseAcc(Vec *acc)
{
	MTXMultVec(kp_fs_rot, acc, acc);

	return kp_fs_revise_deg;
}

f32 KPADGetReviseAngle(void)
{
	return kp_fs_revise_deg;
}

void KPADSetPosPlayMode(KPADChannel chan, BOOL mode)
{
	OSAssert_Line(3338, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	inside_kpads[chan].posPlayMode = mode;
}

void KPADSetHoriPlayMode(KPADChannel chan, BOOL mode)
{
	OSAssert_Line(3344, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	inside_kpads[chan].horiPlayMode = mode;
}

void KPADSetDistPlayMode(KPADChannel chan, BOOL mode)
{
	OSAssert_Line(3350, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	inside_kpads[chan].distPlayMode = mode;
}

void KPADSetAccPlayMode(KPADChannel chan, BOOL mode)
{
	OSAssert_Line(3356, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	inside_kpads[chan].accPlayMode = mode;
}

BOOL KPADGetPosPlayMode(KPADChannel chan)
{
	OSAssert_Line(3362, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	return inside_kpads[chan].posPlayMode;
}

BOOL KPADGetHoriPlayMode(KPADChannel chan)
{
	OSAssert_Line(3368, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	return inside_kpads[chan].horiPlayMode;
}

BOOL KPADGetDistPlayMode(KPADChannel chan)
{
	OSAssert_Line(3374, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	return inside_kpads[chan].distPlayMode;
}

BOOL KPADGetAccPlayMode(KPADChannel chan)
{
	OSAssert_Line(3380, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	return inside_kpads[chan].accPlayMode;
}

void KPADSetButtonProcMode(KPADChannel chan, u8 mode)
{
	OSAssert_Line(3389, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	inside_kpads[chan].buttonProcMode = mode;
}

u8 KPADGetButtonProcMode(KPADChannel chan)
{
	OSAssert_Line(3395, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	return inside_kpads[chan].buttonProcMode;
}

BOOL KPADSetConfig(KPADChannel chan, KPADConfig *config)
{
	OSAssert_Line(3406, (0 <= chan) && (chan < WPAD_MAX_CONTROLLERS));

	if (WPADProbe(chan, nullptr) == WPAD_ENODEV)
	{
		kp_config[chan] = *config;
		return true;
	}
	else
	{
		return false;
	}
}

void KPADSetMplsMagnification(KPADChannel chan, f32 x, f32 y, f32 z)
{
	KMPLSSetDegreeMag(chan, x, y, z);
}
