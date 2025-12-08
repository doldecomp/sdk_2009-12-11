#include "HBMController.h"

/*******************************************************************************
 * headers
 */

#include <revolution/types.h>

#include "HBMRemoteSpk.h"

#include <revolution/hbm.h>
// #include <revolution/os/OSTime.h>
#include <revolution/wpad.h>

#include <context_rvl.h>

/*******************************************************************************
 * variables
 */

namespace homebutton
{
	// .bss
	bool Controller::sBatteryFlag[WPAD_MAX_CONTROLLERS];
	OSAlarm Controller::sAlarm[WPAD_MAX_CONTROLLERS];
	OSAlarm Controller::sAlarmSoundOff[WPAD_MAX_CONTROLLERS];
	Controller *Controller::sThis[WPAD_MAX_CONTROLLERS];
	bool Controller::sSetInfoAsync[WPAD_MAX_CONTROLLERS];
} // namespace homebutton

/*******************************************************************************
 * functions
 */

namespace homebutton {

void Controller::wpadConnectCallback(WPADChannel chan, WPADResult result)
{
	if (!sThis[chan])
		return;

	switch (result)
	{
	case WPAD_ESUCCESS:
		if (!sThis[chan]->mCallbackFlag)
		{
			WPADSetExtensionCallback(chan, &wpadExtensionCallback);
			sThis[chan]->mCallbackFlag = true;
		}

		WPADControlSpeaker(chan, WPAD_SPEAKER_DISABLE, nullptr);
		break;

	case WPAD_ENODEV:
		WPADSetExtensionCallback(chan, sThis[chan]->mOldExtensionCallback);
		sThis[chan]->mCallbackFlag = false;
		sThis[chan]->mCheckSoundTimeFlag = false;
		sThis[chan]->mCheckSoundIntervalFlag = false;
		break;
	}

	if (sThis[chan]->mOldConnectCallback
	    && sThis[chan]->mOldConnectCallback != &wpadConnectCallback)
	{
		(*sThis[chan]->mOldConnectCallback)(chan, result);
	}
}

// WPADDeviceType result;
void Controller::wpadExtensionCallback(WPADChannel chan, s32 result)
{
	if (!sThis[chan])
		return;

	switch (result)
	{
	case WPAD_DEV_INITIALIZING:
		sThis[chan]->soundOff(1000);
		break;
	}

	if (sThis[chan]->mOldExtensionCallback)
		(*sThis[chan]->mOldExtensionCallback)(chan, result);
}

void Controller::soundOnCallback(OSAlarm *alm, OSContext *)
{
	int chan = OSGetAlarmUserDataAny(int, alm);

	sThis[chan]->soundOn();
}

Controller::Controller(int chan, RemoteSpk *spk)
{
	mHBController.chan		= chan;
	mHBController.rumble	= false;
	mHBController.spVol		= 1.0f;

	remotespk				= spk;
	mOldConnectCallback		= nullptr;
	mOldExtensionCallback	= nullptr;
	mCallbackFlag			= false;
	mSoundOffFlag			= false;
	mRumbleFlag				= true;

	if (chan < WPAD_MAX_CONTROLLERS)
	{
		sBatteryFlag[chan]	= false;
		OSCreateAlarm(&sAlarm[chan]);
		OSCreateAlarm(&sAlarmSoundOff[chan]);
		sThis[chan]			= this;
		sSetInfoAsync[chan]	= false;
	}
}

Controller::~Controller()
{
	OSCancelAlarm(&sAlarm[mHBController.chan]);
	OSCancelAlarm(&sAlarmSoundOff[mHBController.chan]);

	sThis[mHBController.chan] = nullptr;
}

void Controller::initCallback()
{
	BOOL flag = WPADIsUsedCallbackByKPAD();
	WPADDeviceType type;

	WPADSetCallbackByKPAD(false);
	mOldConnectCallback =
		WPADSetConnectCallback(mHBController.chan, &wpadConnectCallback);
	WPADSetCallbackByKPAD(flag);

	mOldExtensionCallback =
		WPADSetExtensionCallback(mHBController.chan, &wpadExtensionCallback);

	mRumbleFlag = true;

	switch (WPADProbe(mHBController.chan, &type))
	{
	case WPAD_ESUCCESS:
		mCallbackFlag = true;
		break;

	case WPAD_ENODEV:
		mCallbackFlag = false;
		break;
	}
}

void Controller::clearCallback()
{
	WPADControlSpeaker(mHBController.chan, WPAD_SPEAKER_ENABLE, nullptr);

	WPADSetCallbackByKPAD(false);
	WPADSetConnectCallback(mHBController.chan, mOldConnectCallback);
	WPADSetCallbackByKPAD(true);

	WPADSetExtensionCallback(mHBController.chan, mOldExtensionCallback);
}

void Controller::setKpad(HBMKPadData const *con, bool updatePos)
{
/* NOTE: This is not the same as the one in HomeButton::update; the variables
 * are checked in the opposite order
 */
#define IsValidDevType_(x)									\
	(((x)->kpad->dev_type == WPAD_DEV_CLASSIC				\
	 && (x)->use_devtype == WPAD_DEV_CLASSIC)				\
	 || ((x)->kpad->dev_type == WPAD_DEV_MPLS_PT_CLASSIC	\
	     && (x)->use_devtype == WPAD_DEV_MPLS_PT_CLASSIC))

	if (!con->kpad)
		return;

	if (updatePos)
	{
		if (IsValidDevType_(con))
		{
			mHBController.x = con->pos.x;
			mHBController.y = con->pos.y;
		}
		else
		{
			mHBController.x = con->kpad->pos.x;
			mHBController.y = con->kpad->pos.y;
		}
	}

	mHBController.trig = con->kpad->trig;
	mHBController.hold = con->kpad->hold;
	mHBController.release = con->kpad->release;

	if (IsValidDevType_(con))
	{
		u32 h = con->kpad->ex_status.cl.hold;
		u32 t = con->kpad->ex_status.cl.trig;
		u32 r = con->kpad->ex_status.cl.release;

#define PropagateButtonPress_(button_)							\
	do															\
	{															\
		if (h & WPAD_BUTTON_CL_ ## button_)						\
			mHBController.hold |= WPAD_BUTTON_ ## button_;		\
																\
		if (t & WPAD_BUTTON_CL_ ## button_)						\
			mHBController.trig |= WPAD_BUTTON_ ## button_;		\
																\
		if (r & WPAD_BUTTON_CL_ ## button_)						\
			mHBController.release |= WPAD_BUTTON_ ## button_;	\
	} while (false)

		PropagateButtonPress_(A);
		PropagateButtonPress_(PLUS);
		PropagateButtonPress_(MINUS);
		PropagateButtonPress_(HOME);

#undef PropagateButtonPress_
	}
#undef IsValidDevType_
}

void Controller::clrKpadButton()
{
	mHBController.trig = 0;
	mHBController.hold = 0;
	mHBController.release = 0;
}

void Controller::setInValidPos()
{
	static f32 const invalidPosX = -1e4f;
	static f32 const invalidPosY = -1.5e4f;

	mHBController.x	= invalidPosX;
	mHBController.y	= invalidPosY;
}

int Controller::getChan() const
{
	return mHBController.chan;
}

void Controller::connect()
{
	getRemoteSpk()->Connect(getChan());
}

void Controller::disconnect()
{
	/* ... */
}

void Controller::setSpeakerVol(f32 vol)
{
	mHBController.spVol = vol;
}

f32 Controller::getSpeakerVol() const
{
	return mHBController.spVol;
}

void Controller::playSound(int id)
{
	if (!mSoundOffFlag)
	{
		getRemoteSpk()->Play(getChan(), id, getSpeakerVol() * 10.0f);

		if (WPADIsSpeakerEnabled(getChan()))
		{
			if (!mCheckSoundTimeFlag)
				mPlaySoundTime = OSGetTime();

			mCheckSoundTimeFlag = true;
			mCheckSoundIntervalFlag = false;
		}
	}
}

bool Controller::isPlayingSound() const
{
	return getRemoteSpk()->isPlaying(getChan());
}

bool Controller::isPlayingSoundId(int id) const
{
	if (!isPlayingSound())
		return false;

	if (!getRemoteSpk()->isPlayingId(getChan(), id))
		return false;

	return true;
}

void Controller::initSound()
{
	mCheckSoundTimeFlag = false;
	mCheckSoundIntervalFlag = false;
}

void Controller::updateSound()
{
	int chan = getChan();

	if (!isPlayingSound())
	{
		if (mCheckSoundTimeFlag)
		{
			if (!mCheckSoundIntervalFlag)
			{
				mStopSoundTime = OSGetTime();
				mCheckSoundIntervalFlag = true;
			}
			else
			{
				OSTime time = OSGetTime();
				if (OSTicksToMilliseconds(OSDiffTick(time, mStopSoundTime))
				    >= 1000)
				{
					mCheckSoundTimeFlag = false;
					mCheckSoundIntervalFlag = false;
				}
			}
		}

		return;
	}

	if (mCheckSoundTimeFlag)
	{
		mCheckSoundIntervalFlag = false;

		OSTime time = OSGetTime();
		if (OSTicksToMilliseconds(OSDiffTick(time, mPlaySoundTime))
		    >= 480000)
		{
			mCheckSoundTimeFlag = false;
			mCheckSoundIntervalFlag = false;
			soundOff(1000);
			return;
		}
	}

	// Average radio sensitivity is 80 (see __wpadCalcRadioQuality)
	if (!mSoundOffFlag && WPADGetRadioSensitivity(chan) <= 85)
		soundOff(1000);
}

void Controller::soundOff(int msec)
{
	int chan = getChan();

	if (!WPADIsSpeakerEnabled(chan))
		return;

	WPADControlSpeaker(chan, WPAD_SPEAKER_MUTE, nullptr);

	OSSetAlarmUserDataAny(&sAlarmSoundOff[chan], chan);
	OSCancelAlarm(&sAlarmSoundOff[chan]);
	OSSetAlarm(&sAlarmSoundOff[chan], OSMillisecondsToTicks(msec),
	           &soundOnCallback);

	mSoundOffFlag = true;
}

void Controller::soundOn()
{
	int chan = getChan();

	if (WPADIsSpeakerEnabled(chan))
		WPADControlSpeaker(chan, WPAD_SPEAKER_UNMUTE, nullptr);

	mSoundOffFlag = false;
}

bool Controller::isPlayReady() const
{
	return getRemoteSpk()->isPlayReady(getChan());
}

Controller::HBController *Controller::getController()
{
	return &mHBController;
}

void Controller::startMotor()
{
	if (getChan() < WPAD_MAX_CONTROLLERS && mRumbleFlag && !isPlayingSound())
	{
		setRumble();
		WPADControlMotor(getChan(), WPAD_MOTOR_RUMBLE);
	}
}

void Controller::stopMotor()
{
	if (getChan() < WPAD_MAX_CONTROLLERS && isRumbling())
	{
		clrRumble();
		WPADControlMotor(getChan(), WPAD_MOTOR_STOP);
	}
}

WPADResult Controller::getInfoAsync(WPADInfo *info)
{
	if (getChan() >= WPAD_MAX_CONTROLLERS)
		return WPAD_EBUSY;

	if (isPlayingSound() || isRumbling())
		return WPAD_EBUSY;

	if (getChan() < WPAD_MAX_CONTROLLERS) // it already is?
		sSetInfoAsync[getChan()] = true;

	return WPADGetInfoAsync(getChan(), info, &ControllerCallback);
}

void Controller::ControllerCallback(WPADChannel chan, WPADResult result)
{
	if (result == WPAD_ESUCCESS && chan < WPAD_MAX_CONTROLLERS)
		sBatteryFlag[chan] = true;

	if (chan < WPAD_MAX_CONTROLLERS)
		sSetInfoAsync[chan] = false;
}

bool Controller::getBatteryFlag() const
{
	if (getChan() >= WPAD_MAX_CONTROLLERS)
		return false;

	return sBatteryFlag[getChan()];
}

void Controller::clrBatteryFlag()
{
	if (getChan() >= WPAD_MAX_CONTROLLERS)
		return;

	sBatteryFlag[getChan()] = false;
}

} // namespace homebutton
