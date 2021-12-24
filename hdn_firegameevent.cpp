#include "context.h"

decltype( hooked::original::o_hdn_firegameevent ) hooked::original::o_hdn_firegameevent;

void __fastcall hooked::hdn_firegameevent( void* ecx, void* edx, int a2 )
{
	if ( *reinterpret_cast< uint8_t* >( ( uintptr_t ) ecx + 0x48 ) )
	{
		*reinterpret_cast< float* >( ( uintptr_t ) ecx + 0x54 ) = ctx.m_settings.misc_preserve_killfeed;
	}

	hooked::original::o_hdn_firegameevent( ecx, edx, a2 );
}