#include "context.h"
#include <future>

decltype( hooked::original::o_in_prediction ) hooked::original::o_in_prediction;

void __fastcall hooked::in_prediction( void* ecx, void* edx )
{
	if ( !ctx.m_local )
		return hooked::original::o_in_prediction( ecx, nullptr );

	static uintptr_t CalcPlayerView = memory::pattern::first_code_match( csgo.m_client.module( ), xors( "84 C0 75 08 57 8B CE E8 ? ? ? ? 8B 06" ) );

	if ( uintptr_t( _ReturnAddress( ) ) == CalcPlayerView )
	{
		if ( ctx.m_settings.visuals_no_recoil )
		{
			static  const  auto  weapon_recoil_scale   = csgo.m_engine_cvars( )->FindVar( xors( "weapon_recoil_scale" ) );
			static  const  auto  view_recoil_tracking  = csgo.m_engine_cvars( )->FindVar( xors( "view_recoil_tracking" ) );

			float* eyeAngles;

			/*
				inprediction_hook+2F   89 1C 24                          mov     [esp+0Ch+var_C], ebx ; eyeAngles
				inprediction_hook+32   89 74 24 08                       mov     [esp+0Ch+var_4], esi ; local
			*/
			__asm
			{
				mov   ctx.m_local, esi
				mov   eyeAngles, ebx
			}
			
			eyeAngles[0] -= ctx.m_local->view_punch( ).x + ( ctx.m_local->punch( ).x * weapon_recoil_scale->get_float( ) * view_recoil_tracking->get_float( ) );
			eyeAngles[1] -= ctx.m_local->view_punch( ).y + ( ctx.m_local->punch( ).y * weapon_recoil_scale->get_float( ) * view_recoil_tracking->get_float( ) );
			eyeAngles[2] -= ctx.m_local->view_punch( ).z + ( ctx.m_local->punch( ).z * weapon_recoil_scale->get_float( ) * view_recoil_tracking->get_float( ) );
		}

		for ( int i = 1; i < 65; i++ )
		{
			c_base_entity* ent = csgo.m_entity_list( )->get_entity( i );

			if ( !ent )
				continue;

			if ( ent == ctx.m_local && !csgo.m_input( )->m_fCameraInThirdPerson )
				continue;

			//by setting last bone setup time to float NaN here, all setupbones calls (even from game) will break
			//only way to setupbones then is to invalidate bone cache before running setupbones
			//this boosts performance but also make visuals a bit more polished as u dont see any fighting happening between cheat and game

			if ( !ctx.m_panic && !ent->dormant( ) && ent->alive( ) )
				*(uint32_t*)( uintptr_t( ent ) + 0x2914 ) = 0x7FC00000;
			else if ( *(uint32_t*)( uintptr_t( ent ) + 0x2914 ) == 0x7FC00000 )
				ent->invalidate_bone_cache( );
		}
	}

	return hooked::original::o_in_prediction( ecx, nullptr );
}