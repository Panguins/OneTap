#include "context.h"

decltype( hooked::original::o_on_screen_size_changed ) hooked::original::o_on_screen_size_changed;

void __fastcall hooked::on_screen_size_changed( void* ecx, void* edx, int old_w, int old_h )
{
	hooked::original::o_on_screen_size_changed( ecx, edx, old_w, old_h );

	csgo.m_engine( )->GetScreenSize( ctx.m_screen_w, ctx.m_screen_h );
}