#include "context.h"
#include "misc.h"

c_misc g_misc;
c_move_fix g_move_fix;

void c_misc::init( )
{
	if ( ctx.m_settings.misc_bhop )
		bunnyhop( );

	if ( ctx.m_settings.misc_auto_strafe )
		//c_strafe::strafe(ctx.m_cmd);
		placeholder_strafe( ctx.m_cmd );

	if ( ctx.m_settings.misc_auto_defuse )
		auto_defuse( );

	static  bool    was_on = false;
	static  bool**  byte_14F85540 = memory::pattern::first_code_match< bool** >( csgo.m_client.module( ), xors( "80 3D ? ? ? ? ? 53 56 57 0F 85" ), 2 );

	if ( ctx.m_settings.visuals_no_post_processing && !was_on )
	{
		/*
		aww yea

			if ( byte_14F85540 )
				return 0;
		*/

		**byte_14F85540 = true;
		was_on = true;
	}

	if ( !ctx.m_settings.visuals_no_post_processing && was_on )
	{
		**byte_14F85540 = false;
		was_on = false;
	}
}

void c_misc::ragdolls( )
{
	// ragdoll gravity
	static const auto cl_ragdoll_gravity = csgo.m_engine_cvars( )->FindVar( xors( "cl_ragdoll_gravity" ) );

	float desired_ragdoll_gravity = ctx.m_settings.misc_ragdoll_gravity ? -100.f : 600.f;
	if ( cl_ragdoll_gravity->get_float( ) != desired_ragdoll_gravity )
	{
		cl_ragdoll_gravity->set_value( desired_ragdoll_gravity );
	}

	// ragdoll force
	if ( ctx.m_settings.misc_ragdoll_force )
	{
		int highest_index = csgo.m_entity_list( )->highest_index( );

		for ( int i = 0; i < highest_index; i++ )
		{
			c_base_entity* ent = csgo.m_entity_list( )->get_entity( i );

			if ( !ent )
				continue;

			if ( ent->client( )->m_class_id == CCSRagdoll )
			{
				ent->force( ) *= 97.9f;
				ent->ragdoll_velocity( ) *= 97.9f;
			}
		}
	}
}

void c_misc::auto_defuse( )
{
	c_base_entity* c4{};

	for ( size_t i = 0; i < csgo.m_entity_list( )->highest_index( ); ++i )
	{
		auto ent = csgo.m_entity_list( )->get_entity( i );

		if ( !ent || ent->dormant( ) )
			continue;

		if ( ent->client( )->m_class_id == CPlantedC4 )
		{
			// we've found planted c4, stop looping until c4 is null again
			c4 = ent; 
			break;
		}
	}

	if ( !c4 || ( ctx.m_settings.misc_auto_defuse_key > KEYS_NONE && !g_input.is_key_pressed( ctx.m_settings.misc_auto_defuse_key  ) ) )
		return;	

	const  float  defuse_time     = ctx.m_local->has_defuser( ) ? 5.0f : 10.0f;
	const  float  explosion_time  = c4->detonation_time( ) - csgo.m_globals( )->curtime;
	const  bool   last_tick       = ctx.m_settings.misc_auto_defuse_last_tick;
	const  vec3   origin          = ctx.m_local->origin( );
	const  vec3   goal            = c4->origin( );

	// somewhere around there, couldnt find official number online
	// so i tested myself, might eb different based on different viewangles etc
	// idk. . .
	if ( origin.dist( goal ) < 62.0f )
	{
		if ( !last_tick )
		{
			math::vector_angles( goal - ctx.m_local->head_position( ), ctx.m_cmd->m_viewangles );
			ctx.m_cmd->m_buttons |= IN_USE;
		}
		else if ( explosion_time <= defuse_time + math::time::to_time( 3 ) + csgo.m_engine( )->GetNetChannelInfo( )->GetLatency( FLOW_OUTGOING ) )
		{
			math::vector_angles( goal - ctx.m_local->head_position( ), ctx.m_cmd->m_viewangles );
			ctx.m_cmd->m_buttons |= IN_USE;
		}
	}
}

void c_misc::bunnyhop( )
{
	if ( ctx.m_cmd->m_buttons & IN_JUMP && g_input.is_key_pressed( KEYS_SPACE ) )
	{
		if ( !( ctx.m_local->flags( ) & FL_ONGROUND ) )
			ctx.m_cmd->m_buttons &= ~IN_JUMP;
	}
}

void c_misc::display_ranks( )
{
	static auto reveal_ranks = reinterpret_cast< bool( __cdecl*)( float* ) >( memory::pattern::first_code_match( csgo.m_client.module( ), xors( "55 8B EC 8B 0D ? ? ? ? 68" ) ) );
}

void c_misc::clantag( )
{
	static auto send_clan_tag = reinterpret_cast< void( __fastcall*)( const char*, const char* ) >( memory::pattern::first_code_match( csgo.m_engine.module( ), xors( "53 56 57 8B DA 8B F9 FF 15" ) ) );
}

float c_misc::c_strafe::get_move_angle( float speed )
{
	auto move_angle = math::rad2deg( asin( 15.f / speed ) );

	if ( !isfinite( move_angle ) || move_angle > 90.f )
		move_angle = 90.f;
	else if ( move_angle < 0.f )
		move_angle = 0.f;

	return move_angle;
}

bool c_misc::c_strafe::get_closest_plane( vec3* plane )
{
	c_game_trace trace;
	c_trace_filter filter( ctx.m_local, TRACE_WORLD_ONLY );
	ray_t ray;

	// should probably use solely uninterpolated data for this calculation
	vec3 start = ctx.m_local->origin( );
	vec3 mins = ctx.m_local->mins( );
	vec3 maxs = ctx.m_local->maxs( );

	vec3 planes;
	int count = 0;

	for ( float step = 0.0f; step <= math::pi * 2.0f; step += math::pi / 10.0f )
	{
		vec3 dst = start;

		dst.x += cos( step ) * 64.0f;
		dst.y += sin( step ) * 64.0f;

		ray.Init( start, dst, mins, maxs );

		csgo.m_engine_trace( )->TraceRay( ray, MASK_PLAYERSOLID, &filter, &trace );

		if ( trace.m_fraction < 1.0f )
		{
			planes += trace.m_plane.normal;
			count++;
		}
	}

	planes /= count;

	if ( planes.z < 0.1f )
	{
		*plane = planes;
		return true;
	}

	return false;
}

bool c_misc::c_strafe::will_hit_obstacle_in_future( float predict_time, float step )
{
	c_game_trace tr;
	c_trace_filter filter( ctx.m_local, TRACE_WORLD_ONLY );
	ray_t ray;
	static const auto sv_gravity = csgo.m_engine_cvars( )->FindVar( xors( "sv_gravity" ) );
	static const auto sv_jump_impulse = csgo.m_engine_cvars( )->FindVar( xors( "sv_jump_impulse" ) );
	const float len = ctx.m_local->velocity( ).length2d( );
	bool ground = ctx.m_local->flags( ) & FL_ONGROUND;
	vec3 dst = ctx.m_local->origin( );
	vec3 velocity = ctx.m_local->velocity( );

	auto predicted_ticks_needed = math::time::to_ticks( predict_time );
	auto velocity_rotation_angle = math::rad2deg( atan2( velocity.y, velocity.x ) );
	auto ticks_done = 0;

	if ( !predicted_ticks_needed )
		return false;

	ray.Init( ctx.m_local->origin( ), dst, ctx.m_local->mins( ), ctx.m_local->maxs( ) );

	while ( true )
	{
		float rotation_angle = velocity_rotation_angle + step;

		velocity = vec3(
		                cos( math::deg2rad( rotation_angle ) ) * len,
		                sin( math::deg2rad( rotation_angle ) ) * len,
		                ground ? sv_jump_impulse->get_float( ) : velocity.z - ( sv_gravity->get_float( ) * csgo.m_globals( )->interval_per_tick )
		               );

		dst += velocity * csgo.m_globals( )->interval_per_tick;

		csgo.m_engine_trace( )->TraceRay( ray, MASK_PLAYERSOLID, &filter, &tr );

		if ( tr.m_fraction != 1.0f && tr.m_plane.normal.z <= 0.9f || tr.m_start_solid || tr.m_all_solid )
			break;

		dst = tr.m_dst;
		dst.z -= 2.f;

		ray.Init( tr.m_dst, dst, ctx.m_local->mins( ), ctx.m_local->maxs( ) );

		csgo.m_engine_trace( )->TraceRay( ray, MASK_PLAYERSOLID, &filter, &tr );

		ground = ( tr.m_fraction < 1.0f || tr.m_all_solid || tr.m_start_solid ) && tr.m_plane.normal.z >= 0.7f;

		if ( ++ticks_done >= predicted_ticks_needed )
			return false;

		velocity_rotation_angle = rotation_angle;
	}

	return true;
}

void c_misc::c_strafe::circle_strafe( c_user_cmd* cmd, float* circle_yaw )
{
	const auto min_step = 2.25f;
	const auto max_step = 5.f;
	auto velocity_2d = ctx.m_local->velocity( ).length2d( );

	auto ideal_strafe = std::clamp( get_move_angle( velocity_2d ) * 2.f, min_step, max_step );
	auto predict_time = std::clamp( 320.f / velocity_2d, 0.35f, 1.f );

	auto step = ideal_strafe;

	while ( true )
	{
		if ( !will_hit_obstacle_in_future( predict_time, step ) || step > max_step )
			break;

		step += 0.2f;
	}

	if ( step > max_step )
	{
		step = ideal_strafe;

		while ( true )
		{
			if ( !will_hit_obstacle_in_future( predict_time, step ) || step < -min_step )
				break;

			step -= 0.2f;
		}

		if ( step < -min_step )
		{
			vec3 plane;
			if ( get_closest_plane( &plane ) )
				step = -math::normalize( *circle_yaw - math::rad2deg( atan2( plane.y, plane.x ) ) ) * 0.1f;
		}
		else
			step -= 0.2f;
	}
	else
		step += 0.2f;

	ctx.m_cmd->m_viewangles.y = *circle_yaw = math::normalize( *circle_yaw + step );
	ctx.m_cmd->m_move_dir[1] = copysign( 450.f, -step );
}

void c_misc::c_strafe::strafe( c_user_cmd* cmd )
{
	static auto switch_key = 1.f;
	static auto circle_yaw = 0.f;
	static auto old_yaw = 0.f;

	auto velocity = ctx.m_local->velocity( );
	velocity.z = 0.f;

	auto speed = velocity.length2d( );
	auto ideal_strafe = get_move_angle( speed );

	switch_key *= -1.f;

	if ( ctx.m_local->move_type( ) == MOVETYPE_WALK && !( ctx.m_local->flags( ) & FL_ONGROUND && !( ctx.m_cmd->m_buttons & IN_JUMP ) ) )
	{
		if ( ctx.m_settings.misc_autostrafe_circle > KEYS_NONE && g_input.is_key_pressed( ctx.m_settings.misc_autostrafe_circle ) )
		{
			circle_strafe( cmd, &circle_yaw );
			return;
		}

		if ( ctx.m_cmd->m_move_dir[0] > 0.f )
			ctx.m_cmd->m_move_dir[0] = 0.f;

		auto yaw_delta = math::normalize( ctx.m_cmd->m_viewangles.y - old_yaw );
		auto absolute_yaw_delta = std::abs( yaw_delta );

		circle_yaw = old_yaw = ctx.m_cmd->m_viewangles.y;

		if ( yaw_delta > 0.f )
			ctx.m_cmd->m_move_dir[1] = -450.f;
		else if ( yaw_delta < 0.f )
			ctx.m_cmd->m_move_dir[1] = 450.f;

		if ( absolute_yaw_delta <= ideal_strafe || absolute_yaw_delta >= 30.f )
		{
			vec3 velocity_angles;
			math::vector_angles( velocity, velocity_angles );

			auto velocity_delta = math::normalize( ctx.m_cmd->m_viewangles.y - velocity_angles.y );
			auto retrack = 2.f * ( ideal_strafe * 2.f );

			if ( velocity_delta <= retrack || speed <= 15.f )
			{
				if ( -retrack <= velocity_delta || speed <= 15.f )
				{
					ctx.m_cmd->m_viewangles.y += ideal_strafe * switch_key;
					ctx.m_cmd->m_move_dir[1] = switch_key * 450.f;
				}
				else
				{
					ctx.m_cmd->m_viewangles.y = velocity_angles.y - retrack;
					ctx.m_cmd->m_move_dir[1] = 450.f;
				}
			}
			else
			{
				ctx.m_cmd->m_viewangles.y = velocity_angles.y + retrack;
				ctx.m_cmd->m_move_dir[1] = -450.f;
			}
		}
	}
}

void c_misc::auto_strafe( ) {}

void c_misc::placeholder_strafe( c_user_cmd* cmd )
{
	static float m_previous_yaw;

	auto get_angle_from_speed = []( float speed )
			{
				auto ideal_angle = math::rad2deg( std::atan2( 30.f, speed ) );
				ideal_angle = std::clamp( ideal_angle, 0.f, 90.f );
				return ideal_angle;
			};

	if ( ctx.m_local->flags( ) & FL_ONGROUND )
		return;

	auto velocity = ctx.m_local->velocity( );
	velocity.z = 0;

	static bool flip = false;
	auto turn_direction_modifier = flip ? 1.f : -1.f;
	flip = !flip;

	if ( cmd->m_move_dir[0] > 0.f )
		cmd->m_move_dir[0] = 0.f;

	auto speed = velocity.length2d( );

	auto ideal_move_angle = math::rad2deg( std::atan2( 15.f, speed ) );
	ideal_move_angle = std::clamp( ideal_move_angle, 0.f, 90.f );

	auto yaw_delta = math::normalize( cmd->m_viewangles.y - m_previous_yaw );
	auto abs_yaw_delta = abs( yaw_delta );

	if ( yaw_delta > 0.f )
		cmd->m_move_dir[1] = -450.f;

	else if ( yaw_delta < 0.f )
		cmd->m_move_dir[1] = 450.f;

	if ( abs_yaw_delta <= ideal_move_angle || abs_yaw_delta >= 30.f )
	{
		vec3 velocity_angles;
		math::vector_angles( velocity, velocity_angles );

		auto velocity_angle_yaw_delta = math::normalize( cmd->m_viewangles.y - velocity_angles.y );
		auto velocity_degree = get_angle_from_speed( speed ) * 2;

		if ( velocity_angle_yaw_delta <= velocity_degree || speed <= 15.f )
		{
			if ( -( velocity_degree ) <= velocity_angle_yaw_delta || speed <= 15.f )
			{
				cmd->m_viewangles.y += ( ideal_move_angle * turn_direction_modifier );
				cmd->m_move_dir[1] = 450.f * turn_direction_modifier;
			}

			else
			{
				cmd->m_viewangles.y = velocity_angles.y - velocity_degree;
				cmd->m_move_dir[1] = 450.f;
			}
		}

		else
		{
			cmd->m_viewangles.y = velocity_angles.y + velocity_degree;
			cmd->m_move_dir[1] = -450.f;
		}
	}

	cmd->m_buttons &= ~(IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK);

	if (cmd->m_move_dir[1] < 0.f)
		cmd->m_buttons |= IN_MOVELEFT;
	else
		cmd->m_buttons |= IN_MOVERIGHT;

	if (cmd->m_move_dir[0] < 0.f)
		cmd->m_buttons |= IN_BACK;
	else
		cmd->m_buttons |= IN_FORWARD;
}
