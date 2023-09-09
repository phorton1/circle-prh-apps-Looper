
#ifndef _uiWindow_h_
#define _uiWindow_h_

#include <ws/ws.h>
#include "commonDefines.h"


#define ID_WIN_LOOPER    	100
#define NUM_TRACK_BUTTONS   4

extern void sendSerialMidiCC(int cc_num, int value);

class uiWindow : public wsTopLevelWindow
{
	public:

		uiWindow(wsApplication *pApp, u16 id, s32 xs, s32 ys, s32 xe, s32 ye);

		void enableEraseButton(int i, bool enable);


	private:

		virtual void updateFrame();
		virtual u32 handleEvent(wsEvent *event);

		int send_state;
		bool last_dub_mode;
		u16  stop_button_cmd;
		int control_vol[LOOPER_NUM_CONTROLS];
		int clip_vol[LOOPER_NUM_TRACKS][LOOPER_NUM_LAYERS];
		bool clip_mute[LOOPER_NUM_TRACKS][LOOPER_NUM_LAYERS];

		wsButton *pStopButton;
		wsButton *pDubButton;
		wsButton *pTrackButtons[NUM_TRACK_BUTTONS];
		wsButton *pEraseButtons[NUM_TRACK_BUTTONS];

		static void staticSerialReceiveIRQHandler(void *pThis, unsigned char c);
		void serialReceiveIRQHandler(unsigned char c);
		void handleSerialCC(u8 cc_num, u8 value);
		void sendState();

		int serial_midi_len;
		unsigned char serial_midi_buf[4];

};

#endif // !_uiWindow_h_
