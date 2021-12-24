#include "resolver.h"
#include "autowall.h"
#include "playerlist.h"

c_resolver g_resolver;

void c_resolver::impact( const vec3& pos )
{
	if ( !ctx.m_settings.rage_aimbot_enabled )
		return;

	resolver_data::impact_t new_impact;
	new_impact.end_pos = pos;
	new_impact.time = csgo.m_globals( )->realtime;
	new_impact.impacts.push_back( pos );

	m_impacts.push_back( new_impact );
}

void c_resolver::hurt( int target_id, int hitgroup, int damage )
{
	if ( !ctx.m_settings.rage_aimbot_enabled )
		return;

	m_hurts.push_back( { csgo.m_globals( )->realtime, target_id, hitgroup, damage } );
}

void c_resolver::shot( int target_id, int hitgroup, vec3 origin, vec3 target, c_base_entity* ent, bool did_backtrack )
{
	if ( !ctx.m_settings.rage_aimbot_enabled )
		return;

	static int last_shot = 0;

	if ( last_shot == csgo.m_globals( )->tickcount && !target_id )
	{
		//we can only shoot once per tick :>
		//because it was called twice in 1 cm move tick, this means that a shot was fired by aimbot, and at end of cm move, it registered again
		//we need to register at end of cm tho, so manual shooting doesnt fuck shit up
		return;
	}

	resolver_data::cm_shot_t new_shot;
	new_shot.time = csgo.m_globals( )->realtime;
	new_shot.target_id = target_id;
	new_shot.hitgroup = hitgroup;
	new_shot.step = get_current_set( )->get_step( m_data[target_id].shots );
	new_shot.origin = origin;
	new_shot.pos = target;
	new_shot.did_backtrack = did_backtrack;
	new_shot.has_record = false;

	if ( target_id && ent )
	{
		if ( !did_backtrack && !has_logged_shot( target_id ) )
		{
			//quick and dirty
			//why not iterate?
			//the aimbot will only shoot at legs if there is like no other valid option at all
			//this means the target must be hidden and sticking his foot out or something
			//iterating on that will just fuck shit up
			//once animations are fully accurate, we should get rid of this check
			if ( hitgroup != HITGROUP_LEFTLEG && hitgroup != HITGROUP_RIGHTLEG )
			{
				m_data[target_id].shots = get_current_set( )->increment( m_data[target_id].shots );
			}
		}

		if ( ent->bone_cache( ) && ent->bone_cache_count( ) )
		{
			new_shot.has_record = true;
			new_shot.record.create( ent );
		}
	}

	m_shots.push_back( new_shot );
	last_shot = csgo.m_globals( )->tickcount;
}

void c_resolver::sort_impacts( )
{
	if ( m_impacts.empty( ) )
		return;

	if ( !ctx.m_local )
		return;

	vec3 origin = ctx.m_local->origin( );

	std::vector<resolver_data::impact_t> new_impacts;

	for ( size_t i = 0; i < m_impacts.size( ); i++ )
	{
		bool new_time = true;
		int stored_impact = 0;

		for ( size_t j = 0; j < new_impacts.size( ); j++ )
		{
			if ( new_impacts[j].time == m_impacts[i].time )
			{
				new_time = false;
				stored_impact = j;
				break;
			}
		}

		if ( new_time )
		{
			new_impacts.push_back( m_impacts[i] );
		}
		else
		{
			float stored_dist = origin.dist( new_impacts[stored_impact].end_pos );
			float new_dist = origin.dist( m_impacts[i].end_pos );

			if ( new_dist > stored_dist )
				new_impacts[stored_impact].end_pos = m_impacts[i].end_pos;

			new_impacts[stored_impact].impacts.push_back( m_impacts[i].end_pos );
		}
	}

	m_impacts.clear( );
	for ( size_t i = 0; i < new_impacts.size( ); i++ )
	{
		m_impacts.push_back( new_impacts[i] );
	}
}

void c_resolver::match_impacts( )
{
	// cleanup old records
	float latency = ( csgo.m_engine( )->GetNetChannelInfo( )->GetAvgLatency( 0 ) + csgo.m_engine( )->GetNetChannelInfo( )->GetAvgLatency( 1 ) ) * 2.f;
	float maxtime = 0.1f + latency;

	float realtime = csgo.m_globals( )->realtime;
	for ( size_t i = 0; i < m_shots.size( ); i++ )
	{
		if ( std::abs( realtime - m_shots.at( i ).time ) > maxtime )
		{
			m_shots.erase( m_shots.begin( ) + i );
			i--;
		}
	}

	if ( m_impacts.empty( ) )
		return;

	// match data
	std::vector<resolver_data::match_t> new_matches;

	for ( size_t i = 0; i < m_impacts.size( ); i++ )
	{
		if ( m_shots.empty( ) )
			break;

		resolver_data::impact_t* impact = &m_impacts[i];
		resolver_data::player_hurt_t* hurt = nullptr;
		resolver_data::cm_shot_t shot = m_shots.front( );

		//match it to hurt
		for ( size_t j = 0; j < m_hurts.size( ); j++ )
		{
			if ( impact->time == m_hurts[j].time )
			{
				hurt = &m_hurts[j];
				break;
			}
		}

		resolver_data::match_t new_match;
		new_match.miss = !hurt;
		new_match.shot = shot;
		new_match.impact = *impact;
		new_match.hurt = hurt ? *hurt : resolver_data::player_hurt_t( );

		new_matches.push_back( new_match );
		m_shots.pop_front( );
	}

	m_impacts.clear( );
	m_hurts.clear( );
	m_matches.clear( );

	for ( size_t i = 0; i < new_matches.size( ); i++ )
		m_matches.push_back( new_matches[i] );
}

void c_resolver::resolve_matches( )
{
	if ( m_matches.empty( ) )
		return;

	if ( !ctx.m_local )
		return;

	for ( size_t i = 0; i < m_matches.size( ); i++ )
	{
		resolver_data::match_t& match = m_matches[i];

		bool did_hit = false;

		if ( !match.miss )
		{
			if ( match.shot.did_backtrack )
			{
				csgo.m_hud_chat->ChatPrintf( 0, 0, xors( "\x01 [\x09 1TAP\x01 ]  hit for %d damage (LC)" ), match.hurt.damage );
			}
			else 
			{
				if ( match.shot.target_id == match.hurt.target_id && match.shot.hitgroup == match.hurt.hitgroup )
				{
					//we got a logged shot

					int index = match.shot.target_id;
					m_data[index].valid_log = true;
					m_data[index].logged_step = match.shot.step;
					m_data[index].shots = get_current_set( )->has_step( m_data[index].logged_step );
				}

				csgo.m_hud_chat->ChatPrintf( 0, 0, xors( "\x01 [\x09 1TAP\x01 ]  hit for %d damage" ), match.hurt.damage );
			}

			did_hit = true;
		}

		c_base_entity* target = csgo.m_entity_list( )->get_entity( match.shot.target_id );

		if ( target && target->alive( ) && !did_hit && match.shot.target_id )
		{
			if ( missed_due_to_trace( &match ) )
			{
				//can mean that either resolver messed up in matching impacts, or autowall messed up and thought we could penetrate a wall we couldn't
				csgo.m_hud_chat->ChatPrintf( 0, 0, xors( "\x01 [\x09 1TAP\x01 ]  missed due to trace" ) );
				continue;
			}

			bool spread_miss = missed_due_to_spread( &match );

			int index = match.shot.target_id;

			if ( spread_miss )
			{
				if ( !match.shot.did_backtrack )
					m_data[index].shots = match.shot.step;

				csgo.m_hud_chat->ChatPrintf( 0, 0, xors( "\x01 [\x09 1TAP\x01 ]  missed due to spread" ) );
			}
			else if ( match.shot.did_backtrack )
			{
				csgo.m_hud_chat->ChatPrintf( 0, 0, xors( "\x01 [\x09 1TAP\x01 ]  missed due to lagcomp" ) );
			}
			else if ( m_data[index].valid_log && match.shot.step == m_data[index].shots )
			{
				m_data[index].valid_log = false;
				csgo.m_hud_chat->ChatPrintf( 0, 0, xors( "\x01 [\x09 1TAP\x01 ]  missed due to bad log" ) );
			}
			else
			{
				csgo.m_hud_chat->ChatPrintf( 0, 0, xors( "\x01 [\x09 1TAP\x01 ]  missed due to bad resolve" ) );
			}
		}
	}

	m_matches.clear( );
}

float c_resolver::get_freestand_yaw( c_base_entity* target )
{
	vec3 eye_pos, angle;

	eye_pos = target->origin( ) + vec3( 0.f, 0.f, 64.f );
	math::vector_angles( eye_pos - ctx.m_local->eye_origin( ), angle );

	static auto get_rotated_pos = []( vec3 start, float rotation, float distance )
	{
		float rad = math::deg2rad( rotation );
		start.x += cos( rad ) * distance;
		start.y += sin( rad ) * distance;

		return start;
	};

	weapon_info_t* freestand_rifle = g_auto_wall.get_awp( );

	float best_rotation = 9999.f;
	float lowest_damage = 9999.f;
	float highest_damage = 0.f;

	for ( float rot = 0.f; rot < 360.f; rot += 45.f )
	{
		vec3 pos = get_rotated_pos( eye_pos, angle.y + rot, -30.f );

		float damage = g_auto_wall.get_damage( ctx.m_local, target, pos, freestand_rifle );

		if ( damage > highest_damage )
		{
			highest_damage = damage;
		}
		else if ( damage < lowest_damage )
		{
			lowest_damage = damage;
			best_rotation = rot;
		}
	}

	if ( lowest_damage > 0.0f || highest_damage == 0.0f )
	{
		return angle.y + 180.f;
	}

	return best_rotation;
}

float c_resolver::get_yaw_step( c_base_entity* ent, resolver_data::resolver_data_t* data )
{
	float lby = ent->lowerbodyyaw( );

	if ( !ctx.m_local || !ent )
		return lby;

	//get set
	resolver_set_t* set = get_current_set( );

	//wrap shots to set size
	data->shots = set->wrap( data->shots );

	//get current step from set based on shots
	int step = set->get_step( data->shots );

	//last moving LBY
	if ( step == STEP_LAST_MOVING || step == STEP_FORCED_LAST_MOVING )
	{
		if ( m_data[ent->index( )].valid_last_moving )
			return m_data[ent->index( )].last_moving_lby;

		//continue to next step if no valid moving
		data->shots++;
		return get_yaw_step( ent, data );
	}

	//LBY
	if ( step == STEP_LBY )
	{
		//check if set contains last moving LBY
		bool last_moving = set->has_step( STEP_LAST_MOVING ) || set->has_step( STEP_FORCED_LAST_MOVING );

		//skip lby if set contains last moving and LBY is close to last moving
		if ( last_moving && m_data[ent->index( )].valid_last_moving && math::yaw_diff( m_data[ent->index( )].last_moving_lby, lby ) < 35.f )
		{
			data->shots++;
			return get_yaw_step( ent, data );
		}

		return lby;
	}

	//freestand resolver
	if ( step == STEP_FREESTAND )
		return get_freestand_yaw( ent );

	return lby;
}

float c_resolver::get_pitch_step( c_base_entity* ent, resolver_data::resolver_data_t* data )
{
	float pitch = ent->eye_angles( ).x;

	if ( !ctx.m_local || !ent )
		return pitch;

	//get set
	resolver_set_t* set = get_current_set( );

	//wrap shots to set size
	data->shots = set->wrap( data->shots );

	//get current step from set based on shots
	int step = set->get_step( data->shots );

	// force up
	if ( step == STEP_UP )
		return 89.f;

	return pitch;
}

bool c_resolver::missed_due_to_spread(resolver_data::match_t* match )
{
	bool spread_miss = false;

	c_base_entity* ent = csgo.m_entity_list( )->get_entity( match->shot.target_id );
		
	if ( ent && ent->alive( ) && !ent->dormant( ) && match->shot.has_record )
	{
		vec3 origin = match->shot.origin;
		vec3 impact = match->impact.end_pos;

		matrix3x4_t* bone_cache = ent->bone_cache( );
		size_t bone_cache_count = ent->bone_cache_count( );
		if ( !bone_cache || !bone_cache_count )
			return false; //we cant calculate if we missed due to spread, lets assume not

		//store original data
		resolver_data::simulate_record pristine_data;
		pristine_data.create( ent );

		//replace data
		match->shot.record.restore( ent );

		//do traces
		c_game_trace trace;
		ray_t ray;
		ray.Init( origin, impact );

		csgo.m_engine_trace( )->ClipRayToEntity( ray, MASK_SHOT, ent, &trace );

		spread_miss = !trace.m_ent || trace.m_ent != ent;

		//restore original data
		pristine_data.restore( ent );
	}

	return spread_miss;
}

bool c_resolver::missed_due_to_trace(resolver_data::match_t* match )
{
	vec3 origin = match->shot.origin;
	float impact_dist = origin.dist( match->impact.end_pos );
	float target_dist = origin.dist( match->shot.pos );

	//take off a bit so hitbox and such is accounted for
	target_dist -= 32.f;

	if ( impact_dist > target_dist )
		return false;

	//the trace was too short, lets see if the trace would actually hit the player if it was longer
	c_base_entity* ent = csgo.m_entity_list( )->get_entity( match->shot.target_id );

	if ( ent && ent->alive( ) && !ent->dormant( ) && match->shot.has_record )
	{
		vec3 spread_angle;
		math::vector_angles( match->impact.end_pos - origin, spread_angle );

		vec3 forward;
		math::angle_vectors( spread_angle, &forward );

		matrix3x4_t* bone_cache = ent->bone_cache( );
		size_t bone_cache_count = ent->bone_cache_count( );
		if ( !bone_cache || !bone_cache_count )
			return false;

		//store original data
		resolver_data::simulate_record pristine_data;
		pristine_data.create( ent );

		//replace data
		match->shot.record.restore( ent );

		//do traces
		c_game_trace trace;
		ray_t ray;
		ray.Init( origin, origin + ( forward * 4096.f ) );

		csgo.m_engine_trace( )->ClipRayToEntity( ray, MASK_SHOT, ent, &trace );

		bool hit_player = ( trace.m_ent && trace.m_ent == ent );

		//an extra thing that could be done here, is run autowall on the angle, see if autowall thinks we can damage the player
		//this could show us problem with autowall, because server says we cant hit this angle

		//restore original data
		pristine_data.restore( ent );

		return hit_player;
	}

	return false;
}

void c_resolver::fsn_update( )
{
	for ( int i = 1; i < 65; i++ )
	{
		c_base_entity* ent = csgo.m_entity_list( )->get_entity( i );
		if ( !ent || ent->dormant( ) || !ent->alive( ) )
			continue;

		resolver_angles[i] = ent->eye_angles( );
	}

	if ( !ctx.m_local || !ctx.m_settings.rage_resolver || !ctx.m_local->alive( ) )
	{
		if ( !m_shots.empty( ) )
			m_shots.clear( );

		if ( !m_hurts.empty( ) )
			m_hurts.clear( );

		if ( !m_impacts.empty( ) )
			m_impacts.clear( );

		if ( !m_matches.empty( ) )
			m_matches.clear( );

		return;
	}

	sort_impacts( );
	match_impacts( );
	resolve_matches( );

	int local_team = ctx.m_local->team( );
	for ( int i = 1; i < 65; i++ )
	{
		c_base_entity* ent = csgo.m_entity_list( )->get_entity( i );

		if ( !ent || ent == ctx.m_local || m_data[i].ent != ent || ent->team( ) == local_team )
		{
			m_data[i].ent = ent;
			m_data[i].valid_last_moving = false;

			m_data[i].valid_log = false;
			m_data[i].shots = 0;
			continue;
		}

		if ( !ent->alive( ) || ent->dormant( ) )
		{
			m_data[i].ent = ent;
			m_data[i].valid_last_moving = false;
			continue;
		}

		if ( !g_playerlist.at( i )->is_cheater )
			continue;

		if ( !g_playerlist.at( i )->is_fakewalking && g_playerlist.at( i )->is_onground && g_playerlist.at( i )->speed > 50.f )
		{
			if ( !m_data[i].valid_last_moving )
			{
				int last_moving_step = get_current_set( )->has_step( STEP_FORCED_LAST_MOVING );
				//we gotta set shots to last moving if we have it in current set
				if ( last_moving_step != -1 )
					m_data[i].shots = last_moving_step;
			}

			m_data[i].valid_last_moving = true;
			m_data[i].last_moving_lby = ent->lowerbodyyaw( );
		}

		//force step if we have a logged hit
		if ( m_data[i].valid_log )
		{
			int step_index = get_current_set( )->has_step( m_data[i].logged_step );
			if ( step_index != -1 )
				m_data[i].shots = step_index;
		}

		resolver_angles[i].x = get_pitch_step( ent, &m_data[i] );
		resolver_angles[i].y = get_yaw_step( ent, &m_data[i] );
	}
}