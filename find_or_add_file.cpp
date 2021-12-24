#include "context.h"

decltype( hooked::original::o_find_or_add_file ) hooked::original::o_find_or_add_file;

void* __fastcall hooked::find_or_add_file( void* ecx, void* edx, const char* path )
{
	return hooked::original::o_find_or_add_file( ecx, edx, path );
}