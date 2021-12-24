#include "context.h"
#include "boner.h"
#include "target_selection.h"

c_aimbot g_aimbot;

void c_aimbot::init( )
{
	if ( !ctx.m_cmd || !ctx.m_local )
		return;

	if ( ctx.m_settings.rage_aimbot_enabled )
		aim( );

	g_antiaim.init( );
}

bool c_aimbot::can_hit_point( const vec3& angle, c_base_entity* ent, bool anywhere_on_ent, uint32_t hitbox )
{
	if ( ent == nullptr || ctx.m_settings.rage_anti_spread || ( !ctx.m_settings.rage_hitchance || ctx.m_settings.rage_hitchance_amt == 0 ) )
		return true;

	c_base_weapon* weapon;
	weapon_info_t* weapon_info;

	if ( ( weapon = ctx.m_local->weapon( ) ) == nullptr || ( weapon_info = weapon->wpn_data( ) ) == nullptr )
		return false;

	weapon->update_accuracy_penalty( );

	vec3 angle_forward, angle_right, angle_up, spread, spread_direction, spread_angle, bullet_end, eye_position = ctx.m_local->eye_origin( );
	auto weapon_spread = weapon->spread( ), weapon_inaccuracy = weapon->inaccuracy( );
	uint32_t points_hit = 0, hits_needed = static_cast< uint32_t >( ceil( 256.f * ( static_cast< float >( ctx.m_settings.rage_hitchance_amt ) / 100.f ) ) );

	angle.normalized( );
	math::angle_vectors( angle, &angle_forward, &angle_right, &angle_up );

	c_game_trace trace;

	for ( uint32_t i = 0; i < 256; i++ )
	{
		using namespace math::imports;

		random_seed( i );

		float rand1 = random( 0.0f, 1.0f );
		float rand2 = random( 0.0f, 2.0f * math::pi );
		float rand3 = random( 0.0f, 1.0f );
		float rand4 = random( 0.0f, 2.0f * math::pi );

		switch ( ctx.m_local->weapon( )->item_index( ) )
		{
			case WEAPON_NEGEV:
			{
				if ( ctx.m_local->weapon( )->recoil_index( ) < 3.0f )
				{
					for ( size_t a = 3; a > ctx.m_local->weapon( )->recoil_index( ); --a )
					{
						rand1 *= rand1;
						rand3 *= rand3;
					}

					rand1 = 1.0f - rand1;
					rand3 = 1.0f - rand3;
				}
				break;
			}

			case WEAPON_REVOLVER:
			{
				rand1 = 1.0f - rand1 * rand1;
				rand3 = 1.0f - rand3 * rand3;
				break;
			}
			default: break;
		}

		spread = vec3(
		              cos( rand2 ) * ( rand1 * weapon_inaccuracy ) + cos( rand4 ) * ( rand3 * weapon_spread ),
		              sin( rand2 ) * ( rand1 * weapon_inaccuracy ) + sin( rand4 ) * ( rand3 * weapon_spread ),
		              0.0f );

		spread_direction = ( angle_forward + angle_right * -spread.x + angle_up * -spread.y ).normalized( );

		math::vector_angles( spread_direction, spread_angle );
		math::angle_vectors( angle - ( spread_angle - angle ), &bullet_end );

		csgo.m_engine_trace( )->ClipRayToEntity( ray_t( eye_position, eye_position + bullet_end * weapon_info->range ), MASK_SHOT, ent, &trace );

		if ( trace.m_ent == ent )
		{
			if ( anywhere_on_ent )
				points_hit += 1;
			else if ( trace.m_hitbox == hitbox )
				points_hit += 1;

			if ( points_hit >= hits_needed )
				return true;
		}
	}

	return false;
}

bool c_aimbot::can_hit_point_estimate( c_base_entity* ent, uint32_t hitbox_index )
{
	if ( ent == nullptr || ctx.m_settings.rage_anti_spread || ( !ctx.m_settings.rage_hitchance || ctx.m_settings.rage_hitchance_amt == 0 ) )
		return true;

	c_base_weapon* weapon;
	studiohdr_t* hdr;
	mstudiohitboxset_t* set;
	mstudiobbox_t* hitbox;

	if ( ( weapon = ctx.m_local->weapon( ) ) == nullptr || ( hdr = csgo.m_model_info( )->GetStudioModel( ent->model( ) ) ) == nullptr
	     || ( set = hdr->pHitboxSet( ent->hitbox_set( ) ) ) == nullptr || ( hitbox = set->pHitbox( hitbox_index ) ) == nullptr )
		return false;

	weapon->update_accuracy_penalty( );

	auto cone_angle = weapon->inaccuracy( ) + weapon->spread( );
	auto target_distance = ctx.m_local->firebullets_position( ).dist( ent->eye_origin( ) );
	auto cone_radius = target_distance * std::tan( cone_angle );

	if ( cone_radius == 0.f )
		return true;

	auto cone_area = math::pi * cone_radius * cone_radius;

	float hitbox_area;

	if ( hitbox->m_Radius == -1.f )
		hitbox_area = ( ( hitbox->m_Maxs - hitbox->m_Mins ).length( ) / 2.f ) * ( ( hitbox->m_Maxs - hitbox->m_Mins ).length( ) / 2.f );
	else
		hitbox_area = math::pi * hitbox->m_Radius * hitbox->m_Radius * 0.8f;

	if ( cone_area < hitbox_area )
		return true;

	return ( sqrtf( hitbox_area / cone_area ) * 100.f ) < ctx.m_settings.rage_hitchance_amt;
}

void c_aimbot::nospread( )
{
	vec3 forward, right, up, spread;

	// this should probably be done in engine prediction
	ctx.m_local->weapon( )->update_accuracy_penalty( );

	const float weapon_inaccuracy = ctx.m_local->weapon( )->inaccuracy( );
	const float weapon_spread = ctx.m_local->weapon( )->spread( );

	math::imports::random_seed( ( ctx.m_cmd->m_random_seed & 255 ) + 1 );

	float rand1 = math::imports::random( 0.0f, 1.0f );
	float rand2 = math::imports::random( 0.0f, 2.0f * math::pi );
	float rand3 = math::imports::random( 0.0f, 1.0f );
	float rand4 = math::imports::random( 0.0f, 2.0f * math::pi );

	switch ( ctx.m_local->weapon( )->item_index( ) )
	{
		case WEAPON_NEGEV:
		{
			if ( ctx.m_local->weapon( )->recoil_index( ) < 3.0f )
			{
				for ( size_t i = 3; i > ctx.m_local->weapon( )->recoil_index( ); --i )
				{
					rand1 *= rand1;
					rand3 *= rand3;
				}

				rand1 = 1.0f - rand1;
				rand3 = 1.0f - rand3;
			}
		}

		case WEAPON_REVOLVER:
		{
			rand1 = 1.0f - rand1 * rand1;
			rand3 = 1.0f - rand3 * rand3;
		}
	}

	spread = vec3(
	              cos( rand2 ) * ( rand1 * weapon_inaccuracy ) + cos( rand4 ) * ( rand3 * weapon_spread ),
	              sin( rand2 ) * ( rand1 * weapon_inaccuracy ) + sin( rand4 ) * ( rand3 * weapon_spread ),
	              0.0f );

	math::angle_vectors( ctx.m_cmd->m_viewangles, &forward, &right, &up );

	vec3 spread_dir = forward + ( right * -spread.x ) + ( up * -spread.y );

	switch ( ctx.m_settings.rage_anti_spread )
	{
		case 1:
		{
			math::vector_angles( spread_dir, ctx.m_cmd->m_viewangles );
			break;
		}

		case 2:
		{
			ctx.m_cmd->m_viewangles.x += math::rad2deg( atan( sqrt( spread.x * spread.x + spread.y * spread.y ) ) );
			ctx.m_cmd->m_viewangles.z = -math::rad2deg( atan2( spread.x, spread.y ) );
			break;
		}
	}
}

void c_aimbot::aim( )
{
	m_aim_point.set( );

	static const auto weapon_recoil_scale = csgo.m_engine_cvars( )->FindVar( xors( "weapon_recoil_scale" ) );
	static const auto weapon_accuracy_nospread = csgo.m_engine_cvars( )->FindVar( xors( "weapon_accuracy_nospread" ) );

	if ( !ctx.m_local->weapon( ) || ( !g_input.is_key_pressed( ctx.m_settings.rage_key ) && ctx.m_settings.rage_key > 0 ) )
		return;

	possible_target_t target;
	if ( !g_target_selector.find_target( target ) )
		return;

	m_target = target.m_data->base;
	m_aim_point = target.m_final_aimpoint;

	// if ( ctx.m_settings.wait_for_unlag && !g_lag_comp.is_target_lag_fixed( m_target ) ) 
	// return;

	if ( ctx.m_local->weapon( )->can_fire( ) )
	{
		/*if ( *reinterpret_cast< bool* >( ctx.m_local + 8316 ) && *reinterpret_cast< int* >( ctx.m_local + 2421 ) ) // animstate ptr i think
		{
			if ( *reinterpret_cast< int* >( ctx.m_local->anim_state( ) + 0x4c ) )
			{
				// idafail is killign me here sorry
				// 55 8B EC 83 E4 F8 83 EC 1C 56 8B F1 8B 56 50 heres a sig for ModifyEyePosition
				// this is completely different on client/ server
				// this is the problem we are missing afte jumping
				// here is kinda reversed func im tired
				// https://hastebin.com/xememicesi.m
			}
		}*/

		math::vector_angles( m_aim_point - ctx.m_local->firebullets_position( ), ctx.m_cmd->m_viewangles );

		if ( !can_hit_point( ctx.m_cmd->m_viewangles, m_target ) )
			return;

		if ( ctx.m_settings.rage_auto_shoot )
		{
			auto do_auto_shoot = []( const bool& condition, std::function< void( ) > precursor = nullptr )
					{
						if ( precursor )
							precursor( );

						if ( condition )
						{
							ctx.m_cmd->m_buttons &= ~IN_USE;
							ctx.m_cmd->m_buttons |= IN_ATTACK;

							if ( !ctx.m_local->weapon( )->clip1( ) )
							{
								ctx.m_cmd->m_buttons &= ~IN_ATTACK;
								ctx.m_cmd->m_buttons |= IN_RELOAD;
							}
						}
					};

			switch ( ctx.m_settings.rage_auto_shoot_mode )
			{
				case 0:
				{
					do_auto_shoot( true );
					break;
				}

				case 1:
				{
					do_auto_shoot( ctx.m_lag_handler->get_choked_ticks( ) > 0, []
					               {
						               ctx.m_lag_handler->reset( );
					               } );
					break;
				}

				case 2:
				{
					do_auto_shoot( true, []
					               {
						               ctx.m_lag_handler->choke( );
					               } );
					break;
				}

				case 3:
				{
					do_auto_shoot( !ctx.m_is_anim_tick );
					break;
				}
			}
		}

		if ( !ctx.m_settings.rage_silent )
			csgo.m_engine( )->SetViewAngles( vec3( ctx.m_cmd->m_viewangles.x, ctx.m_cmd->m_viewangles.y, 0.0f ) );

		if ( ctx.m_settings.rage_anti_spread && !weapon_accuracy_nospread->get_int( ) )
			nospread( );

		if ( ctx.m_settings.rage_anti_recoil )
			ctx.m_cmd->m_viewangles -= ctx.m_local->punch( ) * weapon_recoil_scale->get_float( );

		ctx.m_cmd->m_viewangles.normalize( );
	}

	if ( ctx.m_cmd->m_buttons & IN_ATTACK )
	{
		static bool toggle{ };

		if ( ctx.m_local->weapon( )->type( ) == pistol )
			toggle ? ctx.m_cmd->m_buttons |= IN_ATTACK : ctx.m_cmd->m_buttons &= ~IN_ATTACK;

		toggle = !toggle;
	}

	m_target = nullptr;
}
