#include "context.h"

decltype( hooked::original::o_shutdown ) hooked::original::o_shutdown;

void __fastcall hooked::shutdown( void* ecx, void* edx )
{
	// purpose:
	// allow hack to be unloaded
	// as the game is being closed
	// to prevent dumping, crashed process, etc

	// inform other thread that we've closed the game
	ctx.m_panic = true;

	// sleep to prevent the game closing too fast for us to unhook
	// it's possible unhooking doesn't start until 1s after this
	// is called, so this step is vital, please don't question me
	//std::this_thread::sleep_for( std::chrono::seconds( 2 ) );

	// call shutdown, resulting in the process closing, without our module present
	hooked::original::o_shutdown( ecx, edx );
}