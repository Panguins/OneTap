#include "context.h"

decltype( hooked::original::o_override_cfg ) hooked::original::o_override_cfg;

bool __fastcall hooked::override_cfg( void* ecx, void* edx, material_config_t* cfg, bool update )
{
	switch ( ctx.m_settings.visuals_render_mode )
	{
	case 1:
		cfg->m_nFullbright = true;
		break;
	case 2:
		cfg->m_bShowLowResImage = true;
		break;
	}

	return hooked::original::o_override_cfg( ecx, edx, cfg, update );
}