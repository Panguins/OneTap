#include "context.h"
#include "legit.h"
// todo:
// time handling, rcs, smoothing, backtracking, pathing work/ humanized aim

decltype( hooked::original::o_override_mouse_input ) hooked::original::o_override_mouse_input;

void __fastcall hooked::override_mouse_input( void* ecx, void* edx, float* x, float* y )
{
	if ( ctx.m_settings.legit_aimbot && ctx.m_settings.legit_enabled )
	{
		if ( csgo.m_engine( )->IsInGame( ) && ctx.m_local && ctx.m_local->alive( ) )
			g_legit.init_override_mouse_input( x, y );
	}

	hooked::original::o_override_mouse_input( ecx, edx, x, y );
}