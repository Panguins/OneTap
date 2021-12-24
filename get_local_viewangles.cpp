#include "context.h"
#include <intrin.h>

decltype( hooked::original::o_get_local_viewangles ) hooked::original::o_get_local_viewangles;

// demo psilent/ whatever
// BUTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
// http://hrt.rocks/u/DeThU.png
// seems as tho ur animfix CRASHES when i record a demo ! ! ! ! ! 
void __fastcall hooked::get_local_viewangles( void* ecx, void* edx, vec3& angles )
{
	if ( !ctx.m_local )
		return hooked::original::o_get_local_viewangles( ecx, edx, angles );

	vec3 stored;

	hooked::original::o_get_local_viewangles( ecx, edx, stored );

	if ( _ReturnAddress( ) == memory::pattern::first_code_match< void* >( csgo.m_client.module( ), xors( "C7 46 ? ? ? ? ? 43" ) ) )
	{
		// will result in demos
		// looking like this
		// https://www.youtube.com/watch?v=a2E7svp1PGw&t=18s
		ctx.m_local->v_angle( ) = csgo.m_engine( )->GetViewAngles( );
	}

	hooked::original::o_get_local_viewangles( ecx, edx, angles );

	ctx.m_local->v_angle( ) = stored;
}