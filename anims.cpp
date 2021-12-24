#include "anims.h"
#include "context.h"
#include "playerlist.h"

c_anims g_anims;

void anim_data_t::backup( c_base_entity* player )
{
	o_flags = player->flags( );
	o_duck_amount = player->duck_amount( );
}

void anim_data_t::restore( c_base_entity* player )
{
	player->flags( ) = o_flags;
	player->duck_amount( ) = o_duck_amount;
}

// quack fix
void c_anims::duck_fix( c_base_entity* player )
{
	int index = player->index( );

	anim_data_t*   anim_data = &m_data[index];
	player_data_t* player_data = g_playerlist.at( index );

	if ( !player_data->validating_tick )
	{
		float old_duck = anim_data->duck_amount;
		anim_data->duck_amount = player->duck_amount( );

		int ticks = math::time::to_ticks( player_data->simtime - player_data->old_simtime );

		float duck_change = anim_data->duck_amount - old_duck;
		float duck_1tick_change = duck_change / ticks;

		player->duck_amount( ) = old_duck + duck_1tick_change;
	}
	else
	{
		anim_data->duck_amount = player->duck_amount( );
	}
}

void c_anims::jump_fix( c_base_entity* player )
{
	int index = player->index( );

	anim_data_t*   anim_data = &m_data[index];
	player_data_t* player_data = g_playerlist.at( index );

	if ( !player_data->validating_tick )
	{
		int previous_flags = anim_data->flags;
		anim_data->flags = player->flags( );

		player->flags( ) = previous_flags;
		player->flags( ) &= ~FL_ONGROUND;

		if ( anim_data->flags & FL_ONGROUND && previous_flags & FL_ONGROUND )
		{
			player->flags( ) |= FL_ONGROUND;
		}
		else
		{
			if ( player_data->anim_layers[4].m_flWeight != 1.f && player_data->old_anim_layers[4].m_flWeight == 1.f && player_data->anim_layers[5].m_flWeight != 0.f )
			{
				player->flags( ) |= FL_ONGROUND;
			}


			if ( anim_data->flags & FL_ONGROUND && !( previous_flags & FL_ONGROUND ) )
			{
				player->flags( ) &= ~FL_ONGROUND;
			}
		}
	}
	else
	{
		anim_data->flags = player->flags( );
	}
}

void c_anims::pre_anim_update( c_base_entity* player )
{
	int index = player->index( );

	anim_data_t*   anim_data = &m_data[index];
	player_data_t* player_data = g_playerlist.at( index );

	// restore anim layers to raw state
	memcpy( player->anim_overlay( ), player_data->anim_layers, 15 * sizeof CAnimationLayer );
}

void c_anims::anim_update( c_base_entity* player )
{
	int index = player->index( );

	anim_data_t*   anim_data = &m_data[index];
	player_data_t* player_data = g_playerlist.at( index );

	// backup original data
	float o_curtime = csgo.m_globals( )->curtime;
	float o_frametime = csgo.m_globals( )->frametime;
	int   o_framecount = csgo.m_globals( )->framecount;
	int   o_tickcount = csgo.m_globals( )->tickcount;

	// simulate animations at anim update time from server
	float anim_time = player_data->animtime;
	int   anim_ticks = math::time::to_ticks( anim_time );

	csgo.m_globals( )->curtime = anim_time;
	csgo.m_globals( )->frametime = csgo.m_globals( )->interval_per_tick;
	csgo.m_globals( )->framecount = anim_ticks;
	csgo.m_globals( )->tickcount = anim_ticks;

	// allow anim updates
	player->clientside_anim( ) = true;

	// update anims
	player->update_clientside_anim( );

	// stop game from updating anims
	player->clientside_anim( ) = ctx.m_panic; //let go of anims when we are unloading cheat

											  // restore original data
	csgo.m_globals( )->curtime = o_curtime;
	csgo.m_globals( )->frametime = o_frametime;
	csgo.m_globals( )->framecount = o_framecount;
	csgo.m_globals( )->tickcount = o_tickcount;
}

void c_anims::post_anim_update( c_base_entity* player )
{
	int index = player->index( );

	anim_data_t*   anim_data = &m_data[index];
	player_data_t* player_data = g_playerlist.at( index );

	// restore anim layers to raw state
	memcpy( player->anim_overlay( ), player_data->anim_layers, 15 * sizeof CAnimationLayer );
}

void c_anims::origin_fix( c_base_entity* player )
{
	// s/o to senator for 1 line interpolation fix
	player->set_abs_origin( player->origin( ) );
}

void c_anims::velocity_fix( c_base_entity* player )
{
	// more has to be done here
	int index = player->index( );

	anim_data_t*   anim_data = &m_data[index];
	player_data_t* player_data = g_playerlist.at( index );

	if ( player_data->is_fakewalking )
	{
		player->velocity( ) = vec3( );
	}
}

void c_anims::update( )
{
	for ( int i = 1; i < 65; i++ )
	{
		c_base_entity* ent = csgo.m_entity_list( )->get_entity( i );
		if ( !ent )
			continue;

		if ( ent->dormant( ) || !ent->alive( ) )
			continue;

		// local player requires different method of calculating animations
		// this is because with networked entities, we wait for update
		// the data in this update is based on anim update from animtime (old simtime + 1)

		// on local player however, we already got data to simulate right when we started choking, we dont have to wait for send and then simulate back in time
		if ( ent == ctx.m_local )
			continue;

		anim_data_t* anim_data = &m_data[i];
		player_data_t* player_data = g_playerlist.at( i );

		if ( player_data->is_simtime_update )
		{
			anim_data->backup( ent );

			if ( ctx.m_settings.rage_aimbot_enabled )
			{
				origin_fix( ent );
				velocity_fix( ent );
				jump_fix( ent );
				duck_fix( ent );
			}

			pre_anim_update( ent );
			anim_update( ent );
			post_anim_update( ent );

			anim_data->restore( ent );
		}
	}
}

void c_anims::update_local( )
{
	if ( !ctx.m_local || !ctx.m_local->alive( ) )
		return;

	if ( !local_data( )->m_should_update_anims )
		return;

	local_data( )->m_should_update_anims = false;

	float time = local_data( )->m_anim_update_tickbase * csgo.m_globals( )->interval_per_tick;

	// store original
	vec3 o_eye_angles = ctx.m_local->eye_angles( );
	vec3 o_abs_angles = *(vec3*)( (uintptr_t)ctx.m_local + 0xC4 );

	// overwrite angles
	ctx.m_local->eye_angles( ) = local_data( )->m_real_angle;
	*(vec3*)( (uintptr_t)ctx.m_local + 0xC4 ) = local_data( )->m_real_angle;

	// update animations
	pre_anim_update( ctx.m_local );
	anim_update( ctx.m_local );
	post_anim_update( ctx.m_local );

	// calculate lby update time
	if ( ctx.m_local->anim_state( )->m_speed > 0.1f && !local_data( )->m_is_fakewalking )
	{
		local_data( )->m_next_lby_update = time + 0.22f;
	}
	else if ( time >= local_data( )->m_next_lby_update )
	{
		local_data( )->m_next_lby_update = time + 1.1f;
	}

	// restore angles
	ctx.m_local->eye_angles( ) = o_eye_angles;
	*(vec3*)( (uintptr_t)ctx.m_local + 0xC4 ) = o_abs_angles;

}

bool local_data_t::is_lby_update( )
{
	if ( !ctx.m_local || !ctx.m_local->alive( ) )
		return false;

	float time = ctx.m_local->tickbase( ) * csgo.m_globals( )->interval_per_tick;

	return ctx.m_is_anim_tick && time >= m_next_lby_update;
}