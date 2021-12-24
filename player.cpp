#include "context.h"
#include "math.h"
#include "interfaces.h"
#include "boner.h"
#include "autowall.h"

mstudiobbox_t* c_base_entity::get_hitbox( int hitbox )
{
	if ( !model( ) )
		return false;

	studiohdr_t* hdr = csgo.m_model_info( )->GetStudioModel( model( ) );
	if ( !hdr )
		return false;

	mstudiohitboxset_t* set = hdr->pHitboxSet( hitbox_set( ) );
	if ( !set )
		return false;

	return set->pHitbox( hitbox );
}

bool c_base_entity::get_hitbox_position( int hitbox, vec3& out )
{
	matrix3x4_t matrix[128];
	mstudiobbox_t* box;

	if ( !g_boner.get_latest( index( ), matrix ) )
		return false;

	box = get_hitbox( hitbox );

	if ( box )
	{		
		vec3 min, max;

		box->m_Mins = math::VectorRotate( box->m_Mins, box->m_Rotation );
		box->m_Maxs = math::VectorRotate( box->m_Maxs, box->m_Rotation );

		math::VectorTransformASM( &box->m_Mins[0], matrix[box->m_Bone], &min[0] );
		math::VectorTransformASM( &box->m_Maxs[0], matrix[box->m_Bone], &max[0] );

		out = ( min + max ).center( );

		return !out.is_zero( );		
	}

	return false;
}

bool c_base_entity::info( player_info_t* info )
{
	return csgo.m_engine( )->GetPlayerInfo( this->index( ), info );
}

bool c_base_entity::is_flashed( )
{
	return flash_time( ) - csgo.m_globals( )->curtime > flash_duration( ) * 0.5f;
}

c_base_weapon* c_base_entity::weapon( )
{
	return reinterpret_cast< c_base_weapon* >( csgo.m_entity_list( )->get_entity_from_base_handle( this->active_weapon( ) ) );
}

void c_base_entity::select_item( const char* name, int subtype, c_base_weapon* weapon )
{	

}

void c_base_entity::run_think( )
{
	static auto unk_func = memory::pattern::first_code_match< void( __thiscall* )( int ) >( csgo.m_client.module( ), xors( "55 8B EC 56 57 8B F9 8B B7 ? ? ? ? 8B C6" ) );

	int& think = *( int* ) ( ( uintptr_t ) this + 64 );

	if ( think != -1 && think > 0 && think <= this->tickbase( ) )
	{
		think = -1;
		unk_func( 0 );
		this->think( );
	}
}

void c_base_entity::invalidate_bone_cache( )
{
	// int __thiscall InvalidateBoneCache(int this)
	// 80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81

	/*
	.text:1019CDD0 80 3D 88 AA A5 10+                cmp     s_bEnableInvalidateBoneCache, 0
	.text:1019CDD7 74 16                             jz      short locret_1019CDEF
	.text:1019CDD9 A1 44 F4 F7 14                    mov     eax, MostRecentBoneCounter
	.text:1019CDDE 48                                dec     eax
	.text:1019CDDF C7 81 14 29 00 00+                mov     dword ptr [ecx+2914h], 0FF7FFFFFh
	.text:1019CDE9 89 81 80 26 00 00                 mov     [ecx+2680h], eax
	*/

	static uintptr_t InvalidateBoneCache = memory::pattern::first_code_match( csgo.m_client.module( ), xors( "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81" ) );

	ulong recent_bone_counter = **(ulong**)( InvalidateBoneCache + 10 );

	*(ulong*)( uintptr_t( this ) + 0x2680 ) = recent_bone_counter - 1;
	*(ulong*)( uintptr_t( this ) + 0x2914 ) = 0xFF7FFFFF;
	writable_bones( ) = 0;
}

bool c_base_entity::visible( c_base_entity* attacker, const vec3& src, const vec3& dst )
{
	c_trace_filter  filter( attacker );
	c_game_trace    tr;
	ray_t           ray;

	ray.Init( src, dst );

	csgo.m_engine_trace( )->TraceRay( ray, MASK_SHOT | CONTENTS_HITBOX, &filter, &tr );
	g_auto_wall.clip_trace_to_players( src, dst, MASK_SHOT, &tr, this );

	return tr.m_ent == this || tr.m_fraction > 0.97f;
}

bool c_base_weapon::can_fire( )
{
	float time = ctx.m_local->tickbase( ) * csgo.m_globals( )->interval_per_tick;

	// something wrong in prediction, tickbase lagging behind by 1 tick
	// remove this once prediction is actually p plz
	time += csgo.m_globals( )->interval_per_tick;

	if ( ctx.m_local->next_attack( ) <= time )
	{
		return ( next_primary( ) <= time );
	}
	return false;
}

std::string c_base_weapon::print_name( )
{
	const  auto  raw        = get_name( );
	const  auto  localized  = csgo.m_localize( )->localize( raw );

	return util::to_lower( util::unicode_to_ascii( localized ) );
}