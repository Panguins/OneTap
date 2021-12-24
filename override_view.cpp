#include "context.h"

decltype( hooked::original::o_override_view ) hooked::original::o_override_view;

void __fastcall hooked::override_view( void* ecx, void* edx, c_view_setup* view )
{
	if ( !csgo.m_engine( )->IsInGame( ) || !ctx.m_local )
		return hooked::original::o_override_view( ecx, edx, view );

	if ( !ctx.m_local->in_scope( ) || ctx.m_settings.visuals_view_fov_scope )
		view->fov += ctx.m_settings.visuals_view_fov;

	if ( ctx.m_settings.visuals_third_person > KEYS_NONE )
	{
		auto set_thirdperson_state = []( const bool& state, const bool& alive )
		{
			if ( alive )
			{
				static vec3 player_view;
				csgo.m_engine( )->GetViewAngles( player_view );
				csgo.m_input( )->m_fCameraInThirdPerson  = state;
				csgo.m_input( )->m_vecCameraOffset       = state ? vec3( player_view.x, player_view.y, 150.f ) : vec3( player_view.x, player_view.y, player_view.z );				
			}
			else
			{
				csgo.m_input( )->m_fCameraInThirdPerson = false;
				static const uint32_t offset = g_netvars.get_netvar( fnv( "DT_BasePlayer" ), fnv( "m_iObserverMode" ) );

				*reinterpret_cast< int* >( ( uintptr_t ) ctx.m_local + offset ) = state ? OBS_MODE_CHASE : OBS_MODE_IN_EYE;
			}
		};

		static bool on           = false;
		static bool is_clicking  = false;

		// toggle
		{
			if ( g_input.is_key_pressed( ctx.m_settings.visuals_third_person ) )
			{
				if ( !is_clicking )
					on = !on;
				is_clicking = true;
			}
			else
				is_clicking = false;
		}

		// update thirdperson state
		set_thirdperson_state( on, ctx.m_local->alive( ) );

		if ( ctx.m_local->alive( ) && on )
		{
			constexpr   float   cam_hull_offset{ 16.0f };
			const       vec3    cam_hull_min( -cam_hull_offset, -cam_hull_offset, -cam_hull_offset );
			const       vec3    cam_hull_max( cam_hull_offset, cam_hull_offset, cam_hull_offset );
			vec3                cam_forward, origin = ctx.m_local->head_position( true );
			c_game_trace        tr;
			c_trace_filter      filter( ctx.m_local );
			ray_t               ray;

			math::angle_vectors( vec3( csgo.m_input( )->m_vecCameraOffset.x, csgo.m_input( )->m_vecCameraOffset.y, 0.0f ), &cam_forward );

			ray.Init( origin, origin - ( cam_forward * csgo.m_input( )->m_vecCameraOffset.z ), cam_hull_min, cam_hull_max );

			csgo.m_engine_trace( )->TraceRay( ray, MASK_SOLID & ~CONTENTS_MONSTER, &filter, &tr );

			if ( tr.m_fraction < 1.0f )
				csgo.m_input( )->m_vecCameraOffset.z *= tr.m_fraction;
		}
	}

	hooked::original::o_override_view( ecx, edx, view );

	ctx.m_camera_pos = view->origin;
	ctx.m_camera_ang = view->angles;
}