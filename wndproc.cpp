#include <windows.h>
#include "context.h"

decltype( hooked::original::o_wnd_proc ) hooked::original::o_wnd_proc;
long __stdcall hooked::wnd_proc( HWND hwnd, uint32_t msg, uint32_t uparam, long param )
{
	if ( msg == WM_MOUSEMOVE )
		g_input.capture_mouse_move( param );

	g_input.register_key_press( static_cast< VirtualKeyEvents_t >( msg ), static_cast< VirtualKeys_t >( uparam ) );

	// dont pass mouse input to game when menu is open
	if ( ctx.m_menu_open )
	{
		if ( msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_LBUTTONUP || msg == WM_RBUTTONUP 
	      || msg == WM_LBUTTONDBLCLK || msg == WM_RBUTTONDBLCLK || msg == WM_MOUSEFIRST || msg == WM_MOUSEWHEEL )
		{
			return false;
		}
	}

	return CallWindowProc( hooked::original::o_wnd_proc, hwnd, msg, uparam, param );
}