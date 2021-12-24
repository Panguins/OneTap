#pragma once

#include <Windows.h>
#include <new>
#include <string>
#include <iterator>
#include <vector>
#include <sstream>

namespace util
{
	static __forceinline std::string unicode_to_ascii( const std::wstring& unicode )
	{
		std::string ascii_str( unicode.begin( ), unicode.end( ) );
		return ascii_str;
	}

	static __forceinline std::wstring ascii_to_unicode( const std::string& ascii )
	{
		std::wstring unicode_str( ascii.begin( ), ascii.end( ) );
		return unicode_str;
	}

	static __forceinline std::string to_lower( const std::string& upper_case_string )
	{
		std::string lower_case_string( upper_case_string );
		std::transform( lower_case_string.begin( ), lower_case_string.end( ), lower_case_string.begin( ), tolower );
		return lower_case_string;
	}

	static __forceinline std::string to_upper( const std::string& lower_case_string )
	{
		std::string upper_case_string( lower_case_string );
		std::transform( upper_case_string.begin( ), upper_case_string.end( ), upper_case_string.begin( ), toupper );
		return upper_case_string;
	}

	static __forceinline void get_all_files( std::vector<std::string>& out, const std::string& directory )
	{
		HANDLE dir;
		WIN32_FIND_DATA file_data;

		if ( ( dir = FindFirstFile( ( directory + "/*" ).c_str( ), &file_data ) ) == INVALID_HANDLE_VALUE )
			return;

		do
		{
			const std::string file_name = file_data.cFileName;
			std::string full_file_name = directory + "/" + file_name;
			const bool is_directory = ( file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) != 0;

			if ( file_name[0] == '.' || !strstr( file_name.c_str( ), xors( ".cfg" ) ) )
				continue;

			if ( is_directory )
				continue;

			// cull .// and .cfg
			out.push_back( ( full_file_name.substr( 0, full_file_name.find_last_of( "." ) ).substr( 3 ) ) );
		}
		while ( FindNextFile( dir, &file_data ) );

		FindClose( dir );
	}
}