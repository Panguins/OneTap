#include "context.h"
#include <intrin.h>

decltype( hooked::original::o_get_screen_size ) hooked::original::o_get_screen_size;

void __fastcall hooked::get_screen_size( void* ecx, void* edx, int& x, int& y )
{
	static  const  auto  height               = memory::pattern::first_code_match( csgo.m_client.module( ), xors( "8B 0D ? ? ? ? 99 2B C2 D1 F8 89" ) );
	static  const  auto  weapon_recoil_scale  = csgo.m_engine_cvars( )->FindVar( xors( "weapon_recoil_scale" ) );
	const   uintptr_t    prev_ret_addr        = *reinterpret_cast< uintptr_t* >( ( uintptr_t ) _AddressOfReturnAddress( ) - 1 ) + sizeof( uintptr_t );

	hooked::original::o_get_screen_size( ecx, edx, x, y );

	if ( ctx.m_settings.visuals_recoil_crosshair )
	{
		if ( prev_ret_addr == height || prev_ret_addr == height - 14 )
		{
		}
	}
}