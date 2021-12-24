#include "context.h"
#include "autowall.h"
#include "legit.h"

c_legit g_legit;

void c_legit::init_create_move( )
{
	if ( !ctx.m_settings.legit_enabled || !ctx.m_local->weapon( ) )
		return;

	if ( ctx.m_settings.legit_trigger )
		triggerbot( );
}

void c_legit::init_override_mouse_input( float* x, float* y )
{
	aimbot( x, y );

	if ( ctx.m_settings.legit_rcs && ctx.m_settings.legit_rcs_standalone )
	{
		static const auto weapon_recoil_scale = csgo.m_engine_cvars( )->FindVar( xors( "weapon_recoil_scale" ) );

		vec3 aim_angles;
		csgo.m_engine( )->GetViewAngles( aim_angles );

		aim_angles.x -= ctx.m_local->punch( ).x * weapon_recoil_scale->get_float( ) * ( ctx.m_settings.legit_rcs_x / 100.0f );
		aim_angles.y -= ctx.m_local->punch( ).y * weapon_recoil_scale->get_float( ) * ( ctx.m_settings.legit_rcs_y / 100.0f );
	}
}

void c_legit::triggerbot( )
{
	if ( ctx.m_settings.legit_trigger_key > KEYS_NONE && !g_input.is_key_pressed( ctx.m_settings.legit_trigger_key ) )
		return;

	if ( ctx.m_settings.legit_trigger_scope && ( ctx.m_local->weapon( )->type( ) == sniper && !ctx.m_local->in_scope( ) ) )
		return;

	// i'll implement this later 
	/*if ( ctx.m_settings.legit_trigger_delay > 0 )
	{
		const   float   delay      = static_cast< float >( ctx.m_settings.legit_trigger_delay ) * 0.001f;
		static  float   fire_time  = csgo.m_globals( )->curtime + delay;

		if ( csgo.m_globals( )->curtime >= fire_time )
		{
			// we've reached the user's desired delay, time to fire
			ctx.m_cmd->m_buttons |= IN_ATTACK;

			// reset our timer to the next
			fire_time = csgo.m_globals( )->curtime + delay;
		}
	}*/

	c_game_trace tr;
	ray_t ray;
	c_trace_filter filter( ctx.m_local );
	const vec3 src = ctx.m_local->head_position( );
	vec3 dst;
	vec3 viewangles;
	constexpr uint32_t mask = CONTENTS_SOLID | CONTENTS_GRATE | CONTENTS_HITBOX;

	// ...
	{
		csgo.m_engine( )->GetViewAngles( viewangles );
		math::angle_vectors( viewangles, &dst );
	}

	// setup our raytrace
	ray.Init( src, ( dst * 8192.0f ) + src );

	// send out a raytrace and clip player bounds
	{
		csgo.m_engine_trace( )->TraceRay( ray, mask, &filter, &tr );
		g_auto_wall.clip_trace_to_players( src, dst, mask, &tr, ctx.m_local );

		// handle smoke check
		if ( ctx.m_settings.legit_trigger_smoke && m_aim_utils.trace_hit_smoke( src, dst ) )
			return;
	}

	// check if we've hit an entity, and it's hit a desirable hitbox
	if ( tr.m_ent && ( tr.m_hitgroup <= HITGROUP_LEFTLEG && tr.m_hitgroup > HITGROUP_GENERIC ) )
	{
		if ( ( tr.m_ent->team( ) == ctx.m_local->team( ) && !ctx.m_settings.legit_trigger_friendlies ) || tr.m_ent->immune( ) )
			return;

		ctx.m_cmd->m_buttons |= IN_ATTACK;

		// autopistol
		if ( ctx.m_local->weapon( )->type( ) == pistol && ctx.m_cmd->m_buttons & IN_ATTACK )
			ctx.m_cmd->m_cmd_number % 2 == 1 ? ctx.m_cmd->m_buttons |= IN_ATTACK : ctx.m_cmd->m_buttons &= ~IN_ATTACK;
	}
}

//bool c_legit::get_aim_pos( vec3& out, c_base_entity* player )
//{
//	matrix3x4_t matrix[128];
//
//	studiohdr_t* hdr;
//	mstudiohitboxset_t* set;
//	mstudiobbox_t* hitbox;
//
//	vec3 min, max;
//
//	const char* hitboxes[] = { xors("head"), xors("spine"), xors("pelvis"), };
//
//	if ( !player || !g_boner.get_latest( player->index( ), matrix ) )
//		return false;
//
//	hdr = csgo.m_model_info( )->GetStudioModel( player->model( ) );
//
//	if ( !hdr )
//		return false;
//
//	set = hdr->pHitboxSet( player->hitbox_set( ) );
//
//	if ( !set )
//		return false;
//
//	for ( size_t i = 0; i < set->numhitboxes; i++ )
//	{
//		hitbox = set->pHitbox( i );
//
//		if ( hitbox ) 
//		{
//			if ( strstr( hdr->GetBone( hitbox->m_Bone )->pszName( ), hitboxes[ctx.m_settings.rage_hitbox] ) )
//			{
//				hitbox->m_Mins = math::VectorRotate( hitbox->m_Mins, hitbox->m_Rotation );
//				hitbox->m_Maxs = math::VectorRotate( hitbox->m_Maxs, hitbox->m_Rotation );
//
//				math::VectorTransformASM( &hitbox->m_Mins[0], matrix[hitbox->m_Bone], &min[0] );
//				math::VectorTransformASM( &hitbox->m_Maxs[0], matrix[hitbox->m_Bone], &max[0] );
//
//				m_target_hitbox = i;
//
//				out = ( min + max ) * 0.5f;
//				//out.z += hitbox->m_Radius;
//				return !out.is_zero( );
//			}
//		}
//	}
//
//	return false;
//}

void c_legit::aimbot( float* mouse_x, float* mouse_y )
{
	//if ( !g_input.is_key_pressed( ctx.m_settings.legit_aim_key ) && ctx.m_settings.legit_aim_key > KEYS_NONE )
	//	return;
	//
	//c_base_entity* ent = m_aim_utils.get_aim_target( );
	//
	//if ( !ent || !ent->visible( ctx.m_local, ctx.m_local->head_position( ), ent->head_position( ) ) )
	//	return;
	//
	//vec3 hitbox_pos{ };
	//if ( !g_aimbot.get_aim_pos( hitbox_pos, ent ) )
	//	return;
	//
	//static const auto weapon_recoil_scale = csgo.m_engine_cvars( )->FindVar( xors( "weapon_recoil_scale" ) );
	//static const auto sensitivity = csgo.m_engine_cvars( )->FindVar( xors( "sensitivity" ) );
	//
	//float time_delta = m_aim_utils.get_time_delta( );
	//const vec3 delta_angles = math::_vector_angles( ctx.m_local->head_position( ), hitbox_pos );
	//const vec3 current_angles = csgo.m_engine( )->GetViewAngles( );
	//vec3 delta = ( delta_angles - current_angles );
	//const float distance = delta.length2d( );
	//
	//if ( ctx.m_settings.legit_rcs && !ctx.m_settings.legit_rcs_standalone )
	//{
	//	// rcs strength %
	//	delta.x -= ctx.m_local->punch( ).x * weapon_recoil_scale->get_float( ) * ( ctx.m_settings.legit_rcs_x / 100.0f );
	//	delta.y -= ctx.m_local->punch( ).y * weapon_recoil_scale->get_float( ) * ( ctx.m_settings.legit_rcs_y / 100.0f );
	//}
	//
	//if ( csgo.m_globals( )->curtime >= m_aim_utils.m_next_aim_time && distance > ( ctx.m_settings.legit_aim_threshold / 100.0f ) )
	//{
	//	m_aim_utils.m_last_aim_time = csgo.m_globals( )->curtime;
	//
	//	vec2 pixels = math::angles_to_pixels( delta );
	//
	//	if ( ctx.m_settings.legit_smooth < 100 )
	//		pixels = ( pixels / sensitivity->get_float( ) ) * ( ctx.m_settings.legit_smooth / 100.0f );
	//
	//	*mouse_x += pixels.x;
	//	*mouse_y += pixels.y;
	//}
	//
	//// we're on the target(sorta)
	//if ( distance < 1.0f )
	//{
	//	//if ( ctx.m_settings.legit_auto_shoot )
	//
	//	// if had ent and ent died
	//	//m_aim_utils.m_next_aim_time = csgo.m_globals( )->curtime + ( ctx.m_settings.legit_retarget_time / 100.0f );
	//}
}
