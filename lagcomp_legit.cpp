#include "lagcomp_legit.h"
#include "boner.h"

c_lagcomp_legit g_lagcomp_legit;

void c_lagcomp_legit::init()
{
	current_target = nullptr;

	if ( !ctx.m_local || !ctx.m_local->alive( ) )
		return;

	if ( ctx.m_settings.rage_aimbot_enabled )
		return;

	if ( !ctx.m_settings.legit_enabled || !ctx.m_settings.legit_backtrack )
		return;

	update( );
	set_tick( );
}


void c_lagcomp_legit::legit_record::update(c_base_entity* entity, int tickcount )
{
	if ( entity != m_entity )
	{
		m_data.clear( );
		m_entity = entity;
	}

	legit_record_entry new_entry;
	new_entry.tickcount = tickcount;

	g_boner.get_latest( entity->index( ), new_entry.bone_matrix );
	new_entry.aim_pos = entity->abs_origin( ) + vec3( 0.f, 0.f, 40.f );
	m_data.push_front( new_entry );

	while ( m_data.size( ) > ctx.m_tickrate )
		m_data.pop_back( );
}

void c_lagcomp_legit::update( )
{
	for ( int i = 1; i < 65; i++ )
	{
		c_base_entity* entity = csgo.m_entity_list( )->get_entity( i );
		if ( !entity || !entity->alive( ) || entity->dormant( ) || entity->immune( ) )
		{
			m_records[i].reset( );
			continue;
		}

		m_records[i].update( entity, ctx.m_cmd->m_tickcount );
	}
}

void c_lagcomp_legit::set_tick( )
{
	vec3 viewangles = csgo.m_engine( )->GetViewAngles( );
	vec3 punch = ctx.m_local->punch( ) * 2.0f;
	viewangles -= punch;

	c_base_entity* target{ };

	if ( find_closest_target( &target, viewangles ) )
		backtrack_target( target, viewangles );
}

bool c_lagcomp_legit::find_closest_target( c_base_entity** target, const vec3& viewangles )
{
	int local_team = ctx.m_local->team( );
	vec3 local_pos = ctx.m_local->eye_origin( );

	c_base_entity* found_target{ };
	float distance = 180.f;

	for ( int i = 1; i < 65; i++ )
	{
		c_base_entity* entity = csgo.m_entity_list( )->get_entity( i );
		if ( !entity || entity == ctx.m_local )
			continue;

		if ( !entity->alive( ) || entity->dormant( ) || entity->immune( ) )
			continue;

		if ( entity->team( ) == local_team )
			continue;

		vec3 target_pos = entity->abs_origin( ) + vec3( 0.f, 0.f, 40.f );

		vec3 target_angles;
		math::vector_angles( target_pos - local_pos, target_angles );

		vec3 delta = target_angles - viewangles;
		delta.sanitize( );

		float dist = delta.length2d( );
		if ( distance > dist )
		{
			distance = dist;
			found_target = entity;
		}
	}

	*target = found_target;
	return !!found_target;
}

void c_lagcomp_legit::backtrack_target( c_base_entity* target, const vec3& viewangles )
{
	legit_record* record = &m_records[target->index( )];
	if ( record->m_entity != target )
		return;

	vec3 local_pos = ctx.m_local->eye_origin( );
	int target_tickcount{ };
	matrix3x4_t* target_matrix = nullptr;

	float distance = 180.f;

	for ( size_t i{ }; i < record->m_data.size( ); i++ )
	{
		legit_record_entry* entry = &record->m_data.at( i );

		vec3 target_angles;
		math::vector_angles( entry->aim_pos - local_pos, target_angles );

		vec3 delta = target_angles - viewangles;
		delta.sanitize( );

		float dist = delta.length2d( );
		float time_delta = std::abs( ( csgo.m_globals()->tickcount - entry->tickcount ) * csgo.m_globals()->interval_per_tick );
		if ( distance > dist && time_delta <= ctx.m_settings.legit_backtrack_range )
		{
			distance = dist;
			target_tickcount = entry->tickcount;
			target_matrix = entry->bone_matrix;
		}
	}

	if ( target_tickcount && target_matrix )
	{
		ctx.m_cmd->m_tickcount = target_tickcount;

		if ( ctx.m_settings.legit_backtrack_visualize )
		{
			current_target = target;
			memcpy( current_matrix, target_matrix, sizeof( matrix3x4_t ) * 128 );
		}
	}
}