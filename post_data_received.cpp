#include "context.h"
#include "netvar_compression.h"

decltype( hooked::original::o_post_data_received ) hooked::original::o_post_data_received;

// actually PostEntityPacketReceived
void __fastcall hooked::post_data_received( void* ecx, void* edx, int a2 )
{	
	g_netvar_compression.start( );

	hooked::original::o_post_data_received( ecx, edx, a2 );
}