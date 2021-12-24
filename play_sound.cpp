#include "context.h"
#include <intrin.h>

decltype( hooked::original::o_play_sound ) hooked::original::o_play_sound;

void __fastcall hooked::play_sound( void* ecx, void* edx, const char* sound )
{
	/*
		sub_10446420+64   68 F0 04 95 10                    push    offset aUiCompetitive_ ; "!UI/competitive_accept_beep.wav"
		sub_10446420+69   8B 0D C4 ED F8 14                 mov     ecx, dword_14F8EDC4
		sub_10446420+6F   8B 01                             mov     eax, [ecx]
		sub_10446420+71   FF 90 48 01 00 00                 call    dword ptr [eax+148h] ; playsound
		sub_10446420+77   89 B7 6C 31 00 00                 mov     [edi+316Ch], esi ; sig 
	*/

	static  const  auto  mm_ret_addr   = memory::pattern::first_code_match< void* >( csgo.m_client.module( ), xors( "89 B7 ? ? ? ? 8B 4F 04 85 C9" ) );
	static  const  auto  accept_match  = memory::pattern::first_code_match< void( __cdecl* )( ) >( csgo.m_client.module( ), xors( "FF B6 ? ? ? ? FF 76 2C" ), -0x5d );

	if ( ctx.m_settings.misc_auto_accept )
	{
		if ( _ReturnAddress( ) == mm_ret_addr )
			accept_match( );
	}

	hooked::original::o_play_sound( ecx, edx, sound );
}