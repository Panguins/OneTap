#include "settings.h"

#include <windows.h>

void simple_save( const char* path, const char* name, const void* src, size_t size, const char* filename )
{
	auto buffer = reinterpret_cast<char*>( alloca( size * 2 + 1 ) );
	auto data = reinterpret_cast<const uint8_t*>( src );

	for ( size_t i = 0; i < size; i++ )
		sprintf( &buffer[2 * i], "%02X", data[i] );

	WritePrivateProfileStringA( path, name, buffer, filename );
}

void simple_load( const char* path, const char* name, void* dest, size_t size, const char* filename )
{
	auto buffer = reinterpret_cast<char*>( alloca( size * 2 + 1 ) );
	auto data = reinterpret_cast<uint8_t*>( dest );

	GetPrivateProfileStringA( path, name, "", buffer, size * 2 + 1, filename );

	if ( *buffer == 0 )
	{
		for ( size_t i = 0; i < size; i++ )
			data[i] = 0;

		return;
	}

	for ( size_t i = 0; i < size; i++ )
	{
		unsigned temp;
		sscanf( &buffer[2 * i], xors( "%02X" ), &temp );
		data[i] = temp;
	}
}