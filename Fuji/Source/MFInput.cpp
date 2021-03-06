#include "Fuji_Internal.h"
#include "MFVector.h"
#include "MFInput_Internal.h"
#include "MFNetwork_Internal.h"
#include "MFThread.h"
#include "MFSystem.h"

// store device status for all devices
static MFInputDeviceStatus	gDeviceStatus[IDD_Max][MFInput_MaxInputID];

// store states for each input device..
struct InputState
{
	MFGamepadState			gamepadStates[MFInput_MaxInputID];
	MFGamepadState			prevGamepadStates[MFInput_MaxInputID];
	MFKeyState				keyStates[MFInput_MaxInputID];
	MFKeyState				prevKeyStates[MFInput_MaxInputID];
	MFMouseState			mouseStates[MFInput_MaxInputID];
	MFMouseState			prevMouseStates[MFInput_MaxInputID];
	MFAccelerometerState	accelerometerStates[MFInput_MaxInputID];
	MFAccelerometerState	prevAccelerometerStates[MFInput_MaxInputID];
	MFTouchPanelState		touchPanelStates[MFInput_MaxInputID];
	MFTouchPanelState		prevTouchPanelStates[MFInput_MaxInputID];
};

static InputState gInputState;
static InputState *gpThreadState = NULL;

static float gGamepadDeadZone = 0.3f;
static float gMouseAccelleration = 1.0f;

static int gNumGamepads = 0;
static int gNumPointers = 0;
static int gNumKeyboards = 0;
static int gNumAccelerometers = 0;
static int gNumTouchPanels = 0;

static int gNetGamepadStart = MFInput_MaxInputID;
static int gNetPointerStart = MFInput_MaxInputID;
static int gNetKeyboardStart = MFInput_MaxInputID;

static int gInputFrequency = 200;
static MFThread gInputThread = NULL;
static MFMutex gInputMutex;
static volatile bool bInputTerminate = false;
static bool bThreadUpdate = false;

static uint32 MaxEvents = 256;
static MFInputEvent *gInputEvents[IDD_Max][MFInput_MaxInputID];
static uint32 gNumEvents[IDD_Max][MFInput_MaxInputID];
static uint32 gNumEventsRead[IDD_Max][MFInput_MaxInputID];

// DIK to ASCII mappings with shift, caps, and shift-caps tables
static const char KEYtoASCII[256]			= {0,0,0,0,0,0,0,0,'\b','\t',0,0,0,'\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,' ',0,0,0,0,0,0,'\'',0,0,'*','+',',','-','.','/','0','1','2','3','4','5','6','7','8','9',',',';','=','=',0,0,0,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','_','[','\\',']',0,0,'`',0,0,0,0,0,0,0,0,0,0,'/','-','.','0','1','2','3','4','5','6','7','8','9','\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,':',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static const char KEYtoASCIISHIFT[256]		= {0,0,0,0,0,0,0,0,'\b','\t',0,0,0,'\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,' ',0,0,0,0,0,0, '"',0,0,'*','+','<','_','>','?',')','!','@','#','$','%','^','&','*','(',',',':','=','+',0,0,0,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','_','{', '|','}',0,0,'~',0,0,0,0,0,0,0,0,0,0,'/','-','.','0','1','2','3','4','5','6','7','8','9','\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,':',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static const char KEYtoASCIICAPS[256]		= {0,0,0,0,0,0,0,0,'\b','\t',0,0,0,'\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,' ',0,0,0,0,0,0,'\'',0,0,'*','+',',','-','.','/','0','1','2','3','4','5','6','7','8','9',',',';','=','=',0,0,0,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','_','[','\\',']',0,0,'`',0,0,0,0,0,0,0,0,0,0,'/','-','.','0','1','2','3','4','5','6','7','8','9','\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,':',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static const char KEYtoASCIICAPSSHIFT[256]	= {0,0,0,0,0,0,0,0,'\b','\t',0,0,0,'\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,' ',0,0,0,0,0,0, '"',0,0,'*','+','<','_','>','?',')','!','@','#','$','%','^','&','*','(',',',':','=','+',0,0,0,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','_','{', '|','}',0,0,'~',0,0,0,0,0,0,0,0,0,0,'/','-','.','0','1','2','3','4','5','6','7','8','9','\n',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,':',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

MF_API char MFInput_KeyToAscii(int key, bool shift, bool caps)
{
	if(shift)
	{
		if(caps)
			return KEYtoASCIICAPSSHIFT[key];
		else
			return KEYtoASCIISHIFT[key];
	}
	else
	{
		if(caps)
			return KEYtoASCIICAPS[key];
		else
			return KEYtoASCII[key];
	}
}

static const char * const gGamepadStrings[] =
{
	"Cross",
	"Circle",
	"Box",
	"Triangle",
	"L1",
	"R1",
	"L2",
	"R2",
	"Start",
	"Select",
	"L3",
	"R3",
	"DPad Up",
	"DPad Down",
	"DPad Left",
	"DPad Right",
	"Left Analog X-Axis",
	"Left Analog Y-Axis",
	"Right Analog X-Axis",
	"Right Analog Y-Axis"
};

static const char * const gKeyNames[] =
{
	"None",
	"Up",
	"Down",
	"Left",
	"Right",
	"Apps",			// on windows keyboards
	"LWin",			// on windows keyboards
	"RWin",			// on windows keyboards
	"Backspace",
	"Tab",
	"PrintScreen",
	"ScrollLock",
	"Break",
	"Return",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"OEM_102",		// on german keyboard
	"Escape",
	"MyComputer",	// on multimedia keyboards
	"Mail",			// on multimedia keyboards
	"Calculator",	// on multimedia keyboards
	"Stop",			// japanese keyboard
	"Space",
	"Insert",
	"Delete",
	"Home",
	"End",
	"PageUp",
	"PageDown",
	"Apostrophe",
	"ABNT_C1",		// on brazilian keyboard
	"ABNT_C2",		// on brazilian keyboard
	"Asterisk",
	"Plus",
	"Comma",
	"Hyphen",
	"Period",
	"Slash",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"NumPadComma",	// japanese keyboard
	"Semicolon",
	"NumPadEquals",	// japanese keyboard
	"Equals",
	"Sleep",		// on windows keyboards
	"Wake",			// on windows keyboards
	"Power",		// on windows keyboards
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"Underline",	// japanese keyboard
	"LBracket",
	"Backslash",
	"RBracket",
	"F13",			// japanese keyboard
	"F14",			// japanese keyboard
	"Grave",
	"F15",			// japanese keyboard
	"Unlabeled",	// japanese keyboard
	"LControl",
	"LAlt",
	"LShift",
	"RControl",
	"RAlt",
	"RShift",
	"Capital",

	"NumLock",
	"Divide",
	"Subtract",
	"Decimal",

	"NumPad0",
	"NumPad1",
	"NumPad2",
	"NumPad3",
	"NumPad4",
	"NumPad5",
	"NumPad6",
	"NumPad7",
	"NumPad8",
	"NumPad9",
	"NumPadEnter",

	"PlayPause",	// on multimedia keyboards
	"MediaStop",	// on multimedia keyboards
	"MediaSelect",	// on multimedia keyboards
	"NextTrack",	// on multimedia keyboards
	"PrevTrack",	// on multimedia keyboards

	"VolumeDown",	// on multimedia keyboards
	"VolumeUp",		// on multimedia keyboards
	"Mute",			// on multimedia keyboards

	"WebBack",		// on multimedia keyboards
	"WebFavorites",	// on multimedia keyboards
	"WebForward",	// on multimedia keyboards
	"WebHome",		// on multimedia keyboards
	"WebRefresh",	// on multimedia keyboards
	"WebSearch",	// on multimedia keyboards
	"WebStop",		// on multimedia keyboards

	"AT",			// japanese keyboard
	"AX",			// japanese keyboard
	"Colon",		// japanese keyboard
	"Convert",		// japanese keyboard
	"Kana",			// japanese keyboard
	"Kanji",		// japanese keyboard
	"NoConvert",	// japanese keyboard
	"Yen",			// japanese keyboard
};

// Functions
MFInitStatus MFInput_InitModule(int moduleId, bool bPerformInitialisation)
{
	MFZeroMemory(&gInputState, sizeof(gInputState));

	gInputMutex = MFThread_CreateMutex("MFInput Mutex");

	MFInput_InitModulePlatformSpecific();
	MFInput_Update();

	return MFIS_Succeeded;
}

void MFInput_DeinitModule()
{
	MFInput_DeinitModulePlatformSpecific();

	MFThread_DestroyMutex(gInputMutex);
}

void MFInputInternal_ApplySphericalDeadZone(float *pX, float *pY)
{
	float length = *pX**pX + *pY**pY;

	if(length)
	{
		length = MFSqrt(length);

		float scale = 1.0f / length * MFClamp(0.0f, (length - gGamepadDeadZone) / (1.0f - gGamepadDeadZone), 1.0f);

		*pX *= scale;
		*pY *= scale;
	}
}

void MFInput_UpdateState(InputState *pState)
{
	int a;

	// platform specific update
	MFInput_UpdatePlatformSpecific();

	// update keyboard state
	for(a=0; a<gNumKeyboards; a++)
	{
		if(MFInput_IsAvailable(IDD_Keyboard, a))
			MFInput_GetKeyStateInternal(a, &pState->keyStates[a]);
	}

	// update gamepad state
	for(a=0; a<gNumGamepads; a++)
	{
		if(MFInput_IsAvailable(IDD_Gamepad, a))
		{
			// update gamepad state
#if !defined(_RETAIL)
			if(gDeviceStatus[IDD_Gamepad][0] == IDS_Unavailable)
				MFInputInternal_GetGamepadStateFromKeyMap(&pState->gamepadStates[a], &pState->keyStates[0]);
			else
#endif
			MFInput_GetGamepadStateInternal(a, &pState->gamepadStates[a]);

			// apply analog deadzone to axiis
			uint32 deviceFlags = MFInput_GetDeviceFlags(IDD_Gamepad, a);
			if(!(deviceFlags & MFGF_DontUseSphericalDeadzone))
			{
				MFInputInternal_ApplySphericalDeadZone(&pState->gamepadStates[a].values[Axis_LX], &pState->gamepadStates[a].values[Axis_LY]);
				MFInputInternal_ApplySphericalDeadZone(&pState->gamepadStates[a].values[Axis_RX], &pState->gamepadStates[a].values[Axis_RY]);
			}
		}
	}

	// update mouse state
	for(a=0; a<gNumPointers; a++)
	{
		if(MFInput_IsAvailable(IDD_Mouse, a))
			MFInput_GetMouseStateInternal(a, &pState->mouseStates[a]);
	}

	// update accelerometer state
	for(a=0; a<gNumAccelerometers; a++)
	{
		if(MFInput_IsAvailable(IDD_Accelerometer, a))
			MFInput_GetAccelerometerStateInternal(a, &pState->accelerometerStates[a]);
	}

	// update touch panel state
	for(a=0; a<gNumTouchPanels; a++)
	{
		if(MFInput_IsAvailable(IDD_TouchPanel, a))
			MFInput_GetTouchPanelStateInternal(a, &pState->touchPanelStates[a]);
	}

	// add network devices
	if(MFNetwork_IsRemoteInputServerRunning())
	{
		gNetGamepadStart = gNumGamepads;
		gNetKeyboardStart = gNumKeyboards;
		gNetPointerStart = gNumPointers;

		MFNetwork_LockInputMutex();

		// add additional gamepads
		for(a=0; a<MFNetwork_MaxRemoteDevices(); a++)
		{
			MFInputDeviceStatus status = (MFInputDeviceStatus)MFNetwork_GetRemoteDeviceStatus(a);

			if(status > IDS_Unavailable)
			{
				gDeviceStatus[IDD_Gamepad][gNetGamepadStart + a] = status;

				MFNetwork_GetRemoteGamepadState(a, &pState->gamepadStates[gNetGamepadStart + a]);

				gNumGamepads = gNetGamepadStart + a + 1;
			}
		}

		MFNetwork_ReleaseInputMutex();
	}
}

void MFInput_Update()
{
	int a, b;

	// update state for each device
	for(a=0; a<IDD_Max; a++)
	{
		for(b=0; b<MFInput_MaxInputID; b++)
		{
			gDeviceStatus[a][b] = MFInput_GetDeviceStatusInternal(a, b);
		}
	}

	// count number of gamepads
	int maxGamepad = -1;
	for(a=0; a<MFInput_MaxInputID; a++)
	{
		if(MFInput_IsAvailable(IDD_Gamepad, a))
			maxGamepad = a;
	}
	gNumGamepads = maxGamepad+1;

	// update number of keyboards
	int maxKB = -1;
	for(a=0; a<MFInput_MaxInputID; a++)
	{
		if(MFInput_IsAvailable(IDD_Keyboard, a))
			maxKB = a;
	}
	gNumKeyboards = maxKB+1;

	// update number of pointers
	int maxMouse = -1;
	for(a=0; a<MFInput_MaxInputID; a++)
	{
		if(MFInput_IsAvailable(IDD_Mouse, a))
			maxMouse = a;
	}
	gNumPointers = maxMouse+1;

	// update number of accelerometers
	int maxAccelerometer = -1;
	for(a=0; a<MFInput_MaxInputID; a++)
	{
		if(MFInput_IsAvailable(IDD_Accelerometer, a))
			maxAccelerometer = a;
	}
	gNumAccelerometers = maxAccelerometer+1;

	// update number of touch panels
	int maxTouchPanel = -1;
	for(a=0; a<MFInput_MaxInputID; a++)
	{
		if(MFInput_IsAvailable(IDD_TouchPanel, a))
			maxTouchPanel = a;
	}
	gNumTouchPanels = maxTouchPanel+1;

	// copy current states into last states
	MFCopyMemory(gInputState.prevGamepadStates, gInputState.gamepadStates, sizeof(gInputState.prevGamepadStates));
	MFCopyMemory(gInputState.prevKeyStates, gInputState.keyStates, sizeof(gInputState.prevKeyStates));
	MFCopyMemory(gInputState.prevMouseStates, gInputState.mouseStates, sizeof(gInputState.prevMouseStates));
	MFCopyMemory(gInputState.prevAccelerometerStates, gInputState.accelerometerStates, sizeof(gInputState.prevAccelerometerStates));
	MFCopyMemory(gInputState.prevTouchPanelStates, gInputState.touchPanelStates, sizeof(gInputState.prevTouchPanelStates));

	// update the frame input state
	if(!gInputThread)
	{
		MFInput_UpdateState(&gInputState);
	}
	else
	{
		MFThread_LockMutex(gInputMutex);

		// copy thread state to new state
		MFCopyMemory(gInputState.gamepadStates, gpThreadState->gamepadStates, sizeof(gInputState.gamepadStates));
		MFCopyMemory(gInputState.keyStates, gpThreadState->keyStates, sizeof(gInputState.keyStates));
		MFCopyMemory(gInputState.mouseStates, gpThreadState->mouseStates, sizeof(gInputState.mouseStates));
		MFCopyMemory(gInputState.accelerometerStates, gpThreadState->accelerometerStates, sizeof(gInputState.accelerometerStates));
		MFCopyMemory(gInputState.touchPanelStates, gpThreadState->touchPanelStates, sizeof(gInputState.touchPanelStates));

		MFThread_ReleaseMutex(gInputMutex);
	}
}

int MFInput_Thread(void *)
{
	// poll input at high frequency...
	uint64 freq = MFSystem_GetRTCFrequency();
	uint64 interval = freq / gInputFrequency;

	uint64 now = MFSystem_ReadRTC();
	uint64 nextSample = now + interval;

	while(!bInputTerminate)
	{
		MFThread_LockMutex(gInputMutex);

		bThreadUpdate = true;
		MFInput_UpdateState(gpThreadState);
		bThreadUpdate = false;

		// build events
		for(uint32 dev=0; dev<IDD_Max; ++dev)
		{
			for(uint32 i=0; i<MFInput_MaxInputID; ++i)
			{
				if(!gDeviceStatus[dev][i] == IDS_Ready)
					continue;

				switch(dev)
				{
					case IDD_Gamepad:
						for(uint32 j=0; j<GamepadType_Max; ++j)
						{
							if(gNumEvents[dev][i] >= MaxEvents)
								break;

							MFGamepadState &state = gpThreadState->gamepadStates[i];
							MFGamepadState &prev = gpThreadState->prevGamepadStates[i];
							if(state.values[j] == prev.values[j])
								continue;

							MFInputEvent &e = gInputEvents[dev][i][gNumEvents[dev][i]++];
							e.timestamp = now;
							e.event = MFIE_Change;
							e.input = j;
							e.state = state.values[j];
							e.prevState = prev.values[j];
						}
						break;

					case IDD_Keyboard:
						for(uint32 j=0; j<Key_Max; ++j)
						{
							if(gNumEvents[dev][i] >= MaxEvents)
								break;

							MFKeyState &state = gpThreadState->keyStates[i];
							MFKeyState &prev = gpThreadState->prevKeyStates[i];
							if(state.keys[j] == prev.keys[j])
								continue;

							MFInputEvent &e = gInputEvents[dev][i][gNumEvents[dev][i]++];
							e.timestamp = now;
							e.event = MFIE_Change;
							e.input = j;
							e.state = state.keys[j] ? 1.f : 0.f;
							e.prevState = prev.keys[j] ? 1.f : 0.f;
						}
						break;

					default:
						// TODO: parse other input devices into events...
						break;
				}
			}
		}

		MFThread_ReleaseMutex(gInputMutex);

//		uint64 updateTime = MFSystem_ReadRTC();
//		MFDebug_Log(0, MFStr("Input update: %dus", (uint32)((updateTime - now) * 1000000LL / MFSystem_GetRTCFrequency())));

		uint32 ms = (uint32)((nextSample - now) * 1000LL / freq);
		MFThread_Sleep(ms);

		now = MFSystem_ReadRTC();
		do
			nextSample += interval;
		while(now >= nextSample);
	}

	bInputTerminate = false;
	return 0;
}

MF_API void MFInput_EnableBufferedInput(bool bEnable, int frequency)
{
	gInputFrequency = frequency;

	if(bEnable && !gInputThread)
	{
		gpThreadState = (InputState*)MFHeap_AllocAndZero(sizeof(InputState));

		for(uint32 dev=0; dev<IDD_Max; ++dev)
		{
			for(int i=0; i<MFInput_MaxInputID; ++i)
			{
				gInputEvents[dev][i] = (MFInputEvent*)MFHeap_Alloc(sizeof(MFInputEvent)*MaxEvents);
				gNumEvents[dev][i] = 0;
			}
		}

		gInputThread = MFThread_CreateThread("MFInput Thread", &MFInput_Thread, NULL);
	}
	else if(!bEnable && gInputThread)
	{
		bInputTerminate = true;
		MFThread_Join(gInputThread);
		gInputThread = NULL;

		for(uint32 dev=0; dev<IDD_Max; ++dev)
		{
			for(int i=0; i<MFInput_MaxInputID; ++i)
				MFHeap_Free(gInputEvents[dev][i]);
		}

		MFHeap_Free(gpThreadState);
		gpThreadState = NULL;
	}
}

MF_API bool MFInput_IsAvailable(int device, int deviceID)
{
	MFDebug_Assert(device >= 0 && device < IDD_Max, "Invalid Input Device");
	MFDebug_Assert(deviceID >= 0 && deviceID < MFInput_MaxInputID, "Invalid DeviceID");

	bool available = gDeviceStatus[device][deviceID] != IDS_Unavailable;

#if !defined(_RETAIL)
	// this allow's a device with a keyboard and no gamepad's to emulate a gamepad with the keyboard
	if(!available && device == IDD_Gamepad && deviceID == 0 && gNumKeyboards)
		available = true;
#endif

	return available;
}

MF_API bool MFInput_IsConnected(int device, int deviceID)
{
	MFDebug_Assert(device >= 0 && device < IDD_Max, "Invalid Input Device");
	MFDebug_Assert(deviceID >= 0 && deviceID < MFInput_MaxInputID, "Invalid DeviceID");

	bool connected = gDeviceStatus[device][deviceID] != IDS_Disconnected && gDeviceStatus[device][deviceID] != IDS_Unavailable;

#if !defined(_RETAIL)
	// this allow's a device with a keyboard and no gamepad's to emulate a gamepad with the keyboard
	if(!connected && device == IDD_Gamepad && deviceID == 0 && gNumKeyboards)
	{
		connected = MFInput_GetKeyboardStatusState(KSS_ScrollLock, 0);
	}
#endif

	return connected;
}

MF_API bool MFInput_IsReady(int device, int deviceID)
{
	MFDebug_Assert(device >= 0 && device < IDD_Max, "Invalid Input Device");
	MFDebug_Assert(deviceID >= 0 && deviceID < MFInput_MaxInputID, "Invalid DeviceID");

	bool ready = gDeviceStatus[device][deviceID] == IDS_Ready;

#if !defined(_RETAIL)
	// this allow's a device with a keyboard and no gamepad's to emulate a gamepad with the keyboard
	if(!ready && device == IDD_Gamepad && deviceID == 0 && gNumKeyboards)
	{
		ready = MFInput_GetKeyboardStatusState(KSS_ScrollLock, 0);
	}
#endif

	return ready;
}

MF_API float MFInput_Read(int button, int device, int deviceID, float *pPrevState)
{
	MFDebug_Assert(device >= 0 && device < IDD_Max, "Invalid Input Device");
	MFDebug_Assert(deviceID >= -1 && deviceID < MFInput_MaxInputID, "Invalid DeviceID");

	if(deviceID == -1)
	{
		float value = 0.0f;

		for(int a=0; a<MFInput_MaxInputID && !value; a++)
		{
			value = MFInput_Read(button, device, a);
		}

		return value;
	}

	switch(device)
	{
		case IDD_Gamepad:
		{
			if(pPrevState)
				*pPrevState = gInputState.prevGamepadStates[deviceID].values[button];

			return gInputState.gamepadStates[deviceID].values[button];
		}
		case IDD_Mouse:
			if(button < Mouse_MaxAxis)
			{
				if(pPrevState)
					*pPrevState = gInputState.prevMouseStates[deviceID].values[button];

				return gInputState.mouseStates[deviceID].values[button];
			}
			else if(button < Mouse_Max)
			{
				if(pPrevState)
					*pPrevState = gInputState.prevMouseStates[deviceID].buttonState[button - Mouse_MaxAxis] ? 1.0f : 0.0f;

				return gInputState.mouseStates[deviceID].buttonState[button - Mouse_MaxAxis] ? 1.0f : 0.0f;
			}
			break;
		case IDD_Keyboard:
			if(pPrevState)
				*pPrevState = gInputState.prevKeyStates[deviceID].keys[button] ? 1.0f : 0.0f;

			return gInputState.keyStates[deviceID].keys[button] ? 1.0f : 0.0f;
		case IDD_Accelerometer:
			if(button < Acc_XDelta)
			{
				if(pPrevState)
					*pPrevState = gInputState.prevAccelerometerStates[deviceID].values[button];

				return gInputState.accelerometerStates[deviceID].values[button];
			}
			return gInputState.accelerometerStates[deviceID].values[button] - gInputState.accelerometerStates[deviceID].values[button - Acc_XDelta];
		case IDD_TouchPanel:
		{
			if(button >= Touch_Shake)
			{
				switch(button)
				{
					case Touch_Shake:
						return gInputState.touchPanelStates[deviceID].bDidShake ? 1.f : 0.f;
				}
			}

			int contact = button / Touch_Contact1_XPos;
			int control = button % Touch_Contact1_XPos;
			switch(control)
			{
				case Touch_Contact0_XPos:
					return (float)gInputState.touchPanelStates[deviceID].contacts[contact].x;
				case Touch_Contact0_YPos:
					return (float)gInputState.touchPanelStates[deviceID].contacts[contact].y;
				case Touch_Contact0_XDelta:
					return (float)(gInputState.touchPanelStates[deviceID].contacts[contact].x - gInputState.prevTouchPanelStates[deviceID].contacts[contact].x);
				case Touch_Contact0_YDelta:
					return (float)(gInputState.touchPanelStates[deviceID].contacts[contact].y - gInputState.prevTouchPanelStates[deviceID].contacts[contact].y);
			}
		}
		default:
			break;
	}

	if(pPrevState)
		*pPrevState = 0.0f;

	return 0.0f;
}


MF_API bool MFInput_WasPressed(int button, int device, int deviceID)
{
	MFDebug_Assert(device >= 0 && device < IDD_Max, "Invalid Input Device");
	MFDebug_Assert(deviceID >= -1 && deviceID < MFInput_MaxInputID, "Invalid DeviceID");

	if(deviceID == -1)
	{
		bool value = false;

		for(int a=0; a<MFInput_MaxInputID && !value; a++)
		{
			value = MFInput_WasPressed(button, device, a);
		}

		return value;
	}

	switch(device)
	{
		case IDD_Gamepad:
		{
			return gInputState.gamepadStates[deviceID].values[button] && !gInputState.prevGamepadStates[deviceID].values[button];
		}
		case IDD_Mouse:
			if(button < Mouse_MaxAxis)
			{
				return gInputState.mouseStates[deviceID].values[button] && !gInputState.prevMouseStates[deviceID].values[button];
			}
			else if(button < Mouse_Max)
			{
				return gInputState.mouseStates[deviceID].buttonState[button - Mouse_MaxAxis] && !gInputState.prevMouseStates[deviceID].buttonState[button - Mouse_MaxAxis];
			}
			break;
		case IDD_Keyboard:
			return gInputState.keyStates[deviceID].keys[button] && !gInputState.prevKeyStates[deviceID].keys[button];
		default:
			break;
	}
	return false;
}

MF_API bool MFInput_WasReleased(int button, int device, int deviceID)
{
	MFDebug_Assert(device >= 0 && device < IDD_Max, "Invalid Input Device");
	MFDebug_Assert(deviceID >= -1 && deviceID < MFInput_MaxInputID, "Invalid DeviceID");

	if(deviceID == -1)
	{
		bool value = false;

		for(int a=0; a<MFInput_MaxInputID && !value; a++)
		{
			value = MFInput_WasReleased(button, device, a);
		}

		return value;
	}

	switch(device)
	{
		case IDD_Gamepad:
		{
			return !gInputState.gamepadStates[deviceID].values[button] && gInputState.prevGamepadStates[deviceID].values[button];
		}
		case IDD_Mouse:
			if(button < Mouse_MaxAxis)
			{
				return !gInputState.mouseStates[deviceID].values[button] && gInputState.prevMouseStates[deviceID].values[button];
			}
			else if(button < Mouse_Max)
			{
				return !gInputState.mouseStates[deviceID].buttonState[button - Mouse_MaxAxis] && gInputState.prevMouseStates[deviceID].buttonState[button - Mouse_MaxAxis];
			}
			break;
		case IDD_Keyboard:
			return !gInputState.keyStates[deviceID].keys[button] && gInputState.prevKeyStates[deviceID].keys[button];
		default:
			break;
	}
	return false;
}

MF_API size_t MFInput_GetEvents(int device, int deviceID, MFInputEvent *pEvents, size_t maxEvents, bool bPeek)
{
	MFThread_LockMutex(gInputMutex);

	uint32 toRead = 0;
	if(gNumEvents[device][deviceID] != 0)
	{
		if(!pEvents)
		{
			toRead = gNumEvents[device][deviceID] - gNumEventsRead[device][deviceID];
		}
		else
		{
			toRead = MFMin((uint32)maxEvents, gNumEvents[device][deviceID] - gNumEventsRead[device][deviceID]);
			MFCopyMemory(pEvents, gInputEvents[device][deviceID] + gNumEventsRead[device][deviceID], sizeof(MFInputEvent)*toRead);

			if(!bPeek)
			{
				gNumEventsRead[device][deviceID] += toRead;
				if(gNumEventsRead[device][deviceID] == gNumEvents[device][deviceID])
					gNumEvents[device][deviceID] = gNumEventsRead[device][deviceID] = 0;
			}
		}
	}

	MFThread_ReleaseMutex(gInputMutex);

	return toRead;
}

MF_API int MFInput_GetNumGamepads()
{
	return gNumGamepads;
}

MF_API int MFInput_GetNumKeyboards()
{
	return gNumKeyboards;
}

MF_API int MFInput_GetNumPointers()
{
	return gNumPointers;
}

MF_API int MFInput_GetNumAccelerometers()
{
	return gNumAccelerometers;
}

MF_API int MFInput_GetNumTouchPanels()
{
	return gNumTouchPanels;
}

MF_API MFVector MFInput_ReadMousePos(int mouseID)
{
	MFVector pos = MFVector::identity;

	if(mouseID < 0)
	{
		for(int a=0; a<MFInput_MaxInputID; a++)
		{
			MFDebug_Assert(false, "Reading any mouse not written");
		}
	}
	else
	{
		pos.x = MFInput_Read(Mouse_XPos, IDD_Mouse, mouseID);
		pos.y = MFInput_Read(Mouse_YPos, IDD_Mouse, mouseID);
		pos.z = 0.0f;
	}

	return pos;
}

MF_API MFVector MFInput_ReadMouseDelta(int mouseID)
{
	MFVector delta = MFVector::zero;

	if(mouseID < 0)
	{
		for(int a=0; a<MFInput_MaxInputID; a++)
		{
			MFDebug_Assert(false, "Reading any mouse delta not written");
		}
	}
	else
	{
		delta.x = MFInput_Read(Mouse_XDelta, IDD_Mouse, mouseID);
		delta.y = MFInput_Read(Mouse_YDelta, IDD_Mouse, mouseID);
		delta.z = 0.0f;
	}

	return delta;
}

MF_API void MFInput_SetMouseMode(int mode)
{
	MFDebug_Assert(false, "SetMouseMode not written");
}

MF_API void MFInput_SetMouseAcceleration(float multiplier)
{
	MFDebug_Assert(false, "SetMouseAcceleration not written");
}

MF_API MFTouchPanelState *MFInput_GetContactInfo(int touchPanel)
{
	return &gInputState.touchPanelStates[touchPanel];
}

MF_API const char* MFInput_GetDeviceName(int device, int deviceID)
{
	if(deviceID >= gNetGamepadStart && device == IDD_Gamepad)
	{
		return MFStr("Remote %s", MFNetwork_GetRemoteGamepadName(deviceID - gNetGamepadStart));
	}
	else
	{
#if !defined(_RETAIL)
		if(deviceID == IDD_Gamepad && gDeviceStatus[device][deviceID] == IDS_Unavailable)
			return "Keyboard Emulation";
#endif
		return MFInput_GetDeviceNameInternal(device, deviceID);
	}
}

MF_API const char* MFInput_GetGamepadButtonName(int button, int deviceID)
{
	if(deviceID >= gNetGamepadStart)
	{
		return MFNetwork_GetRemoteGamepadButtonName(deviceID - gNetGamepadStart, button);
	}
	else
	{
#if !defined(_RETAIL)
		if(deviceID == IDD_Gamepad && gDeviceStatus[IDD_Gamepad][0] == IDS_Unavailable)
			return gGamepadStrings[button];
#endif
		return MFInput_GetGamepadButtonNameInternal(button, deviceID);
	}
}

MF_API const char* MFInput_EnumerateString(int button, int device, int deviceID, bool includeDevice, bool includeDeviceID)
{
	MFDebug_Assert(device >= 0 && device < IDD_Max, "Invalid Input Device");

	switch(device)
	{
		case IDD_Gamepad:
			return MFStr("%s%s%s%s", includeDevice ? MFInput_GetDeviceName(device, deviceID) : "", includeDeviceID ? MFStr("(%d)", deviceID) : "", (includeDevice||includeDeviceID) ? " " : "", MFInput_GetGamepadButtonName(button, deviceID));
		case IDD_Mouse:
			if(button < Mouse_MaxAxis)
			{

			}
			else
			{
				return MFStr("%s%s%sButton %d", includeDevice ? MFInput_GetDeviceName(device, deviceID) : "", includeDeviceID ? MFStr("(%d)", deviceID) : "", (includeDevice||includeDeviceID) ? " " : "", button - Mouse_MaxAxis + 1);
			}
		case IDD_Keyboard:
			return MFStr("%s%s%s%s", includeDevice ? MFInput_GetDeviceName(device, deviceID) : "", includeDeviceID ? MFStr("(%d)", deviceID) : "", (includeDevice||includeDeviceID) ? " " : "", gKeyNames[button]);
	}

	return "";
}

MF_API void MFInput_SetDeadZone(float deadZone)
{
	gGamepadDeadZone = deadZone;
}

MF_API float MFInput_GetDeadZone()
{
	return gGamepadDeadZone;
}


// gamepad mappings..
char gamepadMappingTable[] =
{
	Mapping_Cross, // Button_P2_Cross
	Mapping_Circle, // Button_P2_Circle
	Mapping_Box, // Button_P2_Box
	Mapping_Triangle, // Button_P2_Triangle

	Mapping_R1, // Button_P2_R1
	Mapping_L1, // Button_P2_L1
	Mapping_L2, // Button_P2_L2
	Mapping_R2, // Button_P2_R2

	Mapping_Start, // Button_P2_Start
	Mapping_Select, // Button_P2_Select

	Mapping_LThumb, // Button_P2_LThumb
	Mapping_RThumb, // Button_P2_RThumb

// general controller enums
	Mapping_DUp, // Button_DUp
	Mapping_DDown, // Button_DDown
	Mapping_DLeft, // Button_DLeft
	Mapping_DRight, // Button_DRight

	Mapping_LX,	// Axis_LX,
	Mapping_LY,	// Axis_LY,
	Mapping_RX,	// Axis_RX,
	Mapping_RY,	// Axis_RY,
};

void MFInputInternal_GetGamepadStateFromKeyMap(MFGamepadState *pGamepadState, MFKeyState *pKeyState)
{
	if(!MFInput_GetKeyboardStatusState(KSS_ScrollLock, 0))
		return;

	for(int a=0; a<GamepadType_Max; a++)
	{
		pGamepadState->values[a] = pKeyState->keys[(int)gamepadMappingTable[a]] ? 1.0f : 0.0f;
	}
}
