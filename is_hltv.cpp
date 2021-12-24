#include "context.h"
#include <intrin.h>

decltype( hooked::original::o_is_hltv ) hooked::original::o_is_hltv;

bool __fastcall hooked::is_hltv( void* ecx, uintptr_t occ_ent )
{
	if ( !ctx.m_settings.rage_aimbot_enabled )
		return hooked::original::o_is_hltv( ecx, occ_ent );

	/*
		ReevaluateAnimLod+1D   8B 0D E0 4E F8 14                 mov     ecx, g_engine
		ReevaluateAnimLod+23   8B 01                             mov     eax, [ecx]
		ReevaluateAnimLod+25   8B 80 74 01 00 00                 mov     eax, [eax+372]  ; engine->is_hltv
		ReevaluateAnimLod+2B   FF D0                             call    eax
		ReevaluateAnimLod+2D   84 C0                             test    al, al
		ReevaluateAnimLod+2F   0F 85 85 02 00 00                 jnz     loc_1037655A
	*/
	static const auto reevaluate_animlod_call = memory::pattern::first_code_match( csgo.m_client.module( ), xors( "84 C0 0F 85 ? ? ? ? A1 ? ? ? ? 8B B7" ) );

	if ( uintptr_t( _ReturnAddress( ) ) == reevaluate_animlod_call && occ_ent )
	{
		// i think the same code is in invalidatebonecache too
		*( int * ) ( occ_ent + 0xA24 ) = -1;
		//*( int * ) ( occ_ent + 0xA30 
		*( int * ) ( occ_ent + 0xA2C ) = *( int * ) ( occ_ent + 0xA28 );
		*( int * ) ( occ_ent + 0xA28 ) = 0;

		/*
			remember the code at the beginning for checking if the ent ishltv? 
			
			well here is that label that it jnz's to, we cann skip all the code by returning true, resulting in a jump to loc_1037655A, however this results in invisible
			niggas on llocal servers i think, could add a detection for that if you care 
		*/
		/*
			ReevaluateAnimLod+2BA                    loc_1037655A:                           ; CODE XREF: ReevaluateAnimLod+2Fj
			ReevaluateAnimLod+2BA                                                            ; ReevaluateAnimLod+46j ...
			ReevaluateAnimLod+2BA  5F                                pop     edi
			ReevaluateAnimLod+2BB  5E                                pop     esi
			ReevaluateAnimLod+2BC  8B E5                             mov     esp, ebp
			ReevaluateAnimLod+2BE  5D                                pop     ebp
			ReevaluateAnimLod+2BF  8B E3                             mov     esp, ebx
			ReevaluateAnimLod+2C1  5B                                pop     ebx
			ReevaluateAnimLod+2C2  C2 04 00                          retn    4
		*/
		 return true;
	}

	return hooked::original::o_is_hltv( ecx, occ_ent );
}

bool __declspec( naked ) __fastcall hooked::is_hltv_gate( void*, void* )
{
	__asm
	{
		mov edx, edi
		jmp is_hltv
	}
}