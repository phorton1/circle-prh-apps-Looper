
#include "uiWindow.h"
#include <circle/logger.h>
#include <utils/myUtils.h>
#include "Looper.h"
#include "uiStatusBar.h"
#include "uiTrack.h"
#include "vuSlider.h"
#include <system/std_kernel.h>
	// to get to serial port


#define log_name  "loopwin"

#define USE_SERIAL_INTERRUPTS 1
	// There was a lag when polling the serial port, so I (finally)
	// went into circle and implemented a decent serial port interrupt
	// API.  If this is set to 1, the interrupt handler is used, if
	// not, polling is used.
	//
	// See note about using LOOPER_LOG() in loopMachine.cpp to
	// prevent "noise" when turning this on.


#define ID_LOOP_STATUS_BAR     		101
#define ID_VU1                		201
#define ID_VU2                		202
#define ID_VU3                		203
#define ID_VU4                		204
#define ID_VU_SLIDER           		250
#define ID_TRACK_CONTROL_BASE  		300
#define ID_LOOP_STOP_BUTTON         401
#define ID_LOOP_DUB_BUTTON    		402
#define ID_LOOP_TRACK_BUTTON_BASE   410
#define ID_ERASE_TRACK_BUTTON_BASE  430

// The official 7" rPI Screen is 800 x 400 == 2:1
// The small screen is 480 x 320 = 1.5:1
// Old VU meters: top  144x26, right:32x180, note
// that VU meters are divisible by 12


//-------------------
// Y Layout
//-------------------
// Screen is split into four vertical areas

#define TOP_MARGIN					2		// stuff is really scrunched at the top
#define TOP_HEIGHT					26		// height of top horiz VU meters and status bar

#define TOP_BUTTON_MARGIN			5		// from vu meters to ERASE buttons and VU labels
#define TOP_BUTTON_HEIGHT			24		// height of erase buttons and VU labels

#define TRACK_TOP_MARGIN			5		// from erase buttons to tracks / vu meters
#define TRACK_BOTTOM_MARGIN			5		// from track bottoms to track buttons

#define BUTTON_HEIGHT				34		// height of bottom buttons
#define BOTTOM_MARGIN				5		// space at bottom of screen

// Y position of objects

#define TOP_VU_SY					(TOP_MARGIN)
#define TOP_VU_EY					(TOP_VU_SY + TOP_HEIGHT - 1)

#define STATUS_SY					(TOP_VU_SY)
#define STATUS_EY					(TOP_VU_EY)

#define TOP_BUTTON_SY				(TOP_VU_EY + TOP_BUTTON_MARGIN)
#define TOP_BUTTON_EY				(TOP_BUTTON_SY + TOP_BUTTON_HEIGHT - 1)

#define TRACK_SY					(TOP_BUTTON_EY + TRACK_TOP_MARGIN)
#define TRACK_EY					(BUTTON_SY - TRACK_BOTTOM_MARGIN)

#define VU_SY						(TRACK_SY)
#define VU_EY						(TRACK_EY)
	// VU meters need to be a multiple of 12!
	// should correct this in the ctor

#define BUTTON_SY					(height - BOTTOM_MARGIN - BUTTON_HEIGHT - 1)
#define BUTTON_EY					(BUTTON_SY + BUTTON_HEIGHT - 1)


//-------------------
// X layout
//-------------------
// Screen is split into LEFT/RIGHT track and vu areas
// based on fixed margins and the VU widths

#define TOP_VU_WIDTH				144			// width of top horiz VU meters; status bar takes space left over
#define TOP_LEFT_MARGIN				5
#define TOP_RIGHT_MARGIN			5
#define STATUS_MARGIN				5

#define TOP_LEFT_VU_SX		(TOP_LEFT_MARGIN)							// start x for left top VU meter
#define TOP_LEFT_VU_EX		(TOP_LEFT_VU_SX + TOP_VU_WIDTH - 1)			// end x for top left VU meter
#define TOP_RIGHT_VU_SX		(width-TOP_VU_WIDTH-TOP_RIGHT_MARGIN - 1)	// start x for right top VU meter
#define TOP_RIGHT_VU_EX		(TOP_RIGHT_VU_SX + TOP_VU_WIDTH - 1)		// end x for top right VU meter

// The top status bar has the same height as the VU meters
// and fits between them

#define STATUS_SX		(TOP_LEFT_VU_EX + STATUS_MARGIN)
#define STATUS_EX		(TOP_RIGHT_VU_SX - STATUS_MARGIN )

// RIGHT SIDE

#define VU_WIDTH					30
#define VU_RIGHT_MARGIN				10			// right vu meters
#define VU_INTER_MARGIN				8			// and to left and right for labels

#define VU_RIGHT_SX					(width - VU_RIGHT_MARGIN - VU_WIDTH - 1)
#define VU_MIDDLE_SX				(VU_RIGHT_SX - VU_WIDTH - VU_INTER_MARGIN)
#define VU_LEFT_SX					(VU_MIDDLE_SX - VU_WIDTH - VU_INTER_MARGIN)
#define VU_LEFT_EX					(VU_LEFT_SX + VU_WIDTH - 1)
#define VU_MIDDLE_EX				(VU_MIDDLE_SX + VU_WIDTH - 1)
#define VU_RIGHT_EX					(VU_RIGHT_SX + VU_WIDTH - 1)

#define VU_LABEL1_SX				(VU_LEFT_SX - 2)
#define VU_LABEL1_EX				(VU_LEFT_EX + 2)
#define VU_LABEL2_SX				(VU_MIDDLE_SX - 2)
#define VU_LABEL2_EX				(VU_MIDDLE_EX + 2)
#define VU_LABEL3_SX				(VU_RIGHT_SX - 2)
#define VU_LABEL3_EX				(VU_RIGHT_EX + 2)


#define TRACK_LEFT_MARGIN			5
#define TRACK_RIGHT_MARGIN			8
#define TRACK_INTER_MARGIN			5

#define TRACK_AREA_SX				(TRACK_LEFT_MARGIN)
#define TRACK_AREA_EX				(VU_LEFT_SX - TRACK_RIGHT_MARGIN)
#define TRACK_AREA_WIDTH			(TRACK_AREA_EX - TRACK_AREA_SX + 1)
#define TRACK_STEP					(TRACK_AREA_WIDTH/LOOPER_NUM_TRACKS)
#define TRACK_WIDTH					((TRACK_AREA_WIDTH - TRACK_INTER_MARGIN * (LOOPER_NUM_TRACKS-1))/LOOPER_NUM_TRACKS)

// the stop and dub buttons split the area under the right vu meters
// with a 5 pixel margin to the tracks and to the right

#define BOTTOM_BUTTON_MARGIN		5

#define STOP_BUTTON_SX				(TRACK_AREA_EX + 2)

#define OTHER_BUTTON_AREA_WIDTH		(width - STOP_BUTTON_SX - 1)
#define OTHER_BUTTON_WIDTH 			((OTHER_BUTTON_AREA_WIDTH - BOTTOM_BUTTON_MARGIN)/2)

// compensate to make stop button a little bigger on small screen

#define STOP_BUTTON_EX				(STOP_BUTTON_SX + OTHER_BUTTON_WIDTH - 1     + 5)
#define DUB_BUTTON_SX				(STOP_BUTTON_EX + BOTTOM_BUTTON_MARGIN)
#define DUB_BUTTON_EX				(DUB_BUTTON_SX + OTHER_BUTTON_WIDTH - 1      - 5)




// old
//
//	#define TOP_MARGIN 			72		// allows for top VU meters and erase buttons
//	#define BOTTOM_MARGIN  		50		// allows for Track buttons and Dub button
//	#define RIGHT_MARGIN    	150		// allows for vertical VU meters
//
//	#define VU_TOP  	TOP_MARGIN+46	// allows for a title above each vertical VU meter
//	#define VU_BOTTOM   VU_TOP + 180	// fixed height VU meters
//
//	#define TRACK_VSPACE  	5
//	#define TRACK_HSPACE  	5
//	#define BUTTON_HEIGHT   35


CSerialDevice *s_pSerial = 0;


// extern
void sendSerialMidiCC(int cc_num, int value)
{
	if (s_pSerial)
	{
		unsigned char midi_buf[4];
		midi_buf[0] = 0x0b;
		midi_buf[1] = 0xb0;
		midi_buf[2] = cc_num;
		midi_buf[3] = value;
		s_pSerial->Write((unsigned char *) midi_buf,4);
	}
}


//----------------------------------------------------------------

uiWindow::uiWindow(wsApplication *pApp, u16 id, s32 xs, s32 ys, s32 xe, s32 ye) :
	wsTopLevelWindow(pApp,id,xs,ys,xe,ye)
{
	LOG("uiWindow ctor(%d,%d,%d,%d)",xs,ys,xe,ye);

	// Init Members

	send_state = 1;
	last_dub_mode = 0;
	stop_button_cmd = 0;
	serial_midi_len = 0;
	for (int i=0; i<LOOPER_NUM_CONTROLS; i++)
	{
		control_vol[i] = 0;
	}
	for (int i=0; i<LOOPER_NUM_TRACKS; i++)
	{
		for (int j=0; j<LOOPER_NUM_LAYERS; j++)
		{
			clip_mute[i][j] = 0;
			clip_vol[i][j] = 0;
		}
	}


	// Init UI

	setBackColor(wsDARK_BLUE);

	int height = ye-ys+1;
    int width = xe-xs+1;
	// int right_col = width - RIGHT_MARGIN;

	new uiStatusBar(this,ID_LOOP_STATUS_BAR,
		STATUS_SX,
		STATUS_SY,
		STATUS_EX,
		STATUS_EY);
		// 165,0,width-165,TOP_MARGIN-1);
	LOG("status bar created",0);

	// TOP LEFT INPUT VU METER - CODEC audio input, period
	// mpd knobs, double numbers are inc_dec msb lsb's
	// ith increments of two
	//
	//   in:DEC2(0x0A)		  		out:DEC2(0z0B)
	//   unused:CC(0x0C)      		mix:DEC1(0x0D:0x00) scale=2
	//   thru:DEC1(0x0E,0x00)=2     loop:CC(0x0F)

	#if WITH_METERS
		new vuSlider(this, ID_VU2,
			TOP_LEFT_VU_SX,
			TOP_VU_SY,
			TOP_LEFT_VU_EX,
			TOP_VU_EY,
			// 6, 2, 150, 28,
			true, 12,
			METER_INPUT,
			LOOPER_CONTROL_INPUT_GAIN,
			-1,							// cable
			-1,							// 0 based channel
			MIDI_EVENT_TYPE_INC_DEC2,	// slow continuous
			0x0A);						// LSB
	#endif

	// TOP RIGHT OUTPUT VU METER - output gain, no monitor

	#if WITH_METERS
		new vuSlider(this,ID_VU2,
			TOP_RIGHT_VU_SX,
			TOP_VU_SY,
			TOP_RIGHT_VU_EX,
			TOP_VU_EY,
			// width-150, 2, width-6, 28,
			true, 12,
			-1,		 					// no meter
			LOOPER_CONTROL_OUTPUT_GAIN,
			-1,							// cable
			-1,							// 0 based channel
			MIDI_EVENT_TYPE_INC_DEC2,	// slow continuous
			0x0B);						// top right knob
	#endif

	// LEFT - THRU vu

	#if WITH_METERS
		wsStaticText *pt1 = new wsStaticText(this,0,"THRU",
			// right_col+6,TOP_MARGIN+23,right_col+42,TOP_MARGIN+39
			VU_LABEL1_SX,
			TOP_BUTTON_SY,
			VU_LABEL1_EX,
			TOP_BUTTON_EY );
		pt1->setAlign(ALIGN_BOTTOM_CENTER);
		pt1->setForeColor(wsWHITE);
		pt1->setFont(wsFont7x12);

		new vuSlider(this,ID_VU2,
			VU_LEFT_SX,
			VU_SY,
			VU_LEFT_EX,
			VU_EY,
			// right_col+8, VU_TOP, right_col+40, VU_BOTTOM,
			false, 12,
			METER_THRU,
			LOOPER_CONTROL_THRU_VOLUME,
			-1,		// cable
			-1,		// 0 based channel
			MIDI_EVENT_TYPE_INC_DEC1,	// faster continous
			0x0E,						// bottom left knob
			0x00);
	#endif

	// MIDDLE - LOOP vu

	#if WITH_METERS
		wsStaticText *pt2 = new wsStaticText(this,0,"LOOP",
			// right_col+50,TOP_MARGIN+23,right_col+86,TOP_MARGIN+39,
			VU_LABEL2_SX,
			TOP_BUTTON_SY,
			VU_LABEL2_EX,
			TOP_BUTTON_EY );
		pt2->setAlign(ALIGN_BOTTOM_CENTER);
		pt2->setForeColor(wsWHITE);
		pt2->setFont(wsFont7x12);

		new vuSlider(this,ID_VU3,
			VU_MIDDLE_SX,
			VU_SY,
			VU_MIDDLE_EX,
			VU_EY,
			// right_col+52, VU_TOP, right_col+84, VU_BOTTOM,
			false, 12,
			METER_LOOP,
			LOOPER_CONTROL_LOOP_VOLUME,
			-1,		// cable
			-1,		// 0 based channel
			MIDI_EVENT_TYPE_CC,			// jumpy CC for foot pedal
			0x0F);						// bottom right knob
	#endif

	// RIGHT - MIX vu

	#if WITH_METERS
		wsStaticText *pt3 = new wsStaticText(this,0,"MIX",
			// right_col+94,TOP_MARGIN+23,right_col+130,TOP_MARGIN+39
			VU_LABEL3_SX,
			TOP_BUTTON_SY,
			VU_LABEL3_EX,
			TOP_BUTTON_EY );
		pt3->setAlign(ALIGN_BOTTOM_CENTER);
		pt3->setForeColor(wsWHITE);
		pt3->setFont(wsFont7x12);

		new vuSlider(this,ID_VU4,
			VU_RIGHT_SX,
			VU_SY,
			VU_RIGHT_EX,
			VU_EY,
			// right_col+96, VU_TOP, right_col+128, VU_BOTTOM,
			false, 12,
			METER_MIX,
			LOOPER_CONTROL_MIX_VOLUME,
			-1,		// cable
			-1,		// 0 based channel
			MIDI_EVENT_TYPE_INC_DEC1,	// faster continous
			0x0D,						// right knob
			0x00);
	#endif

	// UI TRACKS
	// and TRACK buttons

	LOG("creating ui_tracks and track buttons",0);

	int start_x = TRACK_AREA_SX;
	// int btop = height-BOTTOM_MARGIN;
	// int cheight = height-TOP_MARGIN-BOTTOM_MARGIN-TRACK_VSPACE*2;
	// int cwidth = (width-RIGHT_MARGIN-1-TRACK_HSPACE*(LOOPER_NUM_TRACKS+1)) / LOOPER_NUM_TRACKS;
	// int step = TRACK_HSPACE;
	for (int i=0; i<LOOPER_NUM_TRACKS; i++)
	{
		LOG("creating ui_track(%d)",i);
		int end_x = start_x + TRACK_WIDTH - 1;
		
		new uiTrack(
			i,
			this,
			ID_TRACK_CONTROL_BASE + i,
			start_x, TRACK_SY, end_x, TRACK_EY);
			// step,
			// TOP_MARGIN + TRACK_VSPACE,
			// step + cwidth -1,
			// TOP_MARGIN + TRACK_VSPACE + cheight - 1);

		CString track_name;
		track_name.Format("%d",i+1);
		pTrackButtons[i] = new 	wsButton(
			this,
			ID_LOOP_TRACK_BUTTON_BASE + i,
			(const char *) track_name, // getLoopCommandName(LOOP_COMMAND_TRACK_BASE + i),
			start_x+2, BUTTON_SY, end_x-2, BUTTON_EY,
			// step+10,
			// btop+5,
			// step + cwidth - 10,
			// btop+5+BUTTON_HEIGHT-1,
			BTN_STYLE_USE_ALTERNATE_COLORS);
		pTrackButtons[i]->setFont(wsFont12x16);
		pTrackButtons[i]->setAltBackColor(wsSLATE_GRAY);

		pEraseButtons[i] = new wsButton(
			this,
			ID_ERASE_TRACK_BUTTON_BASE + i,
			"erase",
			start_x+2, TOP_BUTTON_SY, end_x-2, TOP_BUTTON_EY,
			// step+10,
			// 40,
			// step + cwidth - 10,
			// 40+BUTTON_HEIGHT-10-1,
			BTN_STYLE_USE_ALTERNATE_COLORS);
		pEraseButtons[i]->setFont(wsFont8x14);
		pEraseButtons[i]->setAltBackColor(wsSLATE_GRAY);
		pEraseButtons[i]->hide();

		start_x += TRACK_STEP;
		
		// step += cwidth + TRACK_HSPACE;
		LOG("finished creating ui_track(%d)",i);
	}

	// Stop and Dub Buttons

	pStopButton = new
		#if USE_MIDI_SYSTEM
			wsMidiButton(
		#else
			wsButton(
		#endif
			this,
			ID_LOOP_STOP_BUTTON,
			"blah",		// button does not start off with a function: getLoopCommandName(LOOP_COMMAND_STOP),
			STOP_BUTTON_SX,
			BUTTON_SY,
			STOP_BUTTON_EX,
			BUTTON_EY,
			// right_col + 3,
			// btop+5-55,
			// width - 12,
			// btop+5-55+BUTTON_HEIGHT+5-1,
			BTN_STYLE_USE_ALTERNATE_COLORS,
			WIN_STYLE_CLICK_LONG);
	pStopButton->setAltBackColor(wsSLATE_GRAY);
	pStopButton->setFont(wsFont12x16);
	pStopButton->hide();

	pDubButton = new
		#if USE_MIDI_SYSTEM
			wsMidiButton(
		#else
			wsButton(
		#endif
			this,
			ID_LOOP_DUB_BUTTON,
			getLoopCommandName(LOOP_COMMAND_DUB_MODE),
			DUB_BUTTON_SX,
			BUTTON_SY,
			DUB_BUTTON_EX,
			BUTTON_EY,
			// right_col + 3,
			// btop+5,
			// width - 12,
			// btop+5+BUTTON_HEIGHT-1,
			BTN_STYLE_USE_ALTERNATE_COLORS,
			WIN_STYLE_CLICK_LONG);
	pDubButton->setAltBackColor(wsSLATE_GRAY);
	pDubButton->setFont(wsFont12x16);

	// register handlers

	s_pSerial = CCoreTask::Get()->GetKernel()->GetSerial();

	#if USE_SERIAL_INTERRUPTS
		if (s_pSerial)
		{
			LOG("Registering serial interrupt handler",0);
			s_pSerial->RegisterReceiveIRQHandler(this,staticSerialReceiveIRQHandler);
		}
	#endif

	LOG("uiWindow ctor finished",0);

}	// uiWindow ctor


void uiWindow::enableEraseButton(int i, bool enable)
{
	if (enable)
		pEraseButtons[i]->show();
	else
		pEraseButtons[i]->show();
}


// virtual
void uiWindow::updateFrame()
{
	int notify_loop = pTheLooper->getPendingLoopNotify();
	if (notify_loop)
	{
		sendSerialMidiCC(NOTIFY_LOOP,notify_loop);
	}

	#if !USE_SERIAL_INTERRUPTS
		// polling approach

		// This code allows for 1..5 to be typed into the buttons
		// or for packets 0x0b 0xb0 NN 0x7f where NN is
		// 21, 22, 23, 31, and 25, for buttons one through 5
		// to be handled.

		if (s_pSerial)
		{
			u8 buf[4];
			int num_read = s_pSerial->Read(buf,4);
			if (num_read == 1)
			{
				// the 1 case is old, nearly obsolete, to control directly from windows machine
				u16 button_num = buf[0] - '1';
				// the 1 case is old, nearly obsolete, to control directly from windows machine
				if (button_num == 7)
					pTheLooper->command(LOOP_COMMAND_DUB_MODE);
				if (button_num == 6)
					pTheLooper->command(LOOP_COMMAND_CLEAR_ALL);
				else if (button_num == 5)
					pTheLooper->command(LOOP_COMMAND_STOP_IMMEDIATE);
				else if (button_num == 4)
					pTheLooper->command(LOOP_COMMAND_CLEAR_ALL);
				else if (button_num < 4)
					pTheLooper->command(LOOP_COMMAND_TRACK_BASE + button_num);
			}

			// handle encapsulated midi messages

			else if (num_read == 4)
			{
				if (buf[0] == 0x0b &&			// 0x0b == CC messages
					buf[1] == 0xb0)				// 0xb0 == CC messages on channel 1
				{
					handleSerialCC(buf[2],buf[3]);
				}

				// unknown midi serial messages

				else
				{
					LOG_WARNING("unknown serial midi command",0);
					display_bytes("unknown midi buffer",buf,4);
				}

				// weird number of bytes
			}
			else if (num_read)
			{
				LOG_WARNING("unexpected number of serial bytes: %d", num_read);
			}
		}
	#endif	// polling for serial midi


	#if 1
		logString_t *msg = pTheLooper->getNextLogString();
		if (msg)
		{
			CLogger::Get()->Write(msg->lname,LogNotice,*msg->string);
			delete msg->string;
			delete msg;
		}
	#endif

	// STOP button is blank if not running,
	// says STOP if running and pending command is not STOP
	// says STOP! if running AND pending command is STOP

	bool running = pTheLooper->getRunning();
	u16  pending = pTheLooper->getPendingCommand();
	u16 t_command = running ?
		pending == LOOP_COMMAND_STOP ?
			LOOP_COMMAND_STOP_IMMEDIATE :
			LOOP_COMMAND_STOP : 0;

	if (stop_button_cmd != t_command)
	{
		stop_button_cmd = t_command;
		pStopButton->setText(getLoopCommandName(t_command));
		sendSerialMidiCC(LOOP_STOP_CMD_STATE_CC,stop_button_cmd);

		if (!t_command)
			pStopButton->hide();
		else
			pStopButton->show();
	}

	// DUB Button changes based on looper dub mode,
	// and notifies the TE of any changes

	bool dub_mode = pTheLooper->getDubMode();
	if (dub_mode != last_dub_mode)
	{
		LOG("updateState dub mode=%d",dub_mode);
		last_dub_mode = dub_mode;
		int bc = dub_mode ? wsORANGE : defaultButtonReleasedColor;
		pDubButton->setBackColor(bc);
		pDubButton->setStateBits(WIN_STATE_DRAW);
		sendSerialMidiCC(LOOP_DUB_STATE_CC,dub_mode);
	}

	// Send initial state or check for midi state changes

	if (send_state)
	{
		sendState();
	}
	else
	{
		for (int i=0; i<LOOPER_NUM_CONTROLS; i++)
		{
			int vol = pTheLooper->getControlValue(i);
			if (control_vol[i] != vol)
			{
				control_vol[i] = vol;
				sendSerialMidiCC(LOOP_CONTROL_BASE_CC+i,vol);
			}
		}

		for (int i=0; i<LOOPER_NUM_TRACKS; i++)
		{
			publicTrack *pTrack = pTheLooper->getPublicTrack(i);
			for (int j=0; j<LOOPER_NUM_LAYERS; j++)
			{
				bool mute = pTrack->getPublicClip(j)->isMuted();
				if (clip_mute[i][j] != mute)
				{
					clip_mute[i][j] = mute;
					sendSerialMidiCC(CLIP_MUTE_BASE_CC + i*LOOPER_NUM_LAYERS + j,mute);
				}
				u8 vol = pTrack->getPublicClip(j)->getVolume();
				if (clip_vol[i][j] != vol)
				{
					clip_vol[i][j] = vol;
					sendSerialMidiCC(CLIP_VOL_BASE_CC + i*LOOPER_NUM_LAYERS + j,vol);
				}
			}
		}

	}	// !send_state

	wsWindow::updateFrame();
}




// virtual
u32 uiWindow::handleEvent(wsEvent *event)
{
	u32 result_handled = 0;
	u32 type = event->getEventType();
	u32 event_id = event->getEventID();
	u32 id = event->getID();
	// wsWindow *obj = event->getObject();
	LOG("uiWindow::handleEvent(%08x,%d,%d)",type,event_id,id);

	if (type == EVT_TYPE_WINDOW &&
	   event_id == EVENT_LONG_CLICK)
	{
		if (id == ID_LOOP_STOP_BUTTON || id == ID_LOOP_DUB_BUTTON)
			pTheLooper->command(LOOP_COMMAND_CLEAR_ALL);
	}
	else if (type == EVT_TYPE_BUTTON &&
		     event_id == EVENT_CLICK)
	{
		if (id == ID_LOOP_STOP_BUTTON)
		{
			if (stop_button_cmd)
				pTheLooper->command(stop_button_cmd);
		}
		else if (id == ID_LOOP_DUB_BUTTON)
		{
			pTheLooper->command(LOOP_COMMAND_DUB_MODE);
		}
		else if (id >= ID_LOOP_TRACK_BUTTON_BASE &&
				 id < ID_LOOP_TRACK_BUTTON_BASE + NUM_TRACK_BUTTONS)
		{
			int track_num = id - ID_LOOP_TRACK_BUTTON_BASE;
			pTheLooper->command(LOOP_COMMAND_TRACK_BASE + track_num);
		}
		else if (id >= ID_ERASE_TRACK_BUTTON_BASE &&
				 id < ID_ERASE_TRACK_BUTTON_BASE + NUM_TRACK_BUTTONS)
		{
			int track_num = id - ID_ERASE_TRACK_BUTTON_BASE;
			pTheLooper->command(LOOP_COMMAND_ERASE_TRACK_BASE + track_num);
		}
		result_handled = 1;
	}
	else if (type == EVT_TYPE_WINDOW &&
		     event_id == EVENT_CLICK &&
			 id >= ID_CLIP_BUTTON_BASE &&
			 id <= ID_CLIP_BUTTON_BASE + LOOPER_NUM_TRACKS*LOOPER_NUM_LAYERS)
	{
		int num = id - ID_CLIP_BUTTON_BASE;
		int track_num = num / LOOPER_NUM_LAYERS;
		int clip_num = num % LOOPER_NUM_LAYERS;

		publicTrack *pTrack = pTheLooper->getPublicTrack(track_num);
		publicClip  *pClip  = pTrack->getPublicClip(clip_num);
		bool mute = pClip->isMuted();

		LOG("setting clip(%d,%d) mute=%d",track_num,clip_num,!mute);
		pClip->setMute(!mute);
		result_handled = 1;
	}

	if (!result_handled)
		result_handled = wsTopLevelWindow::handleEvent(event);

	return result_handled;
}



//-------------------------------------------
// interrupt driven serial midi
//-------------------------------------------

// static
void uiWindow::staticSerialReceiveIRQHandler(void *pThis, unsigned char c)
{
	((uiWindow *)pThis)->serialReceiveIRQHandler(c);

}


void uiWindow::serialReceiveIRQHandler(unsigned char c)
{
	if (!serial_midi_len)
	{
		if (c == 0x0b)
		{
			serial_midi_buf[serial_midi_len++] = c;		// start serial midi message
		}
		else
		{
			// the 1 case is old, nearly obsolete, to control directly from windows machine
			u16 button_num = c - '1';
			if (button_num == 7)
				pTheLooper->command(LOOP_COMMAND_DUB_MODE);
			if (button_num == 6)
				pTheLooper->command(LOOP_COMMAND_CLEAR_ALL);
			else if (button_num == 5)
				pTheLooper->command(LOOP_COMMAND_STOP_IMMEDIATE);
			else if (button_num == 4)
				pTheLooper->command(LOOP_COMMAND_CLEAR_ALL);
			else if (button_num < 4)
				pTheLooper->command(LOOP_COMMAND_TRACK_BASE + button_num);
		}
	}
	else
	{
		serial_midi_buf[serial_midi_len++] = c;		// add to buffer
		if (serial_midi_len == 4)
		{
			if (serial_midi_buf[0] == 0x0b &&			// CC messages
				serial_midi_buf[1] == 0xb0)
			{
				handleSerialCC(serial_midi_buf[2],serial_midi_buf[3]);
			}
			serial_midi_len = 0;

		}	// reached 4 bytes of midi data
	}	// serial_midi_len != 0
}



void uiWindow::handleSerialCC(u8 cc_num, u8 value)
{
	// LOOPER_LOG("handleSerialCC(0x%02x,0x%02x)",cc_num,value);

	// CC's that map to volume controls
	// also works for the teensyExpression loop pedal

	if (cc_num >= LOOP_CONTROL_BASE_CC &&
		cc_num < LOOP_CONTROL_BASE_CC + LOOPER_NUM_CONTROLS)
	{
		int control_num = cc_num - LOOP_CONTROL_BASE_CC;
		pTheLooper->setControl(control_num,value);
		control_vol[control_num] = value;
			// DONT ECHO IT back to TE
			// The control_vol value must be set AFTER the control change
			// or else, based on timing, it can be detected as a change
			// in updaateRframe() because the value has NOT YET changed.
			// Even then there *might* be occasional spurious sends.
	}

	// CC's that map to buttons

	else if (cc_num == LOOP_COMMAND_CC)
	{
		if (value == LOOP_COMMAND_GET_STATE)
		{
			LOOPER_LOG("LOOP_COMMAND_GET_STATE()",0);
			send_state = 1;		// start sending midi state messags
		}
		else
			pTheLooper->command(value);
	}

	else if (cc_num >= CLIP_VOL_BASE_CC &&
			 cc_num < CLIP_VOL_BASE_CC  + LOOPER_NUM_TRACKS * LOOPER_NUM_LAYERS)
	{
		int num = cc_num - CLIP_VOL_BASE_CC;
		int track_num = num / LOOPER_NUM_LAYERS;
		int clip_num = num % LOOPER_NUM_LAYERS;
		// LOOPER_LOG("Serial clip(%d,%d) volume=%d",track_num,clip_num,value);
		publicTrack *pTrack = pTheLooper->getPublicTrack(track_num);
		publicClip *pClip = pTrack->getPublicClip(clip_num);
		pClip->setVolume(value);
		sendSerialMidiCC(cc_num,value);				// echo back to TE
	}
	else if (cc_num >= CLIP_MUTE_BASE_CC &&
			 cc_num < CLIP_MUTE_BASE_CC  + LOOPER_NUM_TRACKS * LOOPER_NUM_LAYERS)
	{
		int num = cc_num - CLIP_MUTE_BASE_CC;
		int track_num = num / LOOPER_NUM_LAYERS;
		int clip_num = num % LOOPER_NUM_LAYERS;
		// LOOPER_LOG("Serial clip(%d,%d) mute=%d",track_num,clip_num,value);
		publicTrack *pTrack = pTheLooper->getPublicTrack(track_num);
		publicClip *pClip = pTrack->getPublicClip(clip_num);
		pClip->setMute(value);
		sendSerialMidiCC(cc_num,value);				// echo back to TE
	}
}




void uiWindow::sendState()
{
	#define FIRST_TRACK_SEND_STATE   3
	#define NUM_SENDS_PER_TRACK		 3
		// we send the track state and the first four volumes in the first bunch
		// then we send 3 more groups of four volumes, and then 4 groups of mutes

	if (send_state == 1)
	{
		for (int i=0; i<LOOPER_NUM_CONTROLS; i++)
		{
			control_vol[i] = pTheLooper->getControlValue(i);
			sendSerialMidiCC(LOOP_CONTROL_BASE_CC+i,control_vol[i]);
		}
		send_state++;
	}
	else if (send_state == 2)
	{
		sendSerialMidiCC(LOOP_STOP_CMD_STATE_CC,
			pTheLooper->getRunning() ?
			pTheLooper->getPendingCommand() == LOOP_COMMAND_STOP ?
				LOOP_COMMAND_STOP_IMMEDIATE :
				LOOP_COMMAND_STOP : 0);
		sendSerialMidiCC(LOOP_DUB_STATE_CC,pTheLooper->getDubMode());
		send_state++;
	}
	else if (send_state < FIRST_TRACK_SEND_STATE + LOOPER_NUM_TRACKS * NUM_SENDS_PER_TRACK)
	{
		int track_num = (send_state - FIRST_TRACK_SEND_STATE) / NUM_SENDS_PER_TRACK;
		int sub_num = (send_state - FIRST_TRACK_SEND_STATE) % NUM_SENDS_PER_TRACK;
		publicTrack *pTrack = pTheLooper->getPublicTrack(track_num);

		if (sub_num == 0)
		{
			sendSerialMidiCC(TRACK_STATE_BASE_CC + track_num,pTrack->getTrackState() & 0xff);
		}
		else if (sub_num == 1)
		{
			for (int j=0; j<LOOPER_NUM_LAYERS; j++)
			{
				bool mute = pTrack->getPublicClip(j)->isMuted();
				clip_mute[track_num][j] = mute;
				sendSerialMidiCC(CLIP_MUTE_BASE_CC + track_num*LOOPER_NUM_LAYERS + j, mute);
			}
		}
		else // sub_num == 2
		{
			for (int j=0; j<LOOPER_NUM_LAYERS; j++)
			{
				u8 vol = pTrack->getPublicClip(0)->getVolume();
				clip_vol[track_num][j] = vol;
				sendSerialMidiCC(CLIP_VOL_BASE_CC + track_num*LOOPER_NUM_LAYERS + j, vol);
			}
		}

		send_state++;
	}
	else
	{
		send_state = 0;
	}
}