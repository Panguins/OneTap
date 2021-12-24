#include "playerlist.h"
#include "context.h"

c_playerlist g_playerlist;

bool c_playerlist::on_ground(c_base_entity* entity)
{
	vec3 bottom[4] =
	{
		entity->mins(),
		entity->mins(),
		entity->mins(),
		entity->mins()
	};

	bottom[1].x = entity->maxs().x;

	bottom[2].x = entity->maxs().x;
	bottom[2].y = entity->maxs().y;

	bottom[3].y = entity->maxs().y;

	for (int i = 0; i < 4; i++)
		bottom[i] += entity->abs_origin();

	c_game_trace result;
	c_trace_filter filter( ctx.m_local, TRACE_WORLD_ONLY );

	for (int i = 0; i < 4; i++)
	{
		csgo.m_engine_trace()->TraceRay(ray_t(bottom[i], bottom[i] + vec3(0, 0, -dist_epsilon)), MASK_PLAYERSOLID, &filter, &result);
		if (result.m_fraction != 1.0f || result.m_start_solid)
			return true;
	}

	for (int i = 0; i < 4; i++)
	{
		csgo.m_engine_trace()->TraceRay(ray_t(bottom[i] + vec3(0, 0, -dist_epsilon), bottom[(i + 1) % 4] + vec3(0, 0, -dist_epsilon)), MASK_PLAYERSOLID, &filter, &result);
		if (result.m_fraction != 1.0f || result.m_start_solid)
			return true;
	}

	csgo.m_engine_trace()->TraceRay(ray_t(bottom[0] + vec3(0, 0, -dist_epsilon), bottom[2] + vec3(0, 0, -dist_epsilon)), MASK_PLAYERSOLID, &filter, &result);
	if (result.m_fraction != 1.0f || result.m_start_solid)
		return true;

	csgo.m_engine_trace()->TraceRay(ray_t(bottom[1] + vec3(0, 0, -dist_epsilon), bottom[3] + vec3(0, 0, -dist_epsilon)), MASK_PLAYERSOLID, &filter, &result);
	if (result.m_fraction != 1.0f || result.m_start_solid)
		return true;

	return false;
}

void c_playerlist::update( )
{
	if ( !csgo.m_engine( )->IsInGame( ) || !ctx.m_local )
		return;

	// loop data
	int local_team = ctx.m_local->team( );

	// temp player data
	player_data_t* data;
	c_base_entity* player;

	for ( int i = 0; i < 65; i++)
	{
		data                    = &players[i];
		player                  = csgo.m_entity_list( )->get_entity( i );
		data->is_valid          = !!player;
		data->validating_tick   = data->base != player || !data->is_valid ||  data->is_dormant || !data->is_alive;
		data->is_simtime_update = false;

		if ( !data->is_valid )
			continue;

		// base data
	 	data->base        = player;
		data->index       = i;
	
		data->is_dormant  = player->dormant( );
		data->is_alive    = player->alive( );
		data->is_immune   = player->immune( );
		data->is_teammate = player->team( ) == local_team;

		// no need to update rest of data
		if ( data->is_dormant || !data->is_alive )
			continue;

		// interesting data
		data->old_simtime = data->simtime;
		float old_lby     = data->lby;

		data->simtime  = player->simulation_time( );
		data->animtime = data->old_simtime + csgo.m_globals( )->interval_per_tick;
		data->lby      = player->lowerbodyyaw( );
		data->distance = ctx.m_local->firebullets_position( ).dist( player->firebullets_position( ) );
		data->health   = player->health( );

		data->is_simtime_update = data->simtime != data->old_simtime;
		if ( data->is_simtime_update )
		{
			data->is_shot_update = player->weapon( ) && player->weapon( )->shot_time( ) == data->simtime;
			data->is_lby_update  = !data->validating_tick && data->lby != old_lby;
			data->is_cheater     = ( data->simtime - data->old_simtime ) != csgo.m_globals( )->interval_per_tick;

			data->fakelag_ticks  = data->validating_tick ? 0 : data->choked_ticks;
			data->choked_ticks   = 0;

			// raw animation layers, haven't yet been screwed up by client
			memcpy( data->old_anim_layers, data->validating_tick ? player->anim_overlay( ) : data->anim_layers, 15 * sizeof CAnimationLayer );
			memcpy( data->anim_layers, player->anim_overlay( ), 15 * sizeof CAnimationLayer );

			data->origin = player->origin( );

			data->velocity = player->velocity();
			data->speed = data->velocity.length();

			data->is_onground = on_ground( player );
			
			//player_info_t info;
			//auto valid = player->info( &info );
			//
			//if ( valid )
			//	data->is_onground ? printf("%s %s\n", info.name, "on ground") : printf("%s %s\n", info.name, "in air");

			data->is_fakewalking = data->is_onground && data->anim_layers[12].m_flWeight < 0.01f && data->speed > 1.f;
			data->is_moving      = data->speed > 0.1f && data->is_onground && !data->is_fakewalking;
		}
		else
		{
			data->choked_ticks++;
		}
	}
}