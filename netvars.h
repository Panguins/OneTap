#pragma once
#include "sdk.h"
#include "fnv.h"

#include <vector>

#define NETVAR( funcname, type, table, name ) \
__forceinline auto& funcname( ) { \
	static std::ptrdiff_t offset = g_netvars.get_netvar( fnv( table ), fnv( name ) ); \
    if ( !offset ) \
	{ \
		offset = g_netvars.get_netvar( fnv( table ), fnv( name ) ); \
	} \
	return *reinterpret_cast< type* >( uintptr_t( this ) + offset ); \
}

#define PNETVAR( funcname, type, table, name ) \
__forceinline auto* funcname( ) { \
	static std::ptrdiff_t offset = g_netvars.get_netvar( fnv( table ), fnv( name ) ); \
    if ( !offset ) \
	{ \
		offset = g_netvars.get_netvar( fnv( table ), fnv( name ) ); \
	} \
	return reinterpret_cast< type* >( uintptr_t( this ) + offset ); \
}

#define NETVAR_OFFSET( funcname, type, table, name, extra ) \
__forceinline auto& funcname( ) { \
	static std::ptrdiff_t offset = g_netvars.get_netvar( fnv( table ), fnv( name ) ); \
	return *reinterpret_cast< type* >( uintptr_t( this ) + offset + extra ); \
}

class c_netvars
{
public:
	void init( );
	std::ptrdiff_t get_netvar( hash_t table, hash_t hash ) const;
	RecvProp* get_prop( hash_t table, hash_t name ) const;
public:
	std::ptrdiff_t get_entry( hash_t hash, RecvTable* table ) const;
	RecvTable* get_table( hash_t hash ) const;
private:
	std::vector< RecvTable* > m_tables;
};

extern c_netvars g_netvars;