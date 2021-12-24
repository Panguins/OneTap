#include "input.h"
#include <string>

c_input_manager g_input;

const char* const key_names[] =
{
	"unknown",
	"mouse1",
	"mouse2",
	"cancel",
	"mouse3",
	"mouse4",
	"mouse5",
	"unknown",
	"backspace",
	"Tab",
	"unknown",
	"unknown",
	"clear",
	"return",
	"unknown",
	"unknown",
	"shift",
	"control",
	"alt",
	"pause",
	"caps lock",
	"kana",
	"unknown",
	"junja",
	"final",
	"kanji",
	"unknown",
	"escape",
	"convert",
	"nonconvert",
	"accept",
	"modechange",
	"space",
	"prior",
	"next",
	"end",
	"home",
	"left",
	"up",
	"right",
	"down",
	"select",
	"print",
	"execute",
	"snapshot",
	"ins",
	"delete",
	"help",
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
	"unknown",
	"unknown",
	"unknown",
	"unknown",
	"unknown",
	"unknown",
	"unknown",
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
	"lwin",
	"rwin",
	"apps",
	"unknown",
	"unknown",
	"numpad0",
	"numpad1",
	"numpad2",
	"numpad3",
	"numpad4",
	"numpad5",
	"numpad6",
	"numpad7",
	"numpad8",
	"numpad9",
	"multiply",
	"add",
	"separator",
	"subtract",
	"decimal",
	"divide",
	"f1",
	"f2",
	"f3",
	"f4",
	"f5",
	"f6",
	"f7",
	"f8",
	"f9",
	"f10",
	"f11",
	"f12",
	"f13",
	"f14",
	"f15",
	"f16",
	"f17",
	"f18",
	"f19",
	"f20",
	"f21",
	"f22",
	"f23",
	"f24",
	"unknown",
	"unknown",
	"unknown",
	"unknown",
	"unknown",
	"unknown",
	"unknown",
	"unknown",
	"numlock",
	"scroll",
	"oem_nec_equal",
	"oem_fj_masshou",
	"oem_fj_touroku",
	"oem_fj_loya",
	"oem_fj_roya",
	"unknown",
	"unknown",
	"unknown",
	"unknown",
	"unknown",
	"unknown",
	"unknown",
	"unknown",
	"unknown",
	"lshift",
	"rshift",
	"lcontrol",
	"rcontrol",
	"lmenu",
	"rmenu",
};

/*
const char* const key_names[] =
{
	xors( "unknown" ),
	xors( "mouse1" ),
	xors( "mouse2" ),
	xors( "cancel" ),
	xors( "mouse3" ),
	xors( "mouse4" ),
	xors( "mouse5" ),
	xors( "unknown" ),
	xors( "Backspace" ),
	xors( "Tab" ),
	xors( "unknown" ),
	xors( "unknown" ),
	xors( "Clear" ),
	xors( "Return" ),
	xors( "unknown" ),
	xors( "unknown" ),
	xors( "Shift" ),
	xors( "Control" ),
	xors( "Alt" ),
	xors( "pause" ),
	xors( "Caps Lock" ),
	xors( "kana" ),
	xors( "unknown" ),
	xors( "junja" ),
	xors( "final" ),
	xors( "kanji" ),
	xors( "unknown" ),
	xors( "Escape" ),
	xors( "convert" ),
	xors( "nonconvert" ),
	xors( "accept" ),
	xors( "modechange" ),
	xors( "Space" ),
	xors( "prior" ),
	xors( "next" ),
	xors( "end" ),
	xors( "home" ),
	xors( "Left" ),
	xors( "Up" ),
	xors( "Right" ),
	xors( "Down" ),
	xors( "select" ),
	xors( "print" ),
	xors( "execute" ),
	xors( "snapshot" ),
	xors( "ins" ),
	xors( "delete" ),
	xors( "help" ),
	xors( "0" ),
	xors( "1" ),
	xors( "2" ),
	xors( "3" ),
	xors( "4" ),
	xors( "5" ),
	xors( "6" ),
	xors( "7" ),
	xors( "8" ),
	xors( "9" ),
	xors( "unknown" ),
	xors( "unknown" ),
	xors( "unknown" ),
	xors( "unknown" ),
	xors( "unknown" ),
	xors( "unknown" ),
	xors( "unknown" ),
	xors( "A" ),
	xors( "B" ),
	xors( "C" ),
	xors( "D" ),
	xors( "E" ),
	xors( "F" ),
	xors( "G" ),
	xors( "H" ),
	xors( "I" ),
	xors( "J" ),
	xors( "K" ),
	xors( "L" ),
	xors( "M" ),
	xors( "N" ),
	xors( "O" ),
	xors( "P" ),
	xors( "Q" ),
	xors( "R" ),
	xors( "S" ),
	xors( "T" ),
	xors( "U" ),
	xors( "V" ),
	xors( "W" ),
	xors( "X" ),
	xors( "Y" ),
	xors( "Z" ),
	xors( "lwin" ),
	xors( "rwin" ),
	xors( "apps" ),
	xors( "unknown" ),
	xors( "unknown" ),
	xors( "numpad0" ),
	xors( "numpad1" ),
	xors( "numpad2" ),
		xors( "numpad3" ),
		xors( "numpad4" ),
		xors( "numpad5" ),
		xors( "numpad6" ),
		xors( "numpad7" ),
		xors( "numpad8" ),
		xors( "numpad9" ),
		xors( "multiply" ),
		xors( "add" ),
		xors( "separator" ),
		xors( "subtract" ),
		xors( "decimal" ),
		xors( "divide" ),
		xors( "f1" ),
		xors( "f2" ),
		xors( "f3" ),
		xors( "f4" ),
		xors( "f5" ),
		xors( "f6" ),
		xors( "f7" ),
		xors( "f8" ),
		xors( "f9" ),
		xors( "f10" ),
		xors( "f11" ),
		xors( "f12" ),
		xors( "f13" ),
		xors( "f14" ),
		xors( "f15" ),
		xors( "f16" ),
		xors( "f17" ),
		xors( "f18" ),
		xors( "f19" ),
		xors( "f20" ),
		xors( "f21" ),
		xors( "f22" ),
		xors( "f23" ),
		xors( "f24" ),
		xors( "unknown" ),
		xors( "unknown" ),
			  xors( "unknown" ),
		xors( "unknown" ),
		xors( "unknown" ),
		xors( "unknown" ),
		xors( "unknown" ),
		xors( "unknown" ),
		xors( "numlock" ),
		xors( "scroll" ),
		xors( "oem_nec_equal" ),
		xors( "oem_fj_masshou" ),
		xors( "oem_fj_touroku" ),
		xors( "oem_fj_loya" ),
		xors( "oem_fj_roya" ),
			  xors( "unknown" ),
		xors( "unknown" ),
		xors( "unknown" ),
		xors( "unknown" ),
		xors( "unknown" ),
		xors( "unknown" ),
		xors( "unknown" ),
		xors( "unknown" ),
		xors( "unknown" ),
		xors( "lshift" ),
		xors( "rshift" ),
		xors( "lcontrol" ),
		xors( "rcontrol" ),
		xors( "lmenu" ),
	xors( "rmenu" ),
};
*/

void c_input_manager::capture_mouse_move( long lparam )
{
	m_mouse_pos[0] = LOWORD( lparam );
	m_mouse_pos[1] = HIWORD( lparam );
}

bool c_input_manager::register_key_press( VirtualKeyEvents_t key_event, VirtualKeys_t key )
{
	switch ( key_event )
	{
	case KEYDOWN:
	{
		if ( is_valid_key( key ) )
			m_pressed_keys[key] = true;
		return true;
	}
	case KEYUP:
	{
		if ( is_valid_key( key ) )
			m_pressed_keys[key] = false;
		return true;
	}
	case SYSKEYDOWN:
	{ //WTF IS THIS STUPID SHIT, WHY IS ALT LITERALLY THE ONLY FUCKING KEY UNDER SYSKEYDOWN OUT OF ALL THE MODIFIER KEYS?
		if ( key == KEYS_ALT )
			m_pressed_keys[key] = true;
		break;
	}
	case SYSKEYUP:
	{
		if ( key == KEYS_ALT )
			m_pressed_keys[key] = false;
		break;
	}
	case LBUTTONDOWN:
		m_pressed_keys[KEYS_MOUSE1] = true;
		return true;
	case LBUTTONUP:
		m_pressed_keys[KEYS_MOUSE1] = false;
		return true;
	case RBUTTONDOWN:
		m_pressed_keys[KEYS_MOUSE2] = true;
		return true;
	case RBUTTONUP:
		m_pressed_keys[KEYS_MOUSE2] = false;
		return true;
	case MBUTTONDOWN:
		m_pressed_keys[KEYS_MOUSE3] = true;
		return true;
	case MBUTTONUP:
		m_pressed_keys[KEYS_MOUSE3] = false;
		return true;
	case XBUTTONDOWN:
	{
		bool pressed_xbutton = static_cast<bool>( HIWORD( key ) - 1 ); //should result in mouse4 as false, and mouse5 as true
		m_pressed_keys[pressed_xbutton ? KEYS_MOUSE5 : KEYS_MOUSE4] = true;
		return true;
	}
	case XBUTTONUP:
	{
		bool pressed_xbutton = static_cast<bool>( HIWORD( key ) - 1 ); //should result in mouse4 as false, and mouse5 as true
		m_pressed_keys[pressed_xbutton ? KEYS_MOUSE5 : KEYS_MOUSE4] = false;
		return true;
	}
	case MOUSEWHEEL:
	{
		short scroll_input = ( short ) HIWORD( key );
		m_scroll_wheel_state = scroll_input > 0 ? 1 : -1;
		return true;
	}
	}

	return key_event == 0x200 || key_event == 0x203 || key_event == 0x206 || key_event == 0x209; //gotta block WM_MOUSEFIST | WM_LBUTTONDBLCLK | WM_RBUTTONDBLCLK | WM_MBUTTONDBLCLK
}

bool c_input_manager::is_key_pressed( int key )
{
	auto k = static_cast< VirtualKeys_t >( key );
	return is_valid_key( k ) && m_pressed_keys[k];
}

const char* c_input_manager::get_key_name( VirtualKeys_t key )
{
	if ( !is_valid_key( key ) )
		return key_names[KEYS_NONE];

	return key_names[key];
}

VirtualKeys_t c_input_manager::is_any_key_pressed( )
{
	for ( size_t i{ }; i < KEYS_MAX; ++i )
	{
		if ( m_pressed_keys[i] )
		{
			return VirtualKeys_t( i );
		}
	}

	return KEYS_NONE;
}

int c_input_manager::get_scroll_state( )
{
	int current_state = m_scroll_wheel_state;
	m_scroll_wheel_state = 0;
	return current_state;
}

char c_input_manager::get_pressed_char( VirtualKeys_t* out )
{
	size_t pressed_character{ };
	for ( size_t i{ }; i < KEYS_MAX; ++i )
	{
		if ( is_key_pressed( VirtualKeys_t( i ) ) )
		{
			if ( ( i >= KEYS_A && i <= KEYS_Z )
				 || ( i >= KEYS_N0 && i <= KEYS_N9 ) )
			{
				pressed_character = i;
			}
		}
	}

	if ( pressed_character )
	{
		if ( out )
		{
			*out = VirtualKeys_t( pressed_character );
		}

		if ( is_key_pressed( KEYS_SHIFT ) )
		{
			if ( pressed_character >= KEYS_A
				 && pressed_character <= KEYS_Z )
				return char( pressed_character );

			//gay way to shift it to symbols
			if ( pressed_character >= KEYS_N0
				 && pressed_character <= KEYS_N9 )
			{
				switch ( pressed_character )
				{
				case KEYS_N0:
					return ')';
				case KEYS_N1:
					return '!';
				case KEYS_N2:
					return '@';
				case KEYS_N3:
					return '#';
				case KEYS_N4:
					return '$';
				case KEYS_N5:
					return '%';
				case KEYS_N6:
					return '^';
				case KEYS_N7:
					return '&';
				case KEYS_N8:
					return '*';
				case KEYS_N9:
					return '(';
				}
			}
		}
		else
		{
			if ( pressed_character >= KEYS_A
				 && pressed_character <= KEYS_Z )
				return char( tolower( pressed_character ) );

			if ( pressed_character >= KEYS_N0
				 && pressed_character <= KEYS_N9 )
				return char( pressed_character );
		}
	}
	else if ( is_key_pressed( KEYS_SPACE ) )
	{
		if ( out )
			*out = KEYS_SPACE;

		return ' ';
	}
	else if ( is_key_pressed( KEYS_BACK ) )
	{
		if ( out )
			*out = KEYS_BACK;

		return 0;
	}

	if ( out )
		*out = KEYS_NONE;

	return 0;
}
