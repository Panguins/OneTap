#include "context.h"
#include "autowall.h"

c_auto_wall g_auto_wall;

float c_auto_wall::get_damage( c_base_entity* shooter, c_base_entity* target, const vec3& point, weapon_info_t* custom_weapon, const vec3* start ) const
{
	if ( !shooter->weapon( ) || !shooter->weapon( )->wpn_data( ) )
		return 0.f; //shooter doesn't have a weapon silly

	c_fire_bullet_params params;

	params.m_src = shooter == ctx.m_local ? shooter->firebullets_position( ) : shooter->origin( ) + vec3( 0.f, 0.f, 64.f );
	params.m_filter.m_filter = shooter;
	params.m_shooter = shooter;
	params.m_target = target;
	params.m_simulated_shot = !!custom_weapon;
	params.m_weapon_info = !!custom_weapon ? custom_weapon : shooter->weapon( )->wpn_data( );

	//shot is simulated from different origin
	if ( start )
		params.m_src = *start;

	params.m_range = !!custom_weapon ? params.m_src.dist( point ) : params.m_weapon_info->range;

	vec3 angles;
	math::vector_angles( point - params.m_src, angles );
	math::angle_vectors( angles, &params.m_dir );

	if ( fire_bullet( params ) )
		return params.m_dmg;

	return 0.f;
}

bool c_auto_wall::is_visible( const vec3& start, const vec3& end, c_base_entity* start_ent, c_base_entity* end_ent )
{
	c_trace_filter filter( start_ent, TRACE_WORLD_ONLY );
	c_game_trace trace;
	ray_t ray;

	ray.Init( start, end );

	csgo.m_engine_trace( )->TraceRay( ray, MASK_SHOT, &filter, &trace );

	return trace.m_fraction >= 0.97f || ( !!end_ent && trace.m_ent == end_ent );
}

void c_auto_wall::clip_trace_to_players( const vec3& start, const vec3& end, int mask, c_game_trace* old_trace, c_base_entity* ent ) const
{
	auto dir = ( end - start ).normalized( );
	auto center = ( ent->mins( ) + ent->maxs( ) ) * 0.5f;
	auto pos( center + ent->abs_origin( ) );

	auto to = pos - start;
	auto range_along = dir.dot( to );

	float range;
	if ( range_along < 0.0f )
		range = -to.length( );
	else if ( range_along > 0.0f )
		range = -( pos - end ).length( );
	else
		range = ( pos - ( dir * range_along + start ) ).length( );

	if ( range <= 60.0f )
	{
		c_game_trace trace;
		ray_t ray;
		ray.Init( start, end );
		csgo.m_engine_trace( )->ClipRayToEntity( ray, mask, ent, &trace );

		if ( old_trace->m_fraction > trace.m_fraction )
			*old_trace = trace;
	}
}

bool c_auto_wall::fire_bullet( c_fire_bullet_params& params ) const
{
	ray_t ray;

	params.m_pen_count = params.m_weapon_info->penetration;
	params.m_dmg = params.m_weapon_info->damage;

	while ( params.m_dmg > 0.f )
	{
		params.m_remaining_len = params.m_range - params.m_len;

		auto end_pos = params.m_src + params.m_dir * params.m_remaining_len;

		ray.Init( params.m_src, end_pos );

		csgo.m_engine_trace( )->TraceRay( ray, MASK_SHOT, &params.m_filter, &params.m_enter_tr );
		clip_trace_to_players( end_pos + params.m_dir * 40.f, params.m_src, MASK_SHOT_HULL | CONTENTS_HITBOX, &params.m_enter_tr, params.m_target );
		//g_aimbot.clip_trace_to_players( params.m_src, end_pos, MASK_SHOT, &params.m_filter, params.m_enter_tr );

		params.m_remaining_len += params.m_enter_tr.m_fraction * params.m_remaining_len;
		params.m_dmg *= std::pow( params.m_weapon_info->range_modifier, params.m_len * 0.002f );

		if ( params.m_enter_tr.m_fraction == 1.0f )
		{
			if ( params.m_simulated_shot )
			{
				//reached end point of trace, calculate damage as if it were a headshot
				scale_damage( HITGROUP_HEAD, params.m_target, params.m_weapon_info->armor_ratio, params.m_dmg );
				return true;
			}

			return false;
		}


		if ( params.m_enter_tr.m_ent && params.m_enter_tr.m_ent->index( ) != 0 
			 && params.m_enter_tr.m_hitgroup <= HITGROUP_RIGHTLEG 
			 && params.m_enter_tr.m_hitgroup >= HITGROUP_HEAD )
		{
			if ( params.m_enter_tr.m_ent->team( ) == params.m_shooter->team( ) )
				return false;

			scale_damage( params.m_simulated_shot ? HITGROUP_HEAD : params.m_enter_tr.m_hitgroup, params.m_enter_tr.m_ent, params.m_weapon_info->armor_ratio, params.m_dmg );

			return true;
		}

		auto enter_surface = csgo.m_phys_props( )->GetSurfaceData( params.m_enter_tr.m_surface.m_srf_props );

		if ( params.m_len > 3000.f || enter_surface->game.m_pen_mod <= 0.1f )
			params.m_weapon_info->penetration = 0.0f;

		if ( !handle_bullet_penetration( params ) )
			return false;
	}

	return false;
}

bool c_auto_wall::handle_bullet_penetration( c_fire_bullet_params& params ) const
{
	// thx dex for some of this

	if ( !ctx.m_settings.rage_autowall )
		return false;

	// readability
	constexpr uint32_t grate = 71, wood = 85, plastic = 76, glass = 89, cardboard = 87;

	// entry
	auto enter_surface = csgo.m_phys_props( )->GetSurfaceData( params.m_enter_tr.m_surface.m_srf_props );
	int enter_material = enter_surface->game.m_material;

	// out_end
	vec3 pen_end;

	// exit trace
	c_game_trace exit_trace;

	params.m_len += params.m_enter_tr.m_fraction * params.m_remaining_len;
	params.m_dmg *= std::pow( params.m_weapon_info->range_modifier, params.m_len * 0.002f );

	if ( params.m_pen_count <= 0 || params.m_weapon_info->penetration <= 0 )
		return false;

	if ( !trace_to_exit( params.m_enter_tr.m_src, params.m_dir, pen_end, params.m_enter_tr, exit_trace ) )
		if ( !( csgo.m_engine_trace( )->GetPointContents( pen_end, MASK_SHOT_HULL, nullptr ) & MASK_SHOT_HULL ) )
			return false;

	// convenience/readability
	bool is_light_surf = ( params.m_enter_tr.m_contents >> 7 ) & 1;
	bool is_solid_surf = ( params.m_enter_tr.m_contents >> 3 ) & 1;

	// get mat at exit
	auto exit_surface = csgo.m_phys_props( )->GetSurfaceData( exit_trace.m_surface.m_srf_props );

	if ( !exit_surface )
		return false;

	// exit point
	int exit_material = exit_surface->game.m_material;

	// penetration handling
	auto damage_mod = 0.16f;
	auto pen_mod = ( enter_surface->game.m_pen_mod + exit_surface->game.m_pen_mod ) * 0.5f;

	// handle transparent mats
	if ( enter_material == grate /* metal vents */ || enter_material == glass /* windows */ )
	{
		pen_mod = 3.0f;
		damage_mod = 0.05f;
	}
	else if ( is_light_surf || is_solid_surf )
	{
		pen_mod = 1.0f;
		damage_mod = 0.16f;
	}

	// thinner materials like wood/plastic get a pen buff
	if ( enter_material == exit_material )
	{
		if ( exit_material == cardboard || exit_material == wood )
			pen_mod = 3.0f;
		else if ( exit_material == plastic )
			pen_mod = 2.0f;
	}

	params.m_len = exit_trace.m_dst.dist( params.m_enter_tr.m_dst );
	auto modifier = std::max( 0.f, 1.0f / pen_mod );
	auto modified_penetration = ( ( modifier * 3.0f ) * std::max( 0.f, ( 3.0f / params.m_pen_count ) * 1.25f )
	                              + ( params.m_dmg * damage_mod ) ) + ( ( ( params.m_len * params.m_len ) * modifier ) * 0.0416f );

	// handle damage scaling
	params.m_dmg -= std::max( 0.f, modified_penetration );

	// can't do any damage
	if ( params.m_dmg <= 0.f )
		return false;

	// set new start point for successful trace
	params.m_src = exit_trace.m_dst;

	// decrement pencount
	params.m_pen_count--;

	// damage is > 0
	return true;
}

// this is just from my old hack, probably wrong, ill reverse later if nobody does b4 me
bool c_auto_wall::trace_to_exit( vec3 src, vec3 dir, vec3& out_end, const c_game_trace& enter, c_game_trace& exit ) const
{
	float dist{ };
	vec3 end;
	int contents;
	c_trace_filter filter;
	ray_t ray, ray_2;

	static auto is_breakable = memory::pattern::first_code_match< bool( __thiscall*)( c_base_entity* ) >( csgo.m_client.module( ), xors( "51 56 8B F1 85 F6 74 68 83 BE" ), -3 );

	ctx.m_local->weapon( )->wpn_data( )->penetration = 4.0f;

	while ( dist <= 90.f )
	{
		dist += 4.0f;

		out_end = src + ( dir * dist );

		contents = csgo.m_engine_trace( )->GetPointContents( out_end, MASK_SHOT, nullptr );

		if ( ( contents & MASK_SHOT_HULL ) && !( contents & CONTENTS_HITBOX ) )
			continue;

		end = out_end - ( dir * 4.0f );

		ray.Init( out_end, end );
		csgo.m_engine_trace( )->TraceRay( ray, MASK_SHOT, nullptr, &exit );

		if ( exit.m_start_solid && ( exit.m_surface.m_flags & SURF_HITBOX ) )
		{
			filter.m_filter = exit.m_ent;

			ray_2.Init( out_end, src );
			csgo.m_engine_trace( )->TraceRay( ray, MASK_SHOT_HULL, &filter, &exit );

			if ( exit.m_fraction < 1 && !exit.m_start_solid )
				return true;
		}
		else if ( !exit.hit( ) || exit.m_start_solid )
		{
			/*
				if ( *(_BYTE *)(ent + 566) == 2 ) // ty llama
			*/
			*reinterpret_cast< int* >( enter.m_ent + 0x236 ) = 2;
			if ( enter.m_ent == csgo.m_entity_list( )->get_entity( 0 ) && is_breakable( enter.m_ent ) )
				return true;

			src = enter.m_dst;
		}
		else if ( ( exit.m_surface.m_flags & SURF_NODRAW ) || !( exit.m_surface.m_flags & SURF_NODRAW ) && exit.m_plane.normal.dot( dir ) <= 1.f )
			return true;
	}

	return false;
}
