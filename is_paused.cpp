#include "context.h"
#include <future>

decltype( hooked::original::o_is_paused ) hooked::original::o_is_paused;

std::vector<uintptr_t> printed_addresses;

bool __fastcall hooked::is_paused( void* ecx, void* edx )
{
	if ( !ctx.m_local )
		return hooked::original::o_is_paused( ecx, nullptr );

	/*
		void C_BaseEntity::InterpolateServerEntities()

	 	// Enable extrapolation?
		CInterpolationContext context;
		context.SetLastTimeStamp( engine->GetLastTimeStamp() );
		if ( cl_extrapolate.GetBool() && !engine->IsPaused() )
		{
			context.EnableExtrapolation( true );
		}
	 */
	static uintptr_t extrapolation_call = memory::pattern::first_code_match( csgo.m_client.module( ), xors( "FF D0 A1 ? ? ? ? B9 ? ? ? ? D9 1D" ) );

	if ( ctx.m_settings.rage_aimbot_enabled && uintptr_t( _ReturnAddress( ) ) == extrapolation_call )
	{
		return true;
	}

	return hooked::original::o_is_paused( ecx, nullptr );
}