#pragma once
#include <memory>
#include <Windows.h>
#include <winternl.h>
#include <fstream>
#include <vector>
#include "x86.h"
#include "memory.h"

//WEE WOO WEE WOO ITS THE DWORD POLICE
using ulong = unsigned long;
using uword = unsigned short;

namespace factory
{
	namespace interfaces
	{
		struct interface_iterator_t
		{
			void* m_create_fn;
			char* m_name;

			interface_iterator_t* m_next;
		};

		inline auto follow_createinterface_export( uintptr_t export_ )
		{
			/*
			.text:006F5F00 CreateInterface proc near
			.text:006F5F00                 push    ebp
			.text:006F5F01                 mov     ebp, esp
			.text:006F5F03                 pop     ebp
			.text:006F5F04                 jmp     sub_6F5E90
			*/
			uintptr_t jmp = export_ + 0x4;
			uintptr_t jmp_target = jmp + *( uintptr_t* ) ( jmp + 0x1 ) + 0x5;

			return jmp_target;
		}

		inline auto find_list_ptr( uintptr_t createinterface )
		{
			/*
			.text:006F5E90                 push    ebp
			.text:006F5E91                 mov     ebp, esp
			.text:006F5E93                 push    esi
			.text:006F5E94                 mov     esi, dword_2EEFDE4
			.text:006F5E9A                 push    edi
			*/
			auto iterator_ptr = **( uintptr_t*** ) ( createinterface + 0x6 );

			return iterator_ptr;
		}

		inline bool is_createinterface_export( uintptr_t export_ )
		{
			return ( *( uint8_t* ) ( export_ ) == x86::encode_push_reg( x86::reg::ebp )
					 && *( uint8_t* ) ( export_ + 4 ) == 0xe9
					 && *( uint8_t* ) ( export_ + 9 ) == 0xcc
					 && *( uint8_t* ) ( export_ + 10 ) == 0xcc );
		}

		inline bool is_createinterface_fn( uintptr_t fn_ )
		{
			return ( *( uint8_t* ) ( fn_ ) == x86::encode_push_reg( x86::reg::ebp )
					 && *( uint8_t* ) ( fn_ + 4 ) == 0x8b
					 && *( uint8_t* ) ( fn_ + 10 ) == x86::encode_push_reg( x86::reg::edi ) );
		}

		class c_interface_manager
		{
		public:
			struct interface_data_t
			{
				std::string	m_name;
				std::string	m_module;
				size_t		m_version;
				uintptr_t	m_ptr;

				template < typename t > __forceinline t get( )
				{
					return reinterpret_cast< t >( m_ptr );
				}
			};

			c_interface_manager( );

			//iterate the interface list to find our desired version
			template < typename t = void* >
			t find_interface( const std::string& module_, std::string name )
			{
				//avoid finding interfaces with matching names
				if ( !::isdigit( name[name.length( )] ) )
					name += "0";

				for ( auto& it : m_interfaces )
				{
					if ( !it.m_module.compare( module_ ) && !it.m_name.compare( name ) )					
						return it.get< t >( );					
				}

				return t{ };
			}

			template < typename t = void* >
			t find_interface( std::string name )
			{
				//avoid finding interfaces with matching names
				if ( !isdigit( name[name.length( ) - 1] ) )
					name += "0";

				for ( auto& it : m_interfaces )
				{
					if ( strstr( it.m_name.c_str( ), name.c_str( ) ) )					
						return it.get< t >( );					
				}

				return t{ };
			}
		private:
			static uintptr_t find_createinterface( void* module_ );

			std::vector< interface_data_t > m_interfaces;
		};
	}
}

extern HMODULE g_dll;
extern factory::interfaces::c_interface_manager g_factory;