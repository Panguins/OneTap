#include "context.h"

decltype( hooked::original::o_reset ) hooked::original::o_reset;

long __stdcall hooked::reset( IDirect3DDevice9* device, void* params )
{
	//if ( !ctx.m_init )
	//	return hooked::original::o_reset( device, params );

	ctx.m_renderer->release( );

	auto ret = hooked::original::o_reset( device, params );

	ctx.m_renderer->create( );

	return ret;
}