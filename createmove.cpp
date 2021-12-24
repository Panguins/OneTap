#include "context.h"
#include "misc.h"
#include "nade_prediction.h"
#include "legit.h"

#include <intrin.h>
#include "lagcomp.h"
#include "lagcomp_legit.h"

decltype( hooked::original::o_create_move ) hooked::original::o_create_move;

bool __fastcall hooked::create_move( void* ecx, void* edx, float isf, c_user_cmd* cmd )
{
	if ( !cmd->m_cmd_number || !ctx.m_local )	
		return hooked::original::o_create_move( ecx, edx, isf, cmd );	

	if ( hooked::original::o_create_move( ecx, edx, isf, cmd ) )
		csgo.m_engine( )->SetViewAngles( cmd->m_viewangles );

	ctx.m_cmd = cmd;

	if ( !ctx.m_lag_handler )	
		ctx.m_lag_handler = std::make_unique< c_lag_handler >( *reinterpret_cast< bool** >( ( uintptr_t ) _AddressOfReturnAddress( ) - sizeof( uintptr_t ) ) - 0x1c );	

	if ( ctx.m_settings.visuals_nade_prediction )
		g_grenade_predictor.create_move( cmd->m_buttons );

	if ( ctx.m_local->alive( ) )
	{
		// fix up tickcount when interp is disabled
		if ( ctx.m_settings.rage_aimbot_enabled )
			ctx.m_cmd->m_tickcount += math::time::to_ticks( c_lagcomp::get_interpolation( ) );

		g_move_fix.start( ctx.m_cmd );
		{
			ctx.m_lag_handler->init( ctx.m_cmd, ctx.m_local->origin( ), ctx.m_local->old_origin( ) );

			// prediction predicts things
			g_prediction.start( );

			//g_lagcomp.init( );
			g_legit.init_create_move( );
			g_lagcomp_legit.init( );
			g_aimbot.init( );
		}

		g_move_fix.finish( ctx.m_cmd );
		g_misc.init( );

		if ( ctx.m_settings.hack_restrictions != 0 )
		{
			ctx.m_cmd->m_viewangles.sanitize( );

			ctx.m_cmd->m_move_dir[0] = std::clamp<float>( ctx.m_cmd->m_move_dir[0], -450.f, 450.f );
			ctx.m_cmd->m_move_dir[1] = std::clamp<float>( ctx.m_cmd->m_move_dir[1], -450.f, 450.f );
		}


		// if anim tick, server will use these angles for hitboxes
		if ( ctx.m_is_anim_tick )
		{
			ctx.m_real_angle = vec3( ctx.m_settings.rage_pitch > 0 ? g_antiaim.calculate_real_angle( ctx.m_cmd->m_viewangles.x ) : ctx.m_cmd->m_viewangles.x, ctx.m_cmd->m_viewangles.y, 0.0f );

			// setup data for animation update
			g_anims.local_data( )->m_should_update_anims = true;
			g_anims.local_data( )->m_is_fakewalking = false; //set this pls
			g_anims.local_data( )->m_anim_update_tickbase = ctx.m_local->tickbase( );
			g_anims.local_data( )->m_real_angle = ctx.m_real_angle;
			g_anims.local_data( )->m_anim_origin = ctx.m_local->origin( );
		}

		// for use in the next createmove call
		ctx.m_is_anim_tick = !ctx.m_lag_handler->choking( );
	}

	ctx.m_last_cmd = ctx.m_cmd;
	return false;
}
