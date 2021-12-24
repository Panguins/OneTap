#include "context.h"

decltype( hooked::original::o_should_draw_fog ) hooked::original::o_should_draw_fog;

bool __fastcall hooked::should_draw_fog( void* ecx, void* edx )
{	
	if ( ctx.m_settings.visuals_no_fog )
		return false;

	return hooked::original::o_should_draw_fog( ecx, edx );
}