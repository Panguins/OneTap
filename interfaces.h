#pragma once
#include <memory>
#include <Windows.h>
#include <winternl.h>
#include <Vector>
#include <string>

#include "x86.h"
#include "factory.h"

#include "sdk.h"

namespace factory
{
	template <typename t>
	class c_interface_base
	{
		friend class c_csgo;

	public:
		void setup_interface_info( void* ptr, bool hook = true, bool safe_hook = false, const char* module_name = nullptr )
		{	
			MEMORY_BASIC_INFORMATION mbi;
			VirtualQuery( ptr, &mbi, sizeof( MEMORY_BASIC_INFORMATION ) );

			m_interface  = ptr;
			m_dll        = static_cast< HMODULE >( mbi.AllocationBase );
			m_hook       = hook ? std::make_shared<memory::c_vmt>( static_cast< void* >( ptr ), safe_hook, module_name ) : nullptr;
		}

		t* operator( )( )
		{
			return m_interface; 
		}

		// compiler fix 
		auto operator->( ) 
		{
			return m_hook;	
		}

		HMODULE module( )
		{
			return m_dll;
		}

	protected:
		t*       m_interface;
		HMODULE  m_dll;
		std::shared_ptr<memory::c_vmt> m_hook;
	};

	class c_csgo
	{
	public:
		c_interface_base<c_base_client>              m_client;
		c_interface_base<c_client_mode>              m_client_mode;
		c_interface_base<c_entity_list>              m_entity_list;
		c_interface_base<c_var>                      m_engine_cvars;
		c_interface_base<CEngineClient>			     m_engine;
		c_interface_base<CEngineTrace>				 m_engine_trace;
		c_interface_base<c_global_vars>				 m_globals;
		c_interface_base<CDebugOverlay>				 m_debug_overlay;
		c_interface_base<IVModelInfoClient>			 m_model_info;
		c_interface_base<ISurface>					 m_surface;
		c_interface_base< VPanel >					 m_panel;
		c_interface_base<c_direct_device_vtable>	 m_device;
		c_interface_base<c_input>					 m_input;
		c_interface_base<IGameMovement>				 m_movement;
		c_interface_base<IPrediction>				 m_prediction;
		c_interface_base<IMaterialSystem>			 m_material_system;
		c_interface_base<IVRenderView>				 m_render_view;
		c_interface_base<IVModelRender>			     m_model_render;
		c_interface_base<c_physics_props>			 m_phys_props;
		c_interface_base< c_client_state >			 m_client_state;
		c_interface_base< void >					 m_engine_sound;
		c_interface_base< c_localize >				 m_localize;
		c_interface_base< IGameEventManager2 >		 m_event_manager;
		c_interface_base< void >					 m_event_manager1;
		c_interface_base< IMDLCache >		         m_mdl_cache;
		c_interface_base< void >		             m_hdn;
		c_interface_base< void >		             m_file_system;

		CHudChat*                                    m_hud_chat;
		HWND                                         m_window;

		void find( void* iface, const std::string& name, bool hook = true, bool safe_hook = false, const char* module_name = nullptr )
		{
			auto interface_instance = static_cast<c_interface_base<void>*>( iface );

			interface_instance->setup_interface_info( g_factory.find_interface( name ), hook, safe_hook, module_name );
			m_container.push_back( interface_instance );
		}

		void set( void* iface, void* pointer, bool hook = true, bool safe_hook = false, const char* module_name = nullptr )
		{
			auto interface_instance = static_cast< c_interface_base<void>* >( iface );

			interface_instance->setup_interface_info( ( void* ) pointer, hook, safe_hook, module_name );
			m_container.push_back( interface_instance );
		}

		void release( )
		{
			// hear me out ok
			// sometimes, restore fails. . . 
			// calling it extra times, fixes it (:
			for ( size_t i = 0; i < 2; i++ )
			{
				for ( size_t i = 0; i < m_container.size( ); ++i )
				{
					if ( m_container[i]->m_hook && m_container[i]->m_hook->count( ) > 0 )
						m_container[i]->m_hook->restore( );
				}
			}
		}

	private:
		std::vector<c_interface_base<void>*> m_container;

	};

	void init(c_csgo* instance);
	void release(c_csgo* instance);
}

extern factory::c_csgo csgo;
