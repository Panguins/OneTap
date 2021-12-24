#include "context.h"

decltype( hooked::original::o_find_model ) hooked::original::o_find_model;

uint16_t __fastcall hooked::find_model( void* ecx, void* edx, char* file )
{
	return hooked::original::o_find_model( ecx, edx, file );
}