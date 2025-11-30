#ifndef RVL_SDK_HBM_REMOTE_SPK_H
#define RVL_SDK_HBM_REMOTE_SPK_H

/*******************************************************************************
 * headers
 */
 
#include <revolution/types.h>

// #include <revolution/arc.h>
// #include <revolution/os/OSAlarm.h>
#include <revolution/wenc.h>
#include <revolution/wpad.h>

#include <context_rvl.h>

/*******************************************************************************
 * classes and functions
 */

namespace homebutton
{
	// [SC5PGN]/build/libs/Debug/slamWiiD.a:HBMRemoteSpk.o(1)::.debug_info::0x189 [original object]
	class RemoteSpk
	{
	// nested types
	private:
		// [SC5PGN]/build/libs/Debug/slamWiiD.a:HBMRemoteSpk.o(1)::.debug_info::0x1f7 [original object]
		struct ChanInfo
		{
			OSAlarm			alarm;			// size 0x30, offset 0x00
			WENCInfo		wencinfo;		// size 0x20, offset 0x30
			WENCPCMSample	const *in_pcm;	// size 0x04, offset 0x50
			int				length;			// size 0x04, offset 0x54
			int				seId;			// size 0x04, offset 0x58
			bool			first;			// size 0x01, offset 0x5c
			s8				vol;			// size 0x01, offset 0x5d
			s8				cannotSendCnt;	// size 0x01, offset 0x5e
			/* 1 byte padding */
			byte2_t			pad; // lol pretty sure this wastes 8 bytes of space
			bool			playReady;		// size 0x01, offset 0x62
			/* 5 bytes padding */
		}; // size 0x68

	// methods
	public:
		// cdtors
		RemoteSpk(void *spkSeBuf);
		/* virtual ~RemoteSpk(); */ // virtual function ordering

		// methods
		bool isPlayReady(WPADChannel chan) const;
		bool isPlaying(WPADChannel chan) const;
		bool isPlayingId(WPADChannel chan, int seId) const;

		void GetPCMFromSeID(int in_ID, WENCPCMSample *&out_wave,
		                    int &out_length);
		void ClearPcm();
		void Start();
		void Stop();
		void Connect(WPADChannel chan);
		void Disconnect(WPADChannel chan);
		void Play(WPADChannel chan, int seID, s8 vol);

		// static methods
		static void SetInstance(RemoteSpk *pThis);
		static RemoteSpk *GetInstance();

	private:
		static void UpdateSpeaker(OSAlarm *alarm, OSContext *context);

		static void SpeakerOnCallback(WPADChannel chan, WPADResult result);
		static void DelaySpeakerOnCallback(OSAlarm *alarm, OSContext *context);

		static void SpeakerOffCallback(WPADChannel chan, WPADResult result);
		static void DelaySpeakerOffCallback(OSAlarm *alarm, OSContext *context);

		static void SpeakerPlayCallback(WPADChannel chan, WPADResult result);
		static void DelaySpeakerPlayCallback(OSAlarm *alarm, OSContext *context);

	// static members
	private:
		static RemoteSpk *spInstance;

	// members
	private:
		ChanInfo	info[WPAD_MAX_CONTROLLERS];	// size 0x1a0, offset 0x000
		OSAlarm		speakerAlarm;				// size 0x030, offset 0x1a0
		ARCHandle	handle;						// size 0x01c, offset 0x1d0
		bool		available;					// size 0x001, offset 0x1ec
		/* 3 bytes padding */
		/* vtable */							// size 0x004, offset 0x1f0
		/* 4 bytes padding */

	// late declarations
	public:
		virtual ~RemoteSpk();
	}; // size 0x1f8
} // namespace homebutton

#endif // RVL_SDK_HBM_REMOTE_SPK_H
