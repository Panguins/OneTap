#include "context.h"

decltype( hooked::original::o_paint_traverse ) hooked::original::o_paint_traverse;

void __fastcall hooked::paint_traverse( void* ecx, void* edx, uint32_t panel, bool repaint, bool allow_force )
{
	if ( ctx.m_settings.visuals_no_scope )
	{
		if ( !strcmp( xors( "HudZoom" ), csgo.m_panel( )->name( panel ) ) )
		{
			if ( ctx.m_local && ctx.m_local->weapon( ) )
			{
				if ( ctx.m_local->weapon( )->item_index( ) == WEAPON_AUG || ctx.m_local->weapon( )->item_index( ) == WEAPON_SG556 )
				{
					// if ( v3 != 2 && v3 != 3 && !*(_BYTE *)(v2 + 12869) && g_pGlobals->curtime > *(float *)(v2 + 13008) )
					// CIronSightController::IsInIronSight, ref'd in func with "throwcharge" string
					*reinterpret_cast< float* >( ctx.m_local->weapon( ) + 13008 ) = csgo.m_globals( )->curtime + 1.0f;
				}
			}

			return;
		}
	}

	hooked::original::o_paint_traverse( ecx, edx, panel, repaint, allow_force );
}