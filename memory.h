#pragma once
#pragma warning(disable : 4996)
#pragma warning(disable : 4018)
#include <Windows.h>
#include <new>
#include <string>
#include <iterator>
#include <vector>
#include <sstream>
#include <intrin.h>

namespace memory
{
	enum dll_sections_t
	{
		SECTION_TEXT,  //.text
		SECTION_RDATA, //.rdata
		SECTION_DATA,  //.data
		SECTION_RSRC,  //.rsrc
		SECTION_RELOC, //.reloc
		SECTION_MAX
	};

	// c_stack
	// stack manager/ helper class	

	// msvc compiletime fix
	#define get_ebp  ( void* ) ( ( uintptr_t ) _AddressOfReturnAddress() - sizeof( uintptr_t ) )
	class c_stack
	{
	private:
		void* base = nullptr;
	public:
		c_stack( void* base )
		{
			this->base = base;
		}

		void previous( unsigned int frames = 1 )
		{
			for ( ; frames < 0; --frames )
			{
				base = *( void** ) base;
			}
		}

		template <typename t>
		t get_local( uintptr_t offset )
		{
			return ( t ) ( ( uintptr_t ) base - offset );
		}

		template <typename t>
		t get_arg( uintptr_t offset )
		{
			( t ) get_retaddr( ) + offset;
		}

		uintptr_t get_retaddr( )
		{
			return ( uintptr_t ) base + sizeof( uintptr_t );
		}
	};

	class c_vmt
	{
		bool m_is_safe_hook{ };

		int                       m_vfunc_count{ };
		uintptr_t*                m_table{ };
		uintptr_t*                m_original{ };
		uintptr_t*                m_new{ };

		uintptr_t find_safe_space( const char* module_name, size_t needed_size )
		{
			uintptr_t module_address = uintptr_t( GetModuleHandleA( module_name ) );

			if ( !module_address )
				return 0;

			IMAGE_DOS_HEADER* dos_header         = (IMAGE_DOS_HEADER*)module_address;
			IMAGE_NT_HEADERS* nt_header          = (IMAGE_NT_HEADERS*)( module_address + dos_header->e_lfanew );
			IMAGE_SECTION_HEADER* section_header = (IMAGE_SECTION_HEADER*)( module_address + dos_header->e_lfanew + sizeof( IMAGE_NT_HEADERS ) );

			uintptr_t start = module_address + section_header[SECTION_DATA].VirtualAddress;
			uintptr_t end   = module_address + section_header[SECTION_DATA].VirtualAddress + nt_header->OptionalHeader.SizeOfInitializedData + nt_header->OptionalHeader.SizeOfUninitializedData;

			uintptr_t current = start;
			while ( current < end )
			{
				if ( *(uintptr_t*)current != 0)
				{
					current += sizeof( uintptr_t );
					continue;
				}

				MEMORY_BASIC_INFORMATION mbi = { 0 };
				VirtualQuery( (void*)current, &mbi, sizeof( mbi ) );

				if ( mbi.AllocationBase 
					 && mbi.BaseAddress 
					 && mbi.State == MEM_COMMIT 
					 && mbi.RegionSize > needed_size
					 && mbi.Protect != PAGE_NOACCESS
					 && !( mbi.Protect & PAGE_GUARD )
					 && mbi.Protect & ( PAGE_EXECUTE_READWRITE | PAGE_READWRITE ) )
				{
					uintptr_t page_start = uintptr_t( mbi.BaseAddress );
					uintptr_t page_end = page_start + mbi.RegionSize;

					// we found a commited memory page inside desired module, do extra ghetto check if its really not in use
					bool in_use = false;

					for ( uintptr_t page_address = page_start; page_address < page_end; page_address += sizeof( uintptr_t ) )
					{
						if ( *(uintptr_t*)page_address != 0 )
							in_use = true;
					}

					if ( !in_use )
						return page_end - needed_size;
				}

				current = mbi.RegionSize > 0 ? uintptr_t( mbi.BaseAddress ) + mbi.RegionSize : current + sizeof( uintptr_t );
			}

			return 0;
		}

	public:
		int count( )
		{
			int vfunc_count{ };

			while ( m_original[vfunc_count] )
			{
				vfunc_count++;
			};

			return vfunc_count;
		}

		c_vmt( void* table, bool safe_hook, const char* module_name )
		{
			if ( !table )
				return;

			m_is_safe_hook = safe_hook;

			m_table = reinterpret_cast< uintptr_t* >( table );
			m_original = *reinterpret_cast< uintptr_t** >( this->m_table );
			m_vfunc_count = count( );

			if ( safe_hook )
			{
				size_t needed_size = ( m_vfunc_count + 1 ) * 4;
				uintptr_t safe_space = find_safe_space( module_name, needed_size );

				// I NEED MY SAFE SPACE ! ! !
				if ( !safe_space )
				{
					printf( "could not find safe space in module %s :'( \n", module_name );
					return;
				}

				m_new = (uintptr_t*)safe_space;
			}
			else
			{
				m_new = new uintptr_t[m_vfunc_count + 1];
			}

			for ( int i = 0; i < m_vfunc_count; i++ )
				m_new[i] = m_original[i - 1];

			*m_table = uintptr_t( &m_new[1] );
		}

		~c_vmt( ) { }

		template< typename T = uintptr_t > T get_function( void* new_function, int index )
		{
			if ( index > m_vfunc_count )
				return 0;

			m_new[index + 1] = (uintptr_t)new_function;
			return( (T)( m_original[index] ) );
		}

		template< typename T = uintptr_t > T get_old_function( int index )
		{
			return( (T)( m_original[index] ) );
		}

		void unhook( int index )
		{
			m_new[index + 1] = m_original[index];
		}

		void restore( )
		{
			if ( m_new )
			{
				if ( m_is_safe_hook )
				{
					memset( m_new, 0, sizeof( uintptr_t ) * ( m_vfunc_count + 1 ) );
					m_new = nullptr;
				}
				else
				{
					delete[] m_new;
					m_new = nullptr;;
				}
			}

			*m_table = uintptr_t( m_original );
		}
	};

	template <typename fn>
	__forceinline fn get_vfunc(void* classbase, int index)
	{
		return (fn) (*(uintptr_t**) classbase)[index];
	}

	namespace pattern
	{
		inline bool bin_match(const uint8_t* code, const std::vector<uint8_t>& pattern)
		{
			for (size_t j = 0; j < pattern.size(); j++)
			{
				if (pattern[j] && code[j] != pattern[j])
					return false;
			}

			return true;
		}

		template <typename t = uintptr_t>
		static t first_match(uintptr_t start, std::string sig, size_t len, std::ptrdiff_t skip = 0)
		{
			// god this is
			std::istringstream iss(sig);
			std::vector<std::string> tokens{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};
			std::vector<uint8_t> pattern;

			for (const auto& hex_byte : tokens)
				pattern.push_back(static_cast<uint8_t>(std::strtoul(hex_byte.c_str(), nullptr, 16)));

			for (size_t i = 0; i < len; i++)
			{
				uint8_t* current_opcode = reinterpret_cast<uint8_t*>(start + i);

				if (current_opcode[0] != pattern.at(0))
					continue;

				if (bin_match(current_opcode, pattern))
					return ((t) (start + i + skip));
			}

			return 0;
		}

		template <typename t = uintptr_t>
		static t first_code_match(const HMODULE& start, const std::string& pattern, const std::ptrdiff_t& skip = 0)
		{
			PIMAGE_DOS_HEADER dos{reinterpret_cast<PIMAGE_DOS_HEADER>(start)};
			PIMAGE_NT_HEADERS nt;

			if (dos->e_magic != IMAGE_DOS_SIGNATURE)
				return 0;

			nt = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uintptr_t>(dos) + dos->e_lfanew);

			return first_match<t>( reinterpret_cast<uintptr_t>( dos ) + nt->OptionalHeader.BaseOfCode, pattern, nt->OptionalHeader.SizeOfCode, skip );;
		}
	};
}
