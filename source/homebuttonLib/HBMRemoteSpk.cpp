#include "HBMRemoteSpk.h"

/*******************************************************************************
 * headers
 */

#include <macros.h>

#include <revolution/types.h>

#include "nw4hbm/ut/Font.h" // IWYU pragma: keep (text)

// #include <revolution/arc.h>
// #include <revolution/os/OSAlarm.h>
// #include <revolution/os/OSInterrupt.h>
// #include <revolution/os/OSTime.h>
#include <revolution/wenc.h>
#include <revolution/wpad.h>

#include <context_rvl.h>

/*******************************************************************************
 * local function declarations
 */

namespace homebutton
{
	static bool MakeVolumeData(WENCPCMSample const *src, WENCPCMSample *dst,
	                           int vol, u32 size);
} // namespace homebutton

/*******************************************************************************
 * variables
 */

namespace homebutton
{
	// .bss
	RemoteSpk *RemoteSpk::spInstance;
} // namespace homebutton

/*******************************************************************************
 * functions
 */

namespace homebutton {

void RemoteSpk::SetInstance(RemoteSpk *pThis)
{
	spInstance = pThis;
}

RemoteSpk *RemoteSpk::GetInstance()
{
	return spInstance;
}

void RemoteSpk::GetPCMFromSeID(int in_ID, WENCPCMSample *&out_wave,
                               int &out_length)
{
	ARCFileInfo af;
	ARCFastOpen(&handle, in_ID, &af);

	out_wave = static_cast<WENCPCMSample *>(ARCGetStartAddrInMem(&af));
	out_length = static_cast<int>(ARCGetLength(&af));

	ARCClose(&af);
}

static bool MakeVolumeData(WENCPCMSample const *src, WENCPCMSample *dst,
                           int vol, u32 size)
{
	u32 enc_size = MIN_EQ(size, 40);
	for (int i = 0; i < enc_size; ++i)
		*dst++ = static_cast<WENCPCMSample>(*src++ * vol / 10);

	if (size > 40)
		return false;

	u32 zero_size = 40 - size;
	for (int i = 0; i < zero_size; ++i)
		*dst++ = 0;

	return true;
}

void RemoteSpk::UpdateSpeaker(OSAlarm *, OSContext *)
{
	WENCPCMSample pcmBuffer[40];
	WENCADPCMSample adpcmBuffer[20];

	if (!GetInstance())
		return;

	ChanInfo *pinfo = GetInstance()->info;

	/* explicitly post-increment */
	for (int i = 0; i < WPAD_MAX_CONTROLLERS; i++, pinfo++)
	{
		if (pinfo->in_pcm && WPADIsSpeakerEnabled(i))
		{
			BOOL intrStatus = OSDisableInterrupts(); /* int intr; */

			if (WPADCanSendStreamData(i))
			{
				MakeVolumeData(pinfo->in_pcm, pcmBuffer, pinfo->vol,
				               pinfo->length / sizeof(WENCPCMSample));
				WENCGetEncodeData(&pinfo->wencinfo, pinfo->first ? 0 : 1,
				                  pcmBuffer, ARRAY_LENGTH(pcmBuffer),
				                  adpcmBuffer);
				WPADSendStreamData(i, adpcmBuffer, ARRAY_LENGTH(adpcmBuffer));

				pinfo->first = false;
				pinfo->cannotSendCnt = 0;
				pinfo->in_pcm += ARRAY_LENGTH(pcmBuffer);
				pinfo->length -= sizeof pcmBuffer;

				if (pinfo->length <= 0)
				{
					pinfo->seId = -1;
					pinfo->in_pcm = nullptr;
				}
			}
			else
			{
				++pinfo->cannotSendCnt;

				if (pinfo->cannotSendCnt > 60)
					pinfo->in_pcm = nullptr;
			}

			OSRestoreInterrupts(intrStatus);
		}
	}
}

void RemoteSpk::ClearPcm()
{
	for (int i = 0; i < WPAD_MAX_CONTROLLERS; ++i)
	{
		info[i].in_pcm = nullptr;
		info[i].seId = -1;
	}
}

RemoteSpk::RemoteSpk(void *spkSeBuf)
{
	SetInstance(this);

	if (spkSeBuf)
		available = ARCInitHandle(spkSeBuf, &handle);
	else
		available = false;

	OSCreateAlarm(&speakerAlarm);

	for (int i = 0; i < WPAD_MAX_CONTROLLERS; ++i)
	{
		OSCreateAlarm(&info[i].alarm);
		info[i].in_pcm = nullptr;
		info[i].seId = -1;
		info[i].first = true;
		info[i].playReady = true;
	}
}

RemoteSpk::~RemoteSpk()
{
	SetInstance(nullptr);
	available = false;

	// manual inline of Stop

	OSCancelAlarm(&speakerAlarm);

	for (int i = 0; i < WPAD_MAX_CONTROLLERS; ++i)
		OSCancelAlarm(&info[i].alarm);
}

void RemoteSpk::Start()
{
	if (!available)
		return;

	OSCreateAlarm(&speakerAlarm);
	OSSetPeriodicAlarm(&speakerAlarm, OSGetTime(),
	                   OSNanosecondsToTicks(6666667), &UpdateSpeaker);

	for (int i = 0; i < WPAD_MAX_CONTROLLERS; ++i)
	{
		OSCreateAlarm(&info[i].alarm);

		info[i].in_pcm = nullptr;
		info[i].seId = -1;
		info[i].first = true;
		info[i].playReady = true;
	}
}

void RemoteSpk::Stop()
{
	OSCancelAlarm(&speakerAlarm);

	for (int i = 0; i < WPAD_MAX_CONTROLLERS; ++i)
		OSCancelAlarm(&info[i].alarm);
}

void RemoteSpk::DelaySpeakerOnCallback(OSAlarm *alarm, OSContext *)
{
	WPADChannel chan = OSGetAlarmUserDataAny(WPADChannel, alarm);
	WPADResult result ATTR_UNUSED =
		WPADControlSpeaker(chan, WPAD_SPEAKER_ENABLE, &SpeakerOnCallback);
}

void RemoteSpk::SpeakerOnCallback(WPADChannel chan, WPADResult result)
{
	RemoteSpk *pRmtSpk = GetInstance();
	if (!pRmtSpk)
		return;

	switch (result)
	{
	case WPAD_ESUCCESS:
		pRmtSpk->info[chan].first = true;
		result =
			WPADControlSpeaker(chan, WPAD_SPEAKER_PLAY, &SpeakerPlayCallback);
		break;

	case WPAD_ETRANSFER:
		pRmtSpk->info[chan].playReady = false;
		break;

	case WPAD_EBUSY:
		OSSetAlarmUserDataAny(&pRmtSpk->info[chan].alarm, chan);
		OSCancelAlarm(&pRmtSpk->info[chan].alarm);
		OSSetAlarm(&pRmtSpk->info[chan].alarm, OSMillisecondsToTicks(50),
		           &DelaySpeakerOnCallback);
		break;
	}
}

/* New! */
void RemoteSpk::DelaySpeakerOffCallback(OSAlarm *alarm, OSContext *)
{
	WPADChannel chan = OSGetAlarmUserDataAny(WPADChannel, alarm);
	WPADResult result ATTR_UNUSED =
		WPADControlSpeaker(chan, WPAD_SPEAKER_DISABLE, &SpeakerOffCallback);
}

/* New! */
void RemoteSpk::SpeakerOffCallback(WPADChannel chan, WPADResult result)
{
	RemoteSpk *pRmtSpk = GetInstance();
	if (!pRmtSpk)
		return;

	switch (result)
	{
	case WPAD_ESUCCESS:
	case WPAD_ETRANSFER:
		pRmtSpk->info[chan].playReady = false;
		break;

	case WPAD_EBUSY:
		OSSetAlarmUserDataAny(&pRmtSpk->info[chan].alarm, chan);
		OSCancelAlarm(&pRmtSpk->info[chan].alarm);
		OSSetAlarm(&pRmtSpk->info[chan].alarm, OSMillisecondsToTicks(50),
		           &DelaySpeakerOffCallback);
		break;
	}
}


void RemoteSpk::DelaySpeakerPlayCallback(OSAlarm *alarm, OSContext *)
{
	WPADChannel chan = OSGetAlarmUserDataAny(WPADChannel, alarm);

	WPADResult result ATTR_UNUSED =
		WPADControlSpeaker(chan, WPAD_SPEAKER_PLAY, &SpeakerPlayCallback);
}

void RemoteSpk::SpeakerPlayCallback(WPADChannel chan, WPADResult result)
{
	RemoteSpk *pRmtSpk = GetInstance();
	if (!pRmtSpk)
		return;

	switch (result)
	{
	case WPAD_ESUCCESS:
		pRmtSpk->info[chan].playReady = true;
		break;

	case WPAD_ETRANSFER:
		pRmtSpk->info[chan].playReady = false;
		break;

	case WPAD_ENODEV:
		pRmtSpk->info[chan].playReady = false;
		break;

	case WPAD_EBUSY:
		OSSetAlarmUserDataAny(&pRmtSpk->info[chan].alarm, chan);
		OSCancelAlarm(&pRmtSpk->info[chan].alarm);
		OSSetAlarm(&pRmtSpk->info[chan].alarm, OSMillisecondsToTicks(50),
		           &DelaySpeakerPlayCallback);
		break;
	}
}

void RemoteSpk::Connect(WPADChannel chan)
{
	if (!available)
		return;

	// NOTE: DWARF says this is an int and not a WPADResult, which is a long
	int result ATTR_UNUSED =
		WPADControlSpeaker(chan, WPAD_SPEAKER_ENABLE, &SpeakerOnCallback);

	byte4_t *p = reinterpret_cast<byte4_t *>(info[chan].wencinfo.data);

	/* explicitly post-increment */
	for (int i = sizeof info[chan].wencinfo.data / sizeof(byte4_t); i > 0;
	     i--, p++)
	{
		*p = 0;
	}

	info[chan].first = true;
	info[chan].playReady = false;
}

/* New! */
void RemoteSpk::Disconnect(WPADChannel chan)
{
	if (!available)
		return;

	// NOTE: DWARF says this is an int and not a WPADResult, which is a long
	int result ATTR_UNUSED =
		WPADControlSpeaker(chan, WPAD_SPEAKER_DISABLE, &SpeakerOffCallback);

	info[chan].playReady = false;
}

void RemoteSpk::Play(WPADChannel chan, int seID, s8 vol)
{
	if (!available)
		return;

	WENCPCMSample *pcm;
	int length;
	GetPCMFromSeID(seID, pcm, length);

	info[chan].cannotSendCnt = 0;
	info[chan].seId = seID;
	info[chan].length = length;
	info[chan].vol = vol;
	info[chan].in_pcm = pcm;
}

bool RemoteSpk::isPlaying(WPADChannel chan) const
{
	return info[chan].in_pcm != nullptr;
}

bool RemoteSpk::isPlayingId(WPADChannel chan, int seId) const
{
	if (isPlaying(chan) && info[chan].seId == seId)
		return true;
	else
		return false;
}

bool RemoteSpk::isPlayReady(WPADChannel chan) const
{
	return info[chan].playReady != false;
}

} // namespace homebutton
