#include "context.h"
#include "antiaim.h"
#include "autowall.h"
#include "target_selection.h"
#include "anims.h"

c_anti_aim g_antiaim;

bool c_anti_aim::should_run( )
{
	// also beginning of round check

	if ( !ctx.m_settings.rage_aimbot_enabled || !ctx.m_settings.rage_anti_aim )
		return false;

	if ( !ctx.m_local || !ctx.m_local->alive( ) )
		return false;

	if ( ctx.m_cmd->m_buttons & IN_ATTACK && ( ctx.m_local->weapon( ) && ctx.m_local->weapon( )->can_fire( ) ) )
		return false;

	if ( ctx.m_local->move_type( ) == MOVETYPE_NOCLIP || ctx.m_local->flags( ) & FL_WATERJUMP || ctx.m_cmd->m_buttons & IN_USE )
		return false;

	if ( ctx.m_local->weapon( ) && ctx.m_local->weapon( )->type( ) == weapon_t::grenade && ctx.m_local->weapon( )->throw_time( ) > 0.f )
		return false;

	// until we have ladder fix
	if ( ctx.m_local->move_type( ) == MOVETYPE_LADDER )
		return false;

	return true;
}

float c_anti_aim::get_ideal_pitch( )
{
	constexpr   float   emotion  = 89.0f;
	constexpr   float   down     = 178.0f;
	static      bool    init     = true;

	static float fakedown, fakeup, zero;

	if ( init )
	{
		// 540/ -540
		calculate_smallest_fake_angle( -180.0f, fakedown );
		calculate_smallest_fake_angle( 180.0f, fakeup );

		// 720
		calculate_smallest_fake_angle( 0.0f, zero );

		init = false;
	}

	// ugh
	switch ( ctx.m_settings.rage_pitch )
	{
	default:
		return ctx.m_cmd->m_viewangles.x;

	case antiaim_pitch_t::emotion:
		return emotion;

	case antiaim_pitch_t::down:
		return down;

	case antiaim_pitch_t::fakedown:
		return fakedown;

	case antiaim_pitch_t::fakeup:
		return fakeup;

	case antiaim_pitch_t::zero:
		return zero;

	case antiaim_pitch_t::fake_nigga:
		return 0.0f;
	}
}

float c_anti_aim::get_ideal_yaw( )
{
	float freestanding_yaw = std::numeric_limits< float >::max( );

	const  bool  fake     = !ctx.m_lag_handler->choking( );
	const  int   setting  = fake ? ctx.m_settings.rage_fake_yaw : ctx.m_settings.rage_yaw;

	// backwards through switch, freestanding is handled manually
	const std::vector< float > yaws = {
		0,
		172.0f,
		std::fmod( csgo.m_globals( )->curtime * ctx.m_settings.rage_yaw_spin_speed, 360.0f ),
		90.0f,
		ctx.m_cmd->m_cmd_number % 2 == 0 ? 120.0f : -120.0f,
		ctx.m_settings.rage_yaw_switch_offset - std::fmod( csgo.m_globals( )->curtime * ctx.m_settings.rage_yaw_switch_speed, ctx.m_settings.rage_yaw_switch_range ),
	};

	auto to_target = [&]( const float& offset, const bool& run = ctx.m_settings.rage_at_targets )
	{
		if ( !run )
			return ctx.m_cmd->m_viewangles.y - offset;

		vec3 aim_angle = ctx.m_cmd->m_viewangles;

		int target_id = g_target_selector.get_highest_threat( );
		if ( target_id )
		{
			auto target = csgo.m_entity_list( )->get_entity( target_id );
			aim_angle = math::_vector_angles( ctx.m_local->origin( ), target->origin( ) );

			if ( target->shots_fired( ) > 0 )			
				aim_angle.y += 45.0f * target->shots_fired( );			
		}
		else
		{
			aim_angle.y += 180.f;
		}

		return aim_angle.y - offset;
	};

	if ( ctx.m_settings.rage_yaw != 6 )
	{
		// we're not freestanding, so run aa as normal

		if ( ctx.m_settings.rage_fake_yaw == ctx.m_settings.rage_yaw && fake )		
			return math::normalize( -to_target( yaws.at( setting ) ) );		

		return math::normalize( to_target( yaws.at( setting ) ) );
	}

	// user has selected freestanding
	if ( !fake )
	{
		if ( get_freestand_yaw( &freestanding_yaw ) > 0 )
		{
			// freestanding angle will
			// make it harder for enemies
			// to actually hiit us, so use it
			m_is_freestanding = true;
			return freestanding_yaw;
		}

		// freestanding either won't help
		// or no enemies can even hit us
		// so just do backwards, why not B)
		return to_target( 140.0f + std::fmod( csgo.m_globals( )->curtime * 150.f, 97.9f ), true );
	}

	// fake angle, doesn't matter /shrug 
	return math::normalize( to_target( yaws.at( setting ) ) );
}

void c_anti_aim::handle_lby( )
{
	bool   is_breaking;
	bool   was_breaking;
	float  break_yaw;

	if ( g_anims.local_data( )->is_lby_update( ) )
	{

	}
}

void c_anti_aim::init( )
{
	if ( !should_run( ) )
		return;

	ctx.m_cmd->m_viewangles.x  = get_ideal_pitch( );
	ctx.m_cmd->m_viewangles.y  = get_ideal_yaw( );
	m_is_freestanding          = false;
}

bool c_anti_aim::is_safe_angle( float yaw )
{
	if ( !ctx.m_local || !ctx.m_local->alive( ) )
		return true;

	int threat_id = g_target_selector.get_highest_threat( );
	if ( !threat_id )
		return true;

	c_base_entity* ent = csgo.m_entity_list( )->get_entity( threat_id );
	if ( !ent )
		return true;

	float rad = math::deg2rad( yaw );
	vec3 head_pos_flick = ctx.m_local->eye_origin( );

	head_pos_flick.x += cos( rad ) * 25.f;
	head_pos_flick.y += sin( rad ) * 25.f;

	float flick_damage = 0.f;
	g_auto_wall.get_damage( ent, ctx.m_local, head_pos_flick, g_auto_wall.get_awp( ) );

	if ( flick_damage < 10.f )
		return true;

	return false;
}

//-1 = no enemies can hit us
//0 = enemies can hit both sides, or no enemies
//1 = we found a safe hiding spot for head
int c_anti_aim::get_freestand_yaw( float* yaw )
{
	if ( !ctx.m_local )
		return 0;

	int threat_id = g_target_selector.get_highest_threat( );
	if ( !threat_id )
		return 0;

	c_base_entity* ent = csgo.m_entity_list( )->get_entity( threat_id );
	if ( !ent )
		return 0;

	vec3 target_position = ent->origin( ) + vec3( 0.f, 0.f, 64.f );
	vec3 eye_pos = ctx.m_local->eye_origin( );
	vec3 angle;
	math::vector_angles( target_position - eye_pos, angle );

	static auto get_rotated_pos = []( vec3 start, float rotation, float distance )
	{
		float rad = math::deg2rad( rotation );
		start.x += cos( rad ) * distance;
		start.y += sin( rad ) * distance;

		return start;
	};

	static auto get_rotated_damage = []( c_base_entity* shooter, vec3 shooter_position, vec3 local_position, vec3 angle, float* dmg_left, float* dmg_right )
	{
		*dmg_left = 0.f;
		*dmg_right = 0.f;

		weapon_info_t* freestand_rifle = g_auto_wall.get_awp( );
		bool first_run = true;

		for ( float dist = 20.f; dist < 150.f; dist += 30.f )
		{
			vec3 pos_left = get_rotated_pos( local_position, angle.y + 90.f, dist );
			vec3 pos_right = get_rotated_pos( local_position, angle.y + 90.f, -dist );

			c_trace_filter filter( ctx.m_local );
			c_game_trace tr;
			ray_t ray;

			ray.Init( local_position, pos_left );
			csgo.m_engine_trace( )->TraceRay( ray, MASK_SHOT, &filter, &tr );
			if ( tr.m_fraction <= 0.98f )
				break;

			tr = c_game_trace( );
			ray = ray_t{ };

			ray.Init( local_position, pos_right );
			csgo.m_engine_trace( )->TraceRay( ray, MASK_SHOT, &filter, &tr );
			if ( tr.m_fraction < 0.98f )
				break;

			float temp_dmg_left = g_auto_wall.get_damage( shooter, ctx.m_local, pos_left, freestand_rifle, &shooter_position );
			float temp_dmg_right = g_auto_wall.get_damage( shooter, ctx.m_local, pos_right, freestand_rifle, &shooter_position );

			if ( first_run && temp_dmg_left && temp_dmg_right )
				return false; //enemies can hit both sides

			*dmg_left += temp_dmg_left;
			*dmg_right += temp_dmg_right;

			first_run = false;
		}

		return true;
	};

	float dmg_left_total{ };
	float dmg_right_total{ };

	if ( !get_rotated_damage( ent, target_position, eye_pos, angle, &dmg_left_total, &dmg_right_total ) )
		return 0; //enemies can hit both side

				  //lets see if we can hide our head
	if ( ( std::max( dmg_left_total, dmg_right_total ) - std::min( dmg_left_total, dmg_right_total ) ) > 50.f )
	{
		*yaw = dmg_left_total > dmg_right_total ? angle.y - 90.f : angle.y + 90.f;
		return 1;
	}

	//run predicted freestand
	int counter_left = 0;
	int counter_right = 0;

	//couldnt hide head and we are not hittable by target, predict target to left/right to see if there is a possible position to hit
	for ( float dist = 20.f; dist < 150.f; dist += 30.f )
	{
		dmg_left_total = 0.f;
		dmg_right_total = 0.f;

		vec3 target_position_left = get_rotated_pos( target_position, angle.y + 90.f, dist );
		vec3 target_position_right = get_rotated_pos( target_position, angle.y + 90.f, -dist );

		if ( !get_rotated_damage( ent, target_position_left, eye_pos, angle, &dmg_left_total, &dmg_right_total ) )
			return 0; //enemies can hit both side

					  //lets see if we can hide our head
		if ( ( std::max( dmg_left_total, dmg_right_total ) - std::min( dmg_left_total, dmg_right_total ) ) > 50.f )
		{
			if ( !counter_left )
			{
				counter_left++;
			}
			else
			{
				*yaw = dmg_left_total > dmg_right_total ? angle.y - 90.f : angle.y + 90.f;
				return 1;
			}
		}

		if ( !get_rotated_damage( ent, target_position_right, eye_pos, angle, &dmg_left_total, &dmg_right_total ) )
			return 0; //enemies can hit both side

					  //lets see if we can hide our head
		if ( ( std::max( dmg_left_total, dmg_right_total ) - std::min( dmg_left_total, dmg_right_total ) ) > 40.f )
		{
			if ( !counter_right )
			{
				counter_right++;
			}
			else
			{
				*yaw = dmg_left_total > dmg_right_total ? angle.y - 90.f : angle.y + 90.f;
				return 1;
			}
		}
	}

	return -1;
}