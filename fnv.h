#pragma once

#pragma warning( disable : 4307 ) // '*': integral constant overflow
#pragma warning( disable : 4244 ) // possible loss of data

#include <cstdint>
#include <string>

using hash_t = unsigned int;

// used for compile-time FNV-1a 32bit hashes.
#define fnv( str )                                 \
    [&]() {                                        \
        constexpr hash_t out = hash::fnv1a( str ); \
                                                   \
        return out;                                \
    }()

// used for compile-time FNV-1a 32bit hashes when above macro cant be used for constexpr variables.
#define fnv_const( str ) Hash::fnv1a_32( str )

namespace hash // FNV-1a ( Fowler-Noll-Vo hash ).
{
	// FNV-1a constants.
	enum : hash_t
	{
		FNV1A_PRIME = 0x1000193,
		FNV1A_BASIS = 0x811C9DC5
	};

	// compile-time strlen.
	__forceinline constexpr size_t ct_strlen( const char *str )
	{
		size_t out = 1;

		for ( ; str[out] != '\0'; ++out );

		return out;
	}

	// hash data.
	__forceinline constexpr hash_t fnv1a( const uint8_t *data, const size_t len )
	{
		hash_t out = FNV1A_BASIS;

		for ( size_t i = 0; i < len; ++i )
			out = ( out ^ data[i] ) * FNV1A_PRIME;

		return out;
	}

	// hash c-style string.
	__forceinline constexpr hash_t fnv1a( const char *str )
	{
		hash_t out = FNV1A_BASIS;
		size_t len = ct_strlen( str );

		for ( size_t i = 0; i < len; ++i )
			out = ( out ^ str[i] ) * FNV1A_PRIME;

		return out;
	}

	// hash C++-style string ( runtime only ).
	__forceinline hash_t fnv1a( const std::string &str )
	{
		return fnv1a( (uint8_t *)str.c_str( ), str.size( ) );
	}
}