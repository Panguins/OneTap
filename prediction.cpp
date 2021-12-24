#include "context.h"

c_prediction g_prediction;

// needs: keep prediction from running after we have already done it
void c_prediction::start( )
{
	/*auto footstep_fix = [&]( const int what_the_heck_do_we_set_sv_foot_steps_to_question_mark ) -> void
	{
	// set sv_footsteps to 0 before calling ProcessMovement, then restore to 1 after.
	};

	if ( ctx.m_settings.predfix_test )
	gmod.m_prediction( )->Update( gmod.m_client_state( )->m_delta_tick,
	gmod.m_client_state( )->m_delta_tick > 0,
	gmod.m_client_state( )->m_last_acknowledged_cmd,
	gmod.m_client_state( )->m_last_outgoing_cmd );
	*/

	if ( !ctx.m_local || !ctx.m_cmd )
		return;

	if ( ctx.m_last_cmd && !ctx.m_last_cmd->m_predicted )
		ctx.m_local->tickbase( )++;

	static  const  auto  run_command   = memory::pattern::first_code_match( csgo.m_client.module( ), xors( "55 8B EC 83 E4 C0 83 EC 34 53 56 8B 75 08" ) );
	static  const  auto  unk_func      = memory::pattern::first_code_match< bool( __thiscall* )( c_base_entity* ) >( csgo.m_client.module( ), xors( "56 57 8B F9 8B 97 ? ? ? ? 83 FA FF 74 41" ) );
	static  const  auto  move_helper   = **memory::pattern::first_code_match< c_move_helper*** >( csgo.m_client.module( ), xors( "8B 0D ? ? ? ? 8B 46 08 68" ), 2 );
	static  const  auto  pseudo_random = memory::pattern::first_code_match<int( __thiscall* )( int )>( csgo.m_client.module( ), xors( "55 8B EC 83 E4 F8 83 EC 70 6A 58" ) );

	auto&  pred_random_seed  = **reinterpret_cast< uint32_t** >( run_command + 48 );
	auto   pred_player       = *reinterpret_cast< c_base_entity** >( run_command + 84 );

	// ?
	const bool first_time = csgo.m_prediction( )->m_first_time_predicted;
	csgo.m_prediction( )->m_first_time_predicted = true;

	// ClientModeShared::CreateMove is called before random_seed is set ( hooking CHLClient::CreateMove and recreating the func by calling CInput::CreateMove might be better for several small reasons )
	ctx.m_cmd->m_random_seed = pseudo_random( ctx.m_cmd->m_cmd_number ) & 0x7ffffffff;

	// StartCommand
	{
		pred_player          = ctx.m_local;
		pred_random_seed     = ctx.m_cmd->m_random_seed;
		m_old_buttons        = ctx.m_cmd->m_buttons;
		m_old_flags          = ctx.m_local->flags( );
		m_old_move_type      = ctx.m_local->move_type( );
		m_old_velocity       = ctx.m_local->velocity( );
		m_old_base_velocity  = ctx.m_local->base_velocity( );
		m_old_fall_velocity  = ctx.m_local->fall_velocity( );
	}

	if ( csgo.m_prediction( )->m_engine_paused )
		csgo.m_globals( )->frametime = 0.0f;

	move_helper->set_host( ctx.m_local );

	// backup globals
	m_old_globals  = *csgo.m_globals( );

	// set globals
	csgo.m_globals( )->curtime = ctx.m_local->tickbase( ) * csgo.m_globals( )->interval_per_tick;
	csgo.m_globals( )->frametime = csgo.m_globals( )->interval_per_tick;

	csgo.m_movement( )->StartTrackPredictionErrors( ctx.m_local );

	ctx.m_local->run_pre_think( );
	ctx.m_local->think( );

	csgo.m_prediction( )->setup_move( ctx.m_local, ctx.m_cmd, move_helper, &m_move_data );

	int stored_tickbase = ctx.m_local->tickbase( );
	csgo.m_movement( )->ProcessMovement( ctx.m_local, &m_move_data );
	ctx.m_local->tickbase( ) = stored_tickbase;

	csgo.m_prediction( )->finish_move( ctx.m_local, ctx.m_cmd, &m_move_data );

	move_helper->unk_func( );

	ctx.m_local->run_post_think( );

	csgo.m_movement( )->FinishTrackPredictionErrors( ctx.m_local );
	move_helper->set_host( nullptr );

	csgo.m_globals( )->curtime    = m_old_globals.curtime;
	csgo.m_globals( )->frametime  = m_old_globals.frametime;
	
	csgo.m_prediction( )->m_first_time_predicted = first_time;

	*( int* ) ( ( uintptr_t ) ctx.m_local + 3269 ) = 0;

	// finishcommand
	{
		pred_player = nullptr;
		pred_random_seed = -1;
	}

	ctx.m_cmd->m_buttons           = m_old_buttons;
	ctx.m_local->flags( )          = m_old_flags;
	ctx.m_local->move_type( )      = m_old_move_type;
	ctx.m_local->velocity( )       = m_old_velocity;
	ctx.m_local->base_velocity( )  = m_old_velocity;
	ctx.m_local->fall_velocity( )  = m_old_fall_velocity;
}