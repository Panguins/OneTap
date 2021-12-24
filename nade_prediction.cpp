#include "nade_prediction.h"
#include "context.h"

c_grenade_predictor g_grenade_predictor;

void c_grenade_predictor::create_move( int buttons )
{
	grenade_path.clear( );
	grenade_bounce.clear( );

	type = 0;
	act = ACT_NONE;

	if ( !ctx.m_local || !ctx.m_local->alive( ) )
		return;

	if ( !ctx.m_settings.visuals_active || !ctx.m_settings.visuals_nade_prediction )
		return;

	bool in_attack = ( buttons & IN_ATTACK ) != 0;
	bool in_attack2 = ( buttons & IN_ATTACK2 ) != 0;

	act = in_attack && in_attack2 ? ACT_LOB :
		in_attack2 ? ACT_DROP :
		in_attack ? ACT_THROW :
		ACT_NONE;

	c_base_weapon* weapon = ctx.m_local->weapon( );
	type = weapon && weapon->type( ) == weapon_t::grenade ? weapon->item_index( ) : WEAPON_NONE;

	if ( !act || !type )
		return;

	simulate( );
}

void c_grenade_predictor::draw( )
{
	if ( !ctx.m_local || !ctx.m_local->alive( ) )
		return;

	if ( type && grenade_path.size( ) > 1 )
	{
		float radius;

		switch ( type )
		{
		case WEAPON_SMOKEGRENADE:
		case WEAPON_DECOY:
		case WEAPON_MOLOTOV:
		case WEAPON_INCGRENADE:
		case WEAPON_HEGRENADE:
			radius = 144.f;
			break;
		default:
			radius = 24.f;
		}

		static float rot_start = 0.f;
		rot_start += 0.01f;

		if ( rot_start >= 1.f )
			rot_start = 0.f;

		drawing::circle3d( grenade_path.back( ), color_t( 255, 0, 0, 255 ), 88, radius, true, rot_start, 0.25f, 0.25f );

		vec3 prev = *grenade_path.begin( );
		for ( auto& cur : grenade_path )
		{
			vec2 parent, child;

			if ( drawing::w2s( prev, parent ) && drawing::w2s( cur, child ) )
			{
				ctx.m_renderer->line( color_t( 255, 255, 255, 200 ), parent.x, parent.y, child.x, child.y );
			}

			prev = cur;
		}

		for ( auto& bounce : grenade_bounce )
		{
			vec2 screen;

			if ( drawing::w2s( bounce, screen ) )
			{
				ctx.m_renderer->filled_rect( color_t( 0, 0, 0, 200 ), screen.x - 2.f, screen.y - 2.f, screen.x + 2.f, screen.y + 2.f );
				ctx.m_renderer->filled_rect( color_t( 255, 0, 0, 200 ), screen.x - 1.f, screen.y - 1.f, screen.x + 1.f, screen.y + 1.f );
			}
		}
	}
}

void c_grenade_predictor::simulate( )
{
	vec3 start, throw_speed;
	setup( start, throw_speed );

	for ( size_t i = 0; i < grenade_path.max_size( ) - 1; i++ )
	{
		grenade_path.push_back( start );

		// 0 = end, 1 = air, 2 = bounce
		int step_result = step( start, throw_speed, i, csgo.m_globals( )->interval_per_tick );
		if ( !step_result )
			break;

		if ( step_result == 2 )
			grenade_bounce.push_back( start );
	}
	grenade_path.push_back( start );
}

void c_grenade_predictor::setup( vec3& source, vec3& throw_speed )
{
	vec3 throw_angle = csgo.m_engine( )->GetViewAngles( );
	float pitch = throw_angle.x;

	if ( pitch <= 90.0f )
	{
		if ( pitch < -90.0f )
		{
			pitch += 360.0f;
		}
	}
	else
	{
		pitch -= 360.0f;
	}

	float a = pitch - ( 90.0f - fabs( pitch ) ) * 10.0f / 90.0f;
	throw_angle.x = a;

	float velocity = 750.0f * 0.9f;

	static constexpr float power[] = { 1.0f, 1.0f, 0.5f, 0.0f };
	float b = power[act];
	b = b * 0.7f;
	b = b + 0.3f;
	velocity *= b;

	vec3 forward;
	vec3 right;
	vec3 up;
	math::angle_vectors( throw_angle, &forward, &right, &up );

	source = ctx.m_local->eye_origin( );
	float off = power[act] * 12.0f - 12.0f;
	source.z += off;

	c_game_trace tr;
	vec3 destination = source + ( forward * 22.f );
	trace_hull( source, destination, tr );

	vec3 back = forward * 6.0f;
	source = tr.m_dst;
	source -= back;

	throw_speed = ctx.m_local->velocity( );
	throw_speed *= 1.25f;
	throw_speed += forward * velocity;
}

int c_grenade_predictor::step( vec3& source, vec3& throw_speed, int tick, float interval )
{
	vec3 move;
	add_gravity_move( move, throw_speed, interval, false );

	vec3 destination = source + move;

	c_game_trace tr;
	trace_hull( source, destination, tr );

	int result = 1;

	if ( check_detonate( throw_speed, tr, tick, interval ) )
	{
		// hit the end
		source = tr.m_dst;
		return 0;
	}

	if ( tr.m_fraction != 1.0f )
	{
		// hit something
		resolve_fly_collision( tr, throw_speed, interval );

		// if we hit an entity, just call it the end
		if ( tr.m_ent && tr.m_ent->client( )->m_class_id == CCSPlayer )
			return 0;

		result = 2;
	}

	source = tr.m_dst;

	return result;
}

void c_grenade_predictor::trace_hull( vec3& src, vec3& end, c_game_trace& tr )
{
	ray_t ray;
	ray.Init( src, end, vec3( -2.0f, -2.0f, -2.0f ), vec3( 2.0f, 2.0f, 2.0f ) );

	c_trace_filter filter( ctx.m_local );

	csgo.m_engine_trace( )->TraceRay( ray, 0x200400B, &filter, &tr );
}

void c_grenade_predictor::add_gravity_move( vec3& move, vec3& vel, float frametime, bool onground )
{
	vec3 basevel( 0.0f, 0.0f, 0.0f );

	move.x = ( vel.x + basevel.x ) * frametime;
	move.y = ( vel.y + basevel.y ) * frametime;

	if ( onground )
	{
		move.z = ( vel.z + basevel.z ) * frametime;
	}
	else
	{
		float gravity = 800.0f * 0.4f;

		float newZ = vel.z - gravity * frametime;
		move.z = ( ( vel.z + newZ ) / 2.0f + basevel.z ) * frametime;

		vel.z = newZ;
	}
}

bool c_grenade_predictor::check_detonate( vec3& throw_speed, c_game_trace& tr, int tick, float interval )
{
	switch ( type )
	{
	case WEAPON_SMOKEGRENADE:
	case WEAPON_DECOY:
	{
		float velocity = throw_speed.length2d( );
		if ( velocity < 0.1f )
		{
			int det_tick_mod = static_cast<int>( 0.2f / interval );
			return !( tick % det_tick_mod );
		}
		return false;
	}
	case WEAPON_MOLOTOV:
	case WEAPON_INCGRENADE:
		if ( tr.m_fraction != 1.0f && tr.m_plane.normal.z > 0.7f )
			return true;
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
		return static_cast<float>( tick ) * interval > 1.5f && !( tick % static_cast<int>( 0.2f / interval ) );

	default:
		return false;
	}
}

void c_grenade_predictor::resolve_fly_collision( c_game_trace& tr, vec3& velocity, float interval )
{
	// Calculate elasticity
	float flSurfaceElasticity = 1.0; // Assume all surfaces have the same elasticity
	float flGrenadeElasticity = 0.45f;
	float flTotalElasticity = flGrenadeElasticity * flSurfaceElasticity;

	if ( flTotalElasticity > 0.9f )
		flTotalElasticity = 0.9f;
	if ( flTotalElasticity < 0.0f )
		flTotalElasticity = 0.0f;

	// Calculate bounce
	vec3 vecAbsVelocity;
	physics_clip_velocity( velocity, tr.m_plane.normal, vecAbsVelocity, 2.0f );
	vecAbsVelocity *= flTotalElasticity;

	// Stop completely once we move too slow
	float flSpeedSqr = vecAbsVelocity.length_sqr( );
	static const float flMinSpeedSqr = 20.0f * 20.0f;
	if ( flSpeedSqr < flMinSpeedSqr )
	{
		vecAbsVelocity.x = 0.0f;
		vecAbsVelocity.y = 0.0f;
		vecAbsVelocity.z = 0.0f;
	}

	// Stop if on ground
	if ( tr.m_plane.normal.z > 0.7f )
	{
		velocity = vecAbsVelocity;
		vecAbsVelocity *= ( 1.0f - tr.m_fraction ) * interval;

		vec3 end = tr.m_dst + vecAbsVelocity;

		trace_hull( tr.m_dst, end, tr );
	}
	else
	{
		velocity = vecAbsVelocity;
	}
}

int c_grenade_predictor::physics_clip_velocity( const vec3& in, const vec3& normal, vec3& out, float overbounce )
{
	static const float STOP_EPSILON = 0.1f;

	float backoff;
	float change;
	float angle;
	int i, blocked;

	blocked = 0;

	angle = normal[2];

	if ( angle > 0 )
	{
		blocked |= 1; // floor
	}
	if ( !angle )
	{
		blocked |= 2; // step
	}

	backoff = in.dot( normal ) * overbounce;

	for ( i = 0; i < 3; i++ )
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
		if ( out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON ) { out[i] = 0; }
	}

	return blocked;
}