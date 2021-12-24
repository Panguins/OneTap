#include "boner.h"
#include "context.h"
#include "playerlist.h"
#include "anims.h"

c_boner g_boner;

void c_boner::update()
{
	memset( valid_bone_matrix, 0, 65 );

	if ( !ctx.m_local )
		return;

	matrix3x4_t temp_matrix[128];

	for ( int i = 1; i < 65; i++ )
	{
		c_base_entity* ent = csgo.m_entity_list( )->get_entity( i );
		if ( !ent )
			continue;

		if ( ent == ctx.m_local )
			continue;

		if ( ent->dormant( ) || !ent->alive( ) )
			continue;

		if ( ctx.m_settings.rage_aimbot_enabled )
		{
			ent->fuck_ik( );
			ent->set_abs_origin( ent->origin( ) );
		}

		ent->invalidate_bone_cache( );
		valid_bone_matrix[i] = ent->setup_bones( temp_matrix, 128, BONE_USED_BY_ANYTHING, ent->simulation_time( ) );

		mutex[i].lock( );
		memcpy( latest_bone_matrix[i], temp_matrix, sizeof( matrix3x4_t ) * 128 );
		mutex[i].unlock( );
	}
}

void c_boner::update_local( )
{
	if ( !ctx.m_local || !ctx.m_local->alive( ) )
		return;

	if ( !csgo.m_input( )->m_fCameraInThirdPerson )
		return;

	int i = ctx.m_local->index( );
	matrix3x4_t temp_matrix[128];

	// dirty way of setting angles
	// pitch here is model rotation, not actual body pitch ( body pitch is stored in pose parameters )
	vec3 o_abs_angles = *(vec3*)( (uintptr_t)ctx.m_local + 0xC4 );

	*(float*)( (uintptr_t)ctx.m_local + 0xC4 ) = 0.f;
	*(float*)( (uintptr_t)ctx.m_local + 0xC8 ) = g_anims.local_data( )->m_real_angle.y;

	ctx.m_local->invalidate_bone_cache( );
	valid_bone_matrix[i] = ctx.m_local->setup_bones( temp_matrix, 128, BONE_USED_BY_ANYTHING, ctx.m_local->simulation_time( ) );

	mutex[i].lock( );
	memcpy( latest_bone_matrix[i], temp_matrix, sizeof( matrix3x4_t ) * 128 );
	mutex[i].unlock( );

	*(vec3*)( (uintptr_t)ctx.m_local + 0xC4 ) = o_abs_angles;
}