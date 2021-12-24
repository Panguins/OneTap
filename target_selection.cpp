#include "target_selection.h"
#include "context.h"
#include "autowall.h"
#include "playerlist.h"
#include "boner.h"
#include "resolver.h"

c_target_selector g_target_selector;

bool c_target_selector::is_hitbox_checked( int index, int hitbox )
{
	auto data = g_playerlist.at( index );

	if ( !data->is_cheater )
		return hitbox == HITBOX_HEAD || hitbox == HITBOX_PELVIS;

	has_remaining_shots[index] = ( ctx.m_missed_shots[index] < ctx.m_settings.rage_max_misses );

	auto weapon = ctx.m_local->weapon( );
	if ( weapon && weapon->item_index( ) == WEAPON_TASER )
		has_remaining_shots[index] = false;

	if ( ctx.m_settings.rage_baim_fakewalk && data->is_fakewalking )
		has_remaining_shots[index] = false;

	if ( !has_remaining_shots[index] )
	{
		switch ( hitbox )
		{
			case HITBOX_PELVIS:
			case HITBOX_CHEST:
			case HITBOX_THORAX:
				return true;
			default:
				return false;
		}
	}

	switch ( hitbox )
	{
		case HITBOX_HEAD:
		case HITBOX_PELVIS:
		case HITBOX_BODY:
		case HITBOX_THORAX:
		case HITBOX_UPPER_CHEST:
		case HITBOX_RIGHT_UPPER_ARM:
		case HITBOX_LEFT_UPPER_ARM:
		case HITBOX_RIGHT_HAND:
		case HITBOX_LEFT_HAND:
		case HITBOX_RIGHT_THIGH:
		case HITBOX_LEFT_THIGH:
		case HITBOX_RIGHT_FOOT:
		case HITBOX_LEFT_FOOT:
			return true;
		default:
			return false;
	}
}

bool c_target_selector::find_normal_target( possible_target_t& target )
{
	possible_targets.clear( );

	int local_team = ctx.m_local->team( );

	for ( int i = 1; i < 65; i++ )
	{
		auto possible_target = csgo.m_entity_list( )->get_entity( i );

		if ( !possible_target || possible_target == ctx.m_local )
			continue;

		if ( possible_target->dormant( ) || !possible_target->alive( ) || possible_target->immune( ) )
			continue;

		if ( possible_target->team( ) == local_team )
			continue;

		auto data = g_playerlist.at( possible_target->index( ) );

		matrix3x4_t matrix[128];
		if ( !g_boner.get_latest( possible_target->index( ), matrix ) )
			continue;

		std::vector< possible_point_t > points;

		for ( auto hitbox_index = 0; hitbox_index < HITBOX_MAX; hitbox_index++ )
		{
			if ( !is_hitbox_checked( i, hitbox_index ) )
				continue;

			auto is_save_fps_hitbox = [](int hitbox) -> bool
					{
						switch ( hitbox )
						{
							case HITBOX_HEAD:
							case HITBOX_PELVIS:
							case HITBOX_CHEST:
							case HITBOX_RIGHT_FOOT:
							case HITBOX_LEFT_FOOT:
								return true;
							default:
								return false;
						}
					};

			if ( !is_save_fps_hitbox( hitbox_index ) && ctx.m_settings.rage_save_fps )
				continue;

			auto hitbox = possible_target->get_hitbox( hitbox_index );

			if ( !hitbox )
				continue;

			if ( !g_aimbot.can_hit_point_estimate( possible_target, hitbox_index ) )
				continue;

			const float pointscale = 0.8f;
			// this stuff needs rewrite
			const float hitbox_radius = hitbox->m_Radius * pointscale;

			vec3 min = hitbox->m_Mins, max = hitbox->m_Maxs;

			hitbox->m_Mins = math::VectorRotate( hitbox->m_Mins, hitbox->m_Rotation );
			hitbox->m_Maxs = math::VectorRotate( hitbox->m_Maxs, hitbox->m_Rotation );

			math::VectorTransformASM( &hitbox->m_Mins[0], matrix[hitbox->m_Bone], &min[0] );
			math::VectorTransformASM( &hitbox->m_Maxs[0], matrix[hitbox->m_Bone], &max[0] );

			auto center = ( min + max ) * 0.5f;

			if ( hitbox->m_Radius == -1.f )
			{
				points.emplace_back( center, hitbox_index );
				points.emplace_back( lerp( center, vec3( min.x, min.y, min.z ), pointscale ), hitbox_index );
				points.emplace_back( lerp( center, vec3( min.x, max.y, min.z ), pointscale ), hitbox_index );
				points.emplace_back( lerp( center, vec3( max.x, max.y, min.z ), pointscale ), hitbox_index );
				points.emplace_back( lerp( center, vec3( max.x, min.y, min.z ), pointscale ), hitbox_index );
				points.emplace_back( lerp( center, vec3( min.x, min.y, max.z ), pointscale ), hitbox_index );
				points.emplace_back( lerp( center, vec3( min.x, max.y, max.z ), pointscale ), hitbox_index );
				points.emplace_back( lerp( center, vec3( max.x, max.y, max.z ), pointscale ), hitbox_index );
				points.emplace_back( lerp( center, vec3( max.x, min.y, max.z ), pointscale ), hitbox_index );
			}
			else
			{
				points.emplace_back( center, hitbox_index );
				points.emplace_back( vec3( min.x + hitbox_radius, min.y, min.z ), hitbox_index );
				points.emplace_back( vec3( min.x - hitbox_radius, min.y, min.z ), hitbox_index );
				points.emplace_back( vec3( min.x, min.y + hitbox_radius, min.z ), hitbox_index );
				points.emplace_back( vec3( min.x, min.y - hitbox_radius, min.z ), hitbox_index );
				points.emplace_back( vec3( min.x, min.y, min.z + hitbox_radius ), hitbox_index );
				points.emplace_back( vec3( min.x, min.y, min.z - hitbox_radius ), hitbox_index );
				points.emplace_back( vec3( max.x + hitbox_radius, max.y, max.z ), hitbox_index );
				points.emplace_back( vec3( max.x - hitbox_radius, max.y, max.z ), hitbox_index );
				points.emplace_back( vec3( max.x, max.y + hitbox_radius, max.z ), hitbox_index );
				points.emplace_back( vec3( max.x, max.y - hitbox_radius, max.z ), hitbox_index );
				points.emplace_back( vec3( max.x, max.y, max.z + hitbox_radius ), hitbox_index );
				points.emplace_back( vec3( max.x, max.y, max.z - hitbox_radius ), hitbox_index );
			}
		}

		possible_targets.emplace_back( data, points );
	}

	if ( possible_targets.empty( ) )
		return false;

	std::sort( possible_targets.begin( ), possible_targets.end( ), []( const possible_target_t& a, const possible_target_t& b) -> bool
	           {
		           // cheaters are always first
		           if ( a.m_data->is_cheater != b.m_data->is_cheater )
			           return a.m_data->is_cheater > b.m_data->is_cheater;

		           // don't bother baiming legits
		           bool force_baim = a.m_data->is_cheater && ctx.m_settings.rage_aim_mode == 2;
		           bool should_baim = a.m_data->is_cheater && ctx.m_settings.rage_aim_mode == 1;

		           // check if we can resolve them when not forcing baim
		           if ( !force_baim && should_baim )
		           {
			           bool a_can_resolve = g_resolver.has_logged_shot( a.m_data->index );
			           bool b_can_resolve = g_resolver.has_logged_shot( b.m_data->index );

			           if ( a_can_resolve != b_can_resolve )
				           return a_can_resolve > b_can_resolve;
		           }

		           // sort targets by selection type
		           switch ( ctx.m_settings.rage_selection )
		           {
			           case 0: // fov
			           {
				           auto get_fov = [&]( const vec3& aim_angle ) -> float
						           {
							           vec3 ang, aim;

							           math::angle_vectors( ctx.m_local->eye_origin( ), &ang );
							           math::angle_vectors( aim_angle, &aim );

							           return math::rad2deg( acos( aim.dot( ang ) / ( ang.dot( aim ) ) ) );
						           };

				           return get_fov( a.m_data->base->eye_origin( ) ) < get_fov( b.m_data->base->eye_origin( ) );
			           }

			           case 1: // distance
				           return ctx.m_local->origin( ).dist( a.m_data->origin ) < ctx.m_local->origin( ).dist( b.m_data->origin );

			           case 2: // lag
				           return a.m_data->simtime > b.m_data->simtime;

			           case 3: // health
				           return a.m_data->health < b.m_data->health;

			           default: // default to cycle
			           case 4: // cycle
				           return rand( ) % 2 == 0;
		           }
	           } );

	auto sorted_target = possible_targets.front( );

	possible_point_t best_point( vec3( 0, 0, 0 ), -1 );
	for ( auto i = 0; i < sorted_target.m_points.size( ); i++ )
	{
		auto cur_point = sorted_target.m_points.at( i );

		bool force_baim = sorted_target.m_data->is_cheater && ctx.m_settings.rage_aim_mode == 2;
		bool should_baim = sorted_target.m_data->is_cheater && ctx.m_settings.rage_aim_mode == 1;
		bool can_resolve = g_resolver.has_logged_shot( sorted_target.m_data->index );

		if ( !force_baim && should_baim )
			force_baim = !can_resolve;

		if ( force_baim )
		{
			bool cur_baim = cur_point.m_target_hitbox >= HITBOX_PELVIS && cur_point.m_target_hitbox <= HITBOX_UPPER_CHEST;
			bool best_baim = best_point.m_target_hitbox >= HITBOX_PELVIS && best_point.m_target_hitbox <= HITBOX_UPPER_CHEST;

			if ( cur_baim && !best_baim )
			{
				best_point = cur_point;
				continue;
			}
		}

		if ( should_baim )
		{
			bool cur_baim = cur_point.m_target_hitbox == HITBOX_PELVIS;
			bool best_baim = best_point.m_target_hitbox == HITBOX_PELVIS;

			if ( cur_baim && !best_baim )
			{
				best_point = cur_point;
				continue;
			}
		}

		// bye foot fetish
		// because legs are not protected by armor, damage based selection tends to favour shooting at legs/feet
		// considering legs/feet aren't the most reliable in CSGO, its best to just skip em when possible
		{
			bool cur_leg = cur_point.m_target_hitbox >= HITBOX_RIGHT_THIGH && cur_point.m_target_hitbox <= HITBOX_LEFT_FOOT;
			bool best_leg = best_point.m_target_hitbox >= HITBOX_RIGHT_THIGH && best_point.m_target_hitbox <= HITBOX_LEFT_FOOT;

			if ( !cur_leg && best_leg )
			{
				best_point = cur_point;
				continue;
			}
		}

		if ( !cur_point.m_checked_damage )
		{
			cur_point.m_damage = g_auto_wall.get_damage( ctx.m_local, sorted_target.m_data->base, cur_point.m_aimpoint );
			cur_point.m_checked_damage = true;
		}

		if ( cur_point.m_damage < ctx.m_settings.rage_autowall_damage )
			continue;

		if ( cur_point.m_damage >= sorted_target.m_data->health )
		{
			cur_point.m_damage = 100.f;

			if ( cur_point.m_target_hitbox >= HITBOX_PELVIS )
				cur_point.m_damage += 1.f;

			if ( cur_point.m_target_hitbox >= HITBOX_PELVIS && cur_point.m_target_hitbox <= HITBOX_UPPER_CHEST )
				cur_point.m_damage += 1.f;
		}

		{
			bool cur_head = cur_point.m_target_hitbox >= HITBOX_HEAD == cur_point.m_target_hitbox <= HITBOX_NECK;

			float required_shot = sorted_target.m_data->health * 0.5f + 1.f;

			if ( !cur_head && cur_point.m_damage > required_shot )
			{
				best_point = cur_point;
				continue;
			}
		}

		//if ( !cur_point.m_checked_hitchance )
		//{
		//	vec3 aim_angle;
		//	math::vector_angles( cur_point.m_aimpoint, aim_angle );
		//
		//	if ( !g_aimbot.can_hit_point( aim_angle, sorted_target.m_data->base ) )
		//	{
		//		cur_point.m_checked_hitchance = true;
		//		continue;
		//	}
		//
		//	cur_point.m_checked_hitchance = true;
		//}

		if ( cur_point.m_damage > best_point.m_damage )
			best_point = cur_point;
	}

	if ( best_point.m_target_hitbox == -1 )
		return false;

	sorted_target.m_final_aimpoint = best_point.m_aimpoint;

	target = sorted_target;

	return true;
}

bool c_target_selector::find_backtrack_target( possible_target_t& target )
{
	return false;
}

bool c_target_selector::find_target( possible_target_t& target )
{
	if ( !ctx.m_local )
		return false;

	if ( find_normal_target( target ) )
		return true;

	if ( find_backtrack_target( target ) )
		return true;

	return false;
}

int c_target_selector::get_highest_threat( )
{
	if ( !ctx.m_local || !ctx.m_local->alive( ) )
		return 0;

	static int latest_tick{ };
	static int latest_target{ };

	if ( latest_tick == csgo.m_globals( )->tickcount )
		return latest_target;

	int local_team = ctx.m_local->team( );
	vec3 local_pos = ctx.m_local->origin( ) + vec3( 0.f, 0.f, 32.f );

	bool found_cheater{ };
	int threat_counter{ };
	const player_data_t* threats[65];

	for ( int i = 0; i < 65; i++ )
	{
		auto player_data = g_playerlist.at( i );

		if ( !player_data->is_valid )
			continue;

		if ( player_data->is_dormant || !player_data->is_alive || player_data->is_immune || player_data->is_teammate )
			continue;

		if ( player_data->is_cheater )
			found_cheater = true;

		threats[threat_counter++] = player_data;
	}

	//we didnt find any possible threats
	if ( !threat_counter )
	{
		latest_tick = csgo.m_globals( )->tickcount;
		latest_target = 0;
		return 0;
	}

	int highest_threat{ };
	float distance = 9999.f;
	for ( int i = 0; i < threat_counter; i++ )
	{
		if ( found_cheater && !threats[i]->is_cheater )
			continue;

		c_base_entity* ent = threats[i]->base;

		vec3 enemy_pos = ent->origin( );
		enemy_pos.z += 64.f;

		if ( g_auto_wall.is_visible( local_pos, enemy_pos, ctx.m_local, ent ) )
		{
			highest_threat = threats[i]->index;
			break;
		}

		vec3 angle;
		math::vector_angles( enemy_pos - local_pos, angle );
		vec3 delta = angle - csgo.m_engine( )->GetViewAngles( );
		delta.sanitize( );

		float dist = delta.length2d( );

		if ( distance > dist )
		{
			highest_threat = threats[i]->index;
			distance = dist;
		}
	}

	latest_tick = csgo.m_globals( )->tickcount;
	latest_target = highest_threat;

	return highest_threat;
}
