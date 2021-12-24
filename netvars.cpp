#include "netvars.h"
#include "context.h"
#include "interfaces.h"

c_netvars g_netvars;

void c_netvars::init( )
{
	for ( auto client_class = csgo.m_client( )->get_all_classes( );
		!!client_class;
		client_class = client_class->m_next
		)
	{
		auto table = client_class->m_table;
		m_tables.push_back( table );
	}
}

RecvTable* c_netvars::get_table( hash_t hash ) const
{
	if ( m_tables.empty( ) ) return nullptr;

	for ( auto& table : m_tables )
	{
		if ( !table ) continue;

		if ( hash == hash::fnv1a( table->m_pNetTableName ) )
		{
			return table;
		}
	}

	return nullptr;
}

//iterating this too much results in a stack overflow, so thats cool
RecvProp* c_netvars::get_prop( hash_t data, hash_t name ) const
{
	RecvProp* prop { };
	RecvTable* child { };

	auto table = get_table( data );
	if ( !table ) return nullptr;

	for ( int i { }; i < table->m_nProps; ++i )
	{
		prop = &table->m_pProps[i];
		child = prop->m_pDataTable;

		if ( child && child->m_nProps )
		{
			auto tmp = get_prop( hash::fnv1a( child->m_pNetTableName ), name );
			if ( tmp ) return tmp;
		}

		if ( name != hash::fnv1a( prop->m_pVarName ) )
			continue;

		return prop;
	}

	return nullptr;
}

std::ptrdiff_t c_netvars::get_entry( hash_t name, RecvTable* table ) const
{
	std::ptrdiff_t ret { };
	RecvProp* prop;
	RecvTable* child;

	for ( int i { }; i < table->m_nProps; ++i )
	{
		prop = &table->m_pProps[i];
		child = prop->m_pDataTable;

		if ( child && child->m_nProps )
		{
			auto tmp = get_entry( name, child );
			if ( tmp ) ret += prop->m_Offset + tmp;
		}

		if ( name != hash::fnv1a( prop->m_pVarName ) )
			continue;

		return prop->m_Offset + ret;
	}

	return ret;
}

std::ptrdiff_t c_netvars::get_netvar( hash_t data, hash_t name ) const
{
	std::ptrdiff_t ret { };
	auto table = get_table( data );
	if ( !table ) return 0;

	ret = get_entry( name, table );
	return ret;
}
