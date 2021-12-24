#include "lagcomp.h"

c_lagcomp g_lagcomp;

void c_lagcomp::record::reset( )
{
	m_ent = nullptr;
	m_data.clear( );
}

void c_lagcomp::store_records( )
{
	for ( int i = 1; i < 65; i++ )
	{
		auto cur_record = m_records[i];

		c_base_entity* ent = csgo.m_entity_list( )->get_entity( i );
		if ( !ent || !ctx.m_local || !ctx.m_local->alive( ) || !ent->alive( ) || ent->team( ) == ctx.m_local->team( ) )
		{
			cur_record.reset( );
			continue;
		}

		if ( ent->immune( ) || ent->dormant( ) )
			continue;

		// Anim Fix/Whatever

		if ( cur_record.m_ent != ent )
		{
			cur_record.reset( );
			cur_record.m_ent = ent;
		}

		if (cur_record.m_data.empty())
		{
			cur_record.m_data.emplace_back( ent );
			continue;
		}

		if ( cur_record.m_data.front( ).m_simtime != ent->simulation_time( ) )
			cur_record.m_data.emplace_back( ent );

		while ( csgo.m_globals( )->curtime - cur_record.m_data.front( ).m_curtime > 1.f ) // sv_max_unlag
			cur_record.m_data.pop_back( );
	}
}

void c_lagcomp::set_tickcount( ) { }

float c_lagcomp::get_interpolation( )
{
	static auto cl_interp = csgo.m_engine_cvars( )->FindVar( xors( "cl_interp" ) );
	static auto cl_interp_ratio = csgo.m_engine_cvars( )->FindVar( xors( "cl_interp_ratio" ) );
	static auto sv_client_min_interp_ratio = csgo.m_engine_cvars( )->FindVar( xors( "sv_client_min_interp_ratio" ) );
	static auto sv_client_max_interp_ratio = csgo.m_engine_cvars( )->FindVar( xors( "sv_client_max_interp_ratio" ) );
	static auto cl_updaterate = csgo.m_engine_cvars( )->FindVar( xors( "cl_updaterate" ) );
	static auto sv_minupdaterate = csgo.m_engine_cvars( )->FindVar( xors( "sv_minupdaterate" ) );
	static auto sv_maxupdaterate = csgo.m_engine_cvars( )->FindVar( xors( "sv_maxupdaterate" ) );

	auto updaterate = std::clamp( cl_updaterate->get_float( ), sv_minupdaterate->get_float( ), sv_maxupdaterate->get_float( ) );
	auto lerp_ratio = std::clamp( cl_interp_ratio->get_float( ), sv_client_min_interp_ratio->get_float( ), sv_client_max_interp_ratio->get_float( ) );
	return std::clamp( lerp_ratio / updaterate, cl_interp->get_float( ), 1.0f );
}

bool c_lagcomp::is_tick_valid( int tickcount )
{
	INetChannelInfo* net_channel = csgo.m_engine( )->GetNetChannelInfo( );
	if ( !net_channel )
		return false;

	float lerp_time = get_interpolation( );

	float avg_latency = net_channel->GetAvgLatency( FLOW_INCOMING ) + net_channel->GetAvgLatency( FLOW_OUTGOING );

	int arrival_tick = csgo.m_globals( )->tickcount + 1 + math::time::to_ticks( avg_latency );

	float correct = std::clamp( lerp_time + net_channel->GetLatency( FLOW_OUTGOING ), 0.f, 1.f ) - math::time::to_time( arrival_tick + math::time::to_ticks( lerp_time ) - tickcount );

	return std::abs( correct ) < 0.2f - csgo.m_globals( )->interval_per_tick;
}

void c_lagcomp::create_move(c_user_cmd* cmd)
{
	m_cmd = cmd;

	set_tickcount( );
}

void c_lagcomp::frame_net_update_end( )
{
	store_records( );
}

c_lagcomp::record_entry_t::record_entry_t( c_base_entity* entity )
{
	m_origin = entity->origin( );
	m_abs_angles = entity->abs_angles( );
	m_eye_angles = entity->eye_angles( );
	m_render_angles = entity->render_angles( );
	m_velocity = entity->velocity( );
	m_simtime = entity->simulation_time( );
	m_flags = entity->flags( );
	m_anim_state = *entity->anim_state( );
	m_curtime = csgo.m_globals( )->curtime;

	memcpy( m_anim_layers, entity->anim_overlay( ), 15 * sizeof CAnimationLayer );
	memcpy( m_bone_matrix, entity->bone_cache( ), entity->bone_cache_count( ) * sizeof matrix3x4_t );
	memcpy( m_pose_params, entity->pose_parameters( ), 24 * sizeof(float) );

	m_bbmin = entity->mins( );
	m_bbmax = entity->maxs( );

	m_ent = entity;
}
