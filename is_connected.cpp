#include "context.h"
#include <intrin.h>
decltype( hooked::original::o_is_connected ) hooked::original::o_is_connected;

bool __fastcall hooked::is_connected( void* ecx, void* edx )
{
	/*
		sub_103973C0+3    8B 0D 60 4E F8 14                 mov     ecx, g_pEngine
		sub_103973C0+9    8B 01                             mov     eax, [ecx]
		sub_103973C0+B    8B 40 6C                          mov     eax, [eax+108]  ; isconnected
		sub_103973C0+E    FF D0                             call    eax
		sub_103973C0+10   84 C0                             test    al, al
		sub_103973C0+12   75 04                             jnz     short loc_103973D8
	*/

	static void* unk_sub_ret_addr = memory::pattern::first_code_match< void* >( csgo.m_client.module( ), xors( "75 04 B0 01 5F" ), -2 );

	if ( ctx.m_settings.misc_unlock_inventory && _ReturnAddress( ) == unk_sub_ret_addr )
		return false;

	return hooked::original::o_is_connected( ecx, edx );
}