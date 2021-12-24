#pragma once
#pragma once

enum VirtualKeyEvents_t
{
	KEYDOWN = 0x0100,
	KEYUP = 0x0101,
	SYSKEYDOWN = 0x104,
	SYSKEYUP = 0x105,
	LBUTTONDOWN = 0x0201,
	LBUTTONUP = 0x0202,
	RBUTTONDOWN = 0x0204,
	RBUTTONUP = 0x0205,
	MBUTTONDOWN = 0x0207,
	MBUTTONUP = 0x0208,
	MOUSEWHEEL = 0x020A,
	XBUTTONDOWN = 0x020B,
	XBUTTONUP = 0x020C,
};

enum VirtualKeys_t
{
	KEYS_NONE = 0,
	KEYS_MOUSE1 = 0X01,
	KEYS_MOUSE2 = 0X02,
	KEYS_CANCEL = 0X03,
	KEYS_MOUSE3 = 0X04,
	KEYS_MOUSE4 = 0X05,
	KEYS_MOUSE5 = 0X06,
	KEYS_BACK = 0X08,
	KEYS_TAB = 0X09,
	KEYS_CLEAR = 0X0C,
	KEYS_RETURN = 0X0D,
	KEYS_SHIFT = 0X10,
	KEYS_CONTROL = 0X11,
	KEYS_ALT = 0X12,
	KEYS_PAUSE = 0X13,
	KEYS_CAPSLOCK = 0X14,
	KEYS_ESCAPE = 0X1B,
	KEYS_CONVERT = 0X1C,
	KEYS_NONCONVERT = 0X1D,
	KEYS_ACCEPT = 0X1E,
	KEYS_MODECHANGE = 0X1F,
	KEYS_SPACE = 0X20,
	KEYS_PRIOR = 0X21,
	KEYS_NEXT = 0X22,
	KEYS_END = 0X23,
	KEYS_HOME = 0X24,
	KEYS_LEFT = 0X25,
	KEYS_UP = 0X26,
	KEYS_RIGHT = 0X27,
	KEYS_DOWN = 0X28,
	KEYS_SELECT = 0X29,
	KEYS_PRINT = 0X2A,
	KEYS_EXECUTE = 0X2B,
	KEYS_SNAPSHOT = 0X2C,
	KEYS_INSERT = 0X2D,
	KEYS_DELETE = 0X2E,
	KEYS_HELP = 0X2F,
	KEYS_N0 = 0X30,
	KEYS_N1 = 0X31,
	KEYS_N2 = 0X32,
	KEYS_N3 = 0X33,
	KEYS_N4 = 0X34,
	KEYS_N5 = 0X35,
	KEYS_N6 = 0X36,
	KEYS_N7 = 0X37,
	KEYS_N8 = 0X38,
	KEYS_N9 = 0X39,
	KEYS_A = 0X41,
	KEYS_B = 0X42,
	KEYS_C = 0X43,
	KEYS_D = 0X44,
	KEYS_E = 0X45,
	KEYS_F = 0X46,
	KEYS_G = 0X47,
	KEYS_H = 0X48,
	KEYS_I = 0X49,
	KEYS_J = 0X4A,
	KEYS_K = 0X4B,
	KEYS_L = 0X4C,
	KEYS_M = 0X4D,
	KEYS_N = 0X4E,
	KEYS_O = 0X4F,
	KEYS_P = 0X50,
	KEYS_Q = 0X51,
	KEYS_R = 0X52,
	KEYS_S = 0X53,
	KEYS_T = 0X54,
	KEYS_U = 0X55,
	KEYS_V = 0X56,
	KEYS_W = 0X57,
	KEYS_X = 0X58,
	KEYS_Y = 0X59,
	KEYS_Z = 0X5A,
	KEYS_LEFTWINDOWS = 0X5B,
	KEYS_RIGHTWINDOWS = 0X5C,
	KEYS_APPLICATION = 0X5D,
	KEYS_NUMPAD0 = 0X60,
	KEYS_NUMPAD1 = 0X61,
	KEYS_NUMPAD2 = 0X62,
	KEYS_NUMPAD3 = 0X63,
	KEYS_NUMPAD4 = 0X64,
	KEYS_NUMPAD5 = 0X65,
	KEYS_NUMPAD6 = 0X66,
	KEYS_NUMPAD7 = 0X67,
	KEYS_NUMPAD8 = 0X68,
	KEYS_NUMPAD9 = 0X69,
	KEYS_MULTIPLY = 0X6A,
	KEYS_ADD = 0X6B,
	KEYS_SEPARATOR = 0X6C,
	KEYS_SUBTRACT = 0X6D,
	KEYS_DECIMAL = 0X6E,
	KEYS_DIVIDE = 0X6F,
	KEYS_F1 = 0X70,
	KEYS_F2 = 0X71,
	KEYS_F3 = 0X72,
	KEYS_F4 = 0X73,
	KEYS_F5 = 0X74,
	KEYS_F6 = 0X75,
	KEYS_F7 = 0X76,
	KEYS_F8 = 0X77,
	KEYS_F9 = 0X78,
	KEYS_F10 = 0X79,
	KEYS_F11 = 0X7A,
	KEYS_F12 = 0X7B,
	KEYS_NUMLOCK = 0X90,
	KEYS_SCROLLLOCK = 0X91,
	KEYS_LEFTSHIFT = 0XA0,
	KEYS_RIGHTSHIFT = 0XA1,
	KEYS_LEFTCONTROL = 0XA2,
	KEYS_RIGHTCONTROL = 0XA3,
	KEYS_LEFTMENU = 0XA4,
	KEYS_RIGHTMENU = 0XA5,
	KEYS_MAX = 0XA6
};

class c_input_manager
{
	bool m_pressed_keys[KEYS_MAX];
	int m_mouse_pos[2];
	int m_scroll_wheel_state;
public:
	void capture_mouse_move( long lparam );

	//registers a key press from wndproc
	bool register_key_press( VirtualKeyEvents_t key_event, VirtualKeys_t key );

	//checks if the key is pressed
	bool is_key_pressed( int key );
	bool is_key_pressed_toggle( int key );

	//returns the first found key pressed, or KEY_NONE if none are
	VirtualKeys_t is_any_key_pressed( );

	//returns the last scroll state and resets it to 0
	int get_scroll_state( );

	//returns the key's name
	const char* get_key_name( VirtualKeys_t key );

	//returns the first found currently pressed key
	char get_pressed_char( VirtualKeys_t* pressed_key = nullptr );

	//check if a key is valid
	inline bool is_valid_key( VirtualKeys_t key ) { return key > KEYS_NONE && key < KEYS_MAX; }

	//get cursor pos
	inline void get_cursor_pos( int& x, int& y )
	{
		x = m_mouse_pos[0];
		y = m_mouse_pos[1];
	}
};


extern c_input_manager g_input;