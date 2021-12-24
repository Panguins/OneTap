#include <algorithm>
#include "interfaces.h"
#include "factory.h"
#include "context.h"
#include "events.h"
factory::c_csgo csgo;

typedef bool ( __thiscall* set_rich_presence )(void*, const char*, const char*);
extern set_rich_presence o_set_rich_presence;

typedef void*( __thiscall* steam_friends_fn )(void*, __int32, __int32, const char*);

namespace SteamAPI
{
	FARPROC GetHSteamPipe;
	FARPROC GetHSteamUser;
	void* Client;
	void* Friends;
}

void factory::init( factory::c_csgo* instance )
{
	// this hurt 
	instance->m_window = FindWindowA( xors( "Valve001" ), nullptr );

	instance->find( &csgo.m_client,				 xors( "VClient" ), true, true, xors( "client.dll" ) );
	instance->find( &csgo.m_entity_list,		 xors( "VClientEntityList" ) );
	instance->find( &csgo.m_engine_cvars,		 xors( "VEngineCvar" ) );
	instance->find( &csgo.m_engine,				 xors( "VEngineClient" ), true, true, xors( "engine.dll" ) );
	instance->find( &csgo.m_engine_trace,		 xors( "EngineTraceClient" ) );
	instance->find( &csgo.m_debug_overlay,		 xors( "VDebugOverlay" ) );
	instance->find( &csgo.m_surface,			 xors( "VGUI_Surface" ) );
	instance->find( &csgo.m_panel,				 xors( "VGUI_Panel" ) );
	instance->find( &csgo.m_model_info,			 xors( "VModelInfoClient" ) );
	instance->find( &csgo.m_movement,	         xors( "GameMovement" ) );
	instance->find( &csgo.m_prediction,			 xors( "VClientPrediction" ) );
	instance->find( &csgo.m_material_system,	 xors( "VMaterialSystem" ) );
	instance->find( &csgo.m_render_view,		 xors( "VEngineRenderView" ) );
	instance->find( &csgo.m_model_render,		 xors( "VEngineModel" ) );
	instance->find( &csgo.m_phys_props,			 xors( "VPhysicsSurfaceProps" ) );
	instance->find( &csgo.m_engine_sound,		 xors( "IEngineSoundClient" ) );
	instance->find( &csgo.m_localize,			 xors( "Localize_" ) );
	instance->find( &csgo.m_event_manager,		 xors( "GAMEEVENTSMANAGER002" ) );
	instance->find( &csgo.m_event_manager1,		 xors( "GAMEEVENTSMANAGER" ) );
	instance->find( &csgo.m_mdl_cache,			 xors( "MDLCache" ) );

	instance->set( &instance->m_file_system, **memory::pattern::first_code_match< void*** >( instance->m_engine.module( ), xors( "8B 0D ? ? ? ? 56 8B 01 FF 90 ? ? ? ? 8B F0 8D 45 F0" ), 2 ) );
	instance->set( &instance->m_globals, *memory::pattern::first_code_match<c_global_vars**>( instance->m_engine.module( ), xors( "68 ? ? ? ? 68 ? ? ? ? FF 50 08 85 C0" ), 1 ), false );
	instance->set( &instance->m_device, reinterpret_cast< c_direct_device_vtable* >( **memory::pattern::first_code_match<uintptr_t**>( GetModuleHandle( xors( "shaderapidx9.dll" ) ), xors( "50 8B 08 FF 51 0C 8B F8" ), -4 ) ) );
	instance->set( &instance->m_input, *memory::pattern::first_code_match<c_input**>( instance->m_client.module( ), xors( "8B 35 ? ? ? ? FF 90 ? ? ? ? 50 B9 ? ? ? ? FF 56 24 5E C3" ), 2 ), false );
	instance->set( &instance->m_client_mode, **reinterpret_cast< c_client_mode*** >( ( *(uintptr_t**)instance->m_client( ) )[10] + 5 ) );
	instance->set( &instance->m_client_state, **reinterpret_cast< c_client_state*** >( ( *( uintptr_t** ) instance->m_engine( ) )[12] + 16 ), false );

	//instance->m_iclient_renderable(*memory::pattern::first_code_match<uintptr_t**>(instance->m_client.module( ), "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 89 7C 24 0C", 0x4E));

	auto hud_component = *memory::pattern::first_code_match<uintptr_t**>( instance->m_client.module( ), xors( "B9 ? ? ? ? E8 ? ? ? ? 83 7D F8" ), 1 );
	auto find_hud_element = memory::pattern::first_code_match<uintptr_t( __thiscall* )( void*, const char* )>( instance->m_client.module( ), xors( "55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28" ) );
	//instance->m_hud_chat = (CHudChat*)( find_hud_element( hud_component, "CHudChat" ) );

	//instance->create( &csgo.m_hdn, *reinterpret_cast< void** >( ( uintptr_t ) v2 ) );

	// before hooks are init, to prevent running sometimes without any netvars
	// after reinjecting, or injecting in game 
	g_netvars.init( );

	hooked::original::o_wnd_proc              = reinterpret_cast< decltype( &hooked::wnd_proc ) >( SetWindowLong( instance->m_window, -4, ( long ) hooked::wnd_proc ) );
	hooked::original::o_reset                 = instance->m_device->get_function<decltype( &hooked::reset )>( &hooked::reset, 16 );
	hooked::original::o_present               = instance->m_device->get_function<decltype( &hooked::present )>( &hooked::present, 17 );

	hooked::original::o_should_draw_fog       = instance->m_client_mode->get_function<decltype( &hooked::should_draw_fog )>( &hooked::should_draw_fog, 17 );
	hooked::original::o_override_view         = instance->m_client_mode->get_function< decltype( &hooked::override_view ) >( &hooked::override_view, 18 );
	hooked::original::o_override_mouse_input  = instance->m_client_mode->get_function< decltype( &hooked::override_mouse_input ) >( &hooked::override_mouse_input, 23 );
	hooked::original::o_create_move           = instance->m_client_mode->get_function<decltype( &hooked::create_move )>( &hooked::create_move, 24 );
	hooked::original::o_post_screen_effects   = instance->m_client_mode->get_function< decltype( &hooked::post_screen_effects ) >( &hooked::post_screen_effects, 44 );

	hooked::original::o_post_data_received    = instance->m_prediction->get_function<decltype( &hooked::post_data_received )>( &hooked::post_data_received, 6 );
	hooked::original::o_get_local_viewangles  = instance->m_prediction->get_function<decltype( &hooked::get_local_viewangles )>( &hooked::get_local_viewangles, 12 );
	hooked::original::o_in_prediction         = instance->m_prediction->get_function<decltype( &hooked::in_prediction )>( &hooked::in_prediction, 14 );
	hooked::original::o_run_command           = instance->m_prediction->get_function<decltype( &hooked::run_command )>( &hooked::run_command, 19 );


	hooked::original::o_shutdown              = csgo.m_client->get_function<decltype( &hooked::shutdown )>( &hooked::shutdown, 4 );
	hooked::original::o_frame_stage_notify    = csgo.m_client->get_function<decltype( &hooked::frame_stage_notify )>( &hooked::frame_stage_notify, 36 );

	hooked::original::o_is_connected          = instance->m_engine->get_function< decltype( &hooked::is_connected ) >( &hooked::is_connected, 27 );
	hooked::original::o_is_paused             = instance->m_engine->get_function< decltype( &hooked::is_paused ) >( &hooked::is_paused, 90 );
	hooked::original::o_is_hltv               = instance->m_engine->get_function< decltype( &hooked::is_hltv ) >( &hooked::is_hltv_gate, 93 );

	hooked::original::o_emit_sound            = instance->m_engine_sound->get_function< decltype( &hooked::emit_sound ) >( &hooked::emit_sound, 5 );

	hooked::original::o_scene_end             = instance->m_render_view->get_function< decltype( &hooked::scene_end ) >( &hooked::scene_end, 9 );

	hooked::original::o_get_screen_size       = instance->m_surface->get_function< decltype( &hooked::get_screen_size ) >( &hooked::get_screen_size, 44 );
	hooked::original::o_on_screen_size_changed = instance->m_surface->get_function< decltype( &hooked::on_screen_size_changed ) >( &hooked::on_screen_size_changed, 67 );
	//hooked::original::o_lock_cursor         = instance->m_surface->get_function< decltype( &hooked::lock_cursor ) >( &hooked::lock_cursor, 67 );
	hooked::original::o_play_sound            = instance->m_surface->get_function< decltype( &hooked::play_sound ) >( &hooked::play_sound, 82 );

	hooked::original::o_paint_traverse        = instance->m_panel->get_function< decltype( &hooked::paint_traverse ) >( &hooked::paint_traverse, 41 );
	
	hooked::original::o_draw_model_execute    = instance->m_model_render->get_function< decltype( &hooked::draw_model_execute ) >( &hooked::draw_model_execute, 21 ); 
	//hooked::original::o_setup_bones = instance->m_iclient_renderable->get_function<decltype(&hooked::setup_bones)>(&hooked::setup_bones, 13); // this results in fuckery // nvm its my game install or smth

	hooked::original::o_find_model            = instance->m_mdl_cache->get_function< decltype( &hooked::find_model ) >( &hooked::find_model, 10 );

	hooked::original::o_override_cfg          = instance->m_material_system->get_function< decltype( &hooked::override_cfg ) >( &hooked::override_cfg, 21 );

	//hooked::original::o_find_or_add_file      = instance->m_file_system->get_function< decltype( &hooked::find_or_add_file ) >( &hooked::find_or_add_file, 41 );

	//SFHudDeathNoticeAndBotStatus__FireGameEvent
	//hooked::original::o_hdn_firegameevent = instance->m_hdn->get_function< decltype( &hooked::hdn_firegameevent ) >( &hooked::hdn_firegameevent, 1 );

	event_manager.start( csgo.m_event_manager( ) );

	/*
	printf( " /$$$$$$$                                     /$$   /$$ /$$                                     /$$   /$$                     /$$      \n" );
	printf( "| $$__  $$                                   | $$$ | $$|__/                                    | $$  | $$                    | $$      \n" );
	printf( "| $$  \\ $$  /$$$$$$   /$$$$$$$ /$$$$$$$      | $$$$| $$ /$$  /$$$$$$   /$$$$$$   /$$$$$$       | $$  | $$  /$$$$$$   /$$$$$$ | $$   /$$\n" );
	printf( "| $$$$$$$  /$$__  $$ /$$_____//$$_____/      | $$ $$ $$| $$ /$$__  $$ /$$__  $$ |____  $$      | $$$$$$$$ /$$__  $$ /$$__  $$| $$  /$$/\n" );
	printf( "| $$__  $$| $$  \\ $$|  $$$$$$|  $$$$$$       | $$  $$$$| $$| $$  \\ $$| $$  \\ $$  /$$$$$$$      | $$__  $$| $$  \\ $$| $$  \\ $$| $$$$$$/ \n" );
	printf( "| $$  \\ $$| $$  | $$ \\____  $$\\____  $$      | $$\\  $$$| $$| $$  | $$| $$  | $$ /$$__  $$      | $$  | $$| $$  | $$| $$  | $$| $$_  $$ \n" );
	printf( "| $$$$$$$/|  $$$$$$/ /$$$$$$$//$$$$$$$/      | $$ \\  $$| $$|  $$$$$$$|  $$$$$$$|  $$$$$$$      | $$  | $$|  $$$$$$/|  $$$$$$/| $$ \\  $$\n" );
	printf( "|_______/  \\______/ |_______/|_______/       |__/  \\__/|__/ \\____  $$ \\____  $$ \\_______/      |__/  |__/ \\______/  \\______/ |__/  \\__/\n" );
	printf( "                                                            /$$  \\ $$ /$$  \\ $$                                                        \n" );
	printf( "                                                           |  $$$$$$/|  $$$$$$/                                                        \n" );
	printf( "                                                            \\______/  \\______/                                                         \n" );
	*/
}

void factory::release(c_csgo* instance)
{
	event_manager.stop( csgo.m_event_manager( ) );
	instance->release( );
	SetWindowLong( instance->m_window, -4, ( long ) hooked::original::o_wnd_proc );
}
