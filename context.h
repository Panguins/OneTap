#pragma once

#include <stdint.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <thread>
#include <iterator>
#include <sstream>
#include <algorithm>
#include <functional>

#include "string.h"

#include "renderer.h"

#include <d3d9.h>
#include <d3dx9.h>

#pragma comment( lib, "d3d9.lib" )
#pragma comment( lib, "d3dx9.lib" )

#include "sdk.h"
#include "player.h"
#include "interfaces.h"
#include "memory.h"
#include "math.h"
#include "input.h"
#include "settings.h"
#include "gui.h"
#include "boner.h"
#include "anims.h"

#include "prediction.h"
#include "rage.h"
#include "antiaim.h"

#define anti_rin( items )\
 static bool init = true; \
if ( init ) \
{ \
	items; \
	init = false;\
} \

typedef unsigned long ulong32_t; // c:
typedef HMODULE       module_t;
typedef HANDLE        handle_t;
typedef FILE          file_t;
typedef HWND          hwnd_t;

class c_batcher
{
public:
	void add_call( )
	{
		m_calls++;
	}

	template< typename t >
	void call( std::function< t > func, size_t num_args, ... )
	{
		va_list list;
		va_start( list, num_args );

		if ( num_args )

		for ( size_t i = 0; i < m_calls; i++ )
			num_args ? func( list ) : func( );
	}

private:
	size_t m_calls;
};

class c_lag_handler;

struct c_context
{
	HINSTANCE            m_instance;
	bool                 m_panic;
	bool                 m_init = false;
	bool                 m_menu_open = false;

	c_base_entity*       m_local;
	c_user_cmd*          m_cmd;
	c_user_cmd*          m_last_cmd;
	vec3                 m_real_angle;
	bool                 m_is_anim_tick;
	uint32_t             m_tickrate;

	VMatrix		         m_matrix;
	vec3                 m_camera_pos;
	vec3                 m_camera_ang;

	int                  m_screen_w;
	int                  m_screen_h;
	
	std::unique_ptr< c_lag_handler >  m_lag_handler;
	std::unique_ptr< c_render >       m_renderer;

	c_gui                             m_gui;
	c_settings                        m_settings;

	std::vector< std::string >        m_cfg_list;
	int                               m_current_cfg;

	c_render_context*                 m_render_ctx;

	int  m_missed_shots[128];
}; extern c_context ctx;

class c_lag_handler
{
	enum
	{
		STATE_CHOKE,
		STATE_SEND,
		STATE_WAIT,
	};

public:
	c_lag_handler( bool* send_packet )
	{
		m_send_packet = send_packet;
	}

	inline void do_fakelag( )
	{
		//if ( ctx.m_cmd->m_buttons & IN_ATTACK && ( ctx.m_local->weapon( ) && ctx.m_local->weapon( )->can_fire( ) ) )
		//	return;

		switch ( ctx.m_settings.misc_fakelag_type )
		{
			case 0:
			{
				if ( breaking_lag_comp( ) )				
					reset( );				

				if ( get_choked_ticks( ) < 15 )
					choke( );

				break;
			}

			case 2:
			{
				static  bool   toggle             = true;
				const   float  len                = ctx.m_local->velocity( ).length2d( );
				const   int    choke_to_teleport  = std::ceil( 68.0f / ( len * csgo.m_globals( )->interval_per_tick ) );
				static  int    ideal_choke        = toggle ? math::imports::random( ctx.m_settings.misc_fakelag_rand_min, ctx.m_settings.misc_fakelag_rand_max ) 
												  : choke_to_teleport;

				ideal_choke = std::clamp( ideal_choke, 0, 15 );

				if ( get_choked_ticks( ) < ideal_choke )
				{
					choke( );
				}
				else
				{
					toggle = !toggle;

					ideal_choke = toggle ? math::imports::random( ctx.m_settings.misc_fakelag_rand_min, ctx.m_settings.misc_fakelag_rand_max )
						        : choke_to_teleport;			
					
					reset( );
				}

				break;
			}
		}
	}

	inline void do_fakeland( )
	{
		const   auto  anim_state = ctx.m_local->anim_state( );
		static  int   time_here = 0;

		if ( anim_state->m_hitground_anim )
		{
			// start our timer which will be used to find 
			// if it's desirable to fake land
			time_here++;
		}
		else
		{
			// reset after we've stopped the ground animation, at this point
			// we're either, back in the air, or walking around
			time_here = 0;
		}

		// if we've just hit the ground, and we aren't deep into the animation
		// like would happen if you llanded, and walked around after
		// then, choke, creating 'fake land'
		if ( anim_state->m_hitground_anim && time_here <= 1 /* magic sanity check number */ )
		{
			// tell lag handler to fuck off
			m_fake_landing = true;

			// send, choke, need to force hard choke of 15 ticks, but this is HARD cuz lag manager stuff
			reset( );
			choke( );
		}
		else
		{
			// reset
			m_fake_landing = false;
		}
	}

	inline void init( c_user_cmd* cmd, const vec3& origin, const vec3& last_origin )
	{
		m_cmd           = cmd;
		m_cur_origin    = origin;
		m_ticks_choked  = csgo.m_client_state( )->chokedcommands;

		if ( ctx.m_settings.rage_aimbot_enabled )
		{
			if ( ctx.m_settings.misc_fakelag )
			{
				if ( ctx.m_settings.misc_fakelag_land )
					do_fakeland( );
			}

			if ( ctx.m_settings.misc_fakelag && ctx.m_settings.misc_fakelag_type != 1 )
			{
				if ( ctx.m_settings.misc_fakelag_type != 1 )
					do_fakelag( );
			}
			else
			{
				if ( g_antiaim.should_run( ) )
				{
					if ( ctx.m_cmd->m_cmd_number % 2 == 0 )
						choke( );
					else if ( !m_fake_landing )
						reset( );
				}
			}
		}

		if ( m_ticks_choked >= m_max_choke )
		{
			// force a send
			*m_send_packet = true;
		}

		if ( m_ticks_choked < 1 )
		{
			// last origin server will get
			m_last_origin = m_cur_origin;

			// last matrix the server will get
			// use boner slut
			// sorry
			g_boner.get_latest( ctx.m_local->index( ), m_last_matrix );
		}
	}

	inline void choke( )
	{
		m_tick_choke_started = m_cmd->m_tickcount;
		*m_send_packet = false;
	}

	inline void reset( )
	{
		m_tick_choke_started = -1;
		*m_send_packet = true;
	}

	inline bool choking( )
	{
		// so we cant use this cuz send packet only gets set after all the
		// lag stuff is actually ran, it'd be set in finish
		// need rewrite
		// Fixed ! ! ! 

		return !*m_send_packet;
		return m_ticks_choked > 0;
	}

	inline size_t get_choked_ticks( )
	{
		return m_ticks_choked;
	}

	// idea:
	// return value:
	// 0 - no
	// 1 - yes
	// 2 - will next tick
	inline bool breaking_lag_comp( )
	{
		printf( "%f\n", ( m_cur_origin - m_last_origin ).length2d( ) );
		return ( m_cur_origin - m_last_origin ).length2d( ) > 64.0f;
	}

	inline vec3 get_server_origin( )
	{
		return m_last_origin;
	}

	inline matrix3x4_t* get_server_matrix( )
	{
		if ( !m_last_matrix )
			ctx.m_local->setup_bones( m_last_matrix, 128, BONE_USED_BY_HITBOX, ctx.m_local->simulation_time( ) );

		return m_last_matrix;
	}

private:

	// ...
	size_t        m_ticks_choked;
	bool*         m_send_packet;
	c_user_cmd*   m_cmd;

	// tick we started choking, useful for some stuff i guess
	size_t        m_tick_choke_started;

	// useful for calculations
	vec3          m_last_origin;
	vec3          m_cur_origin;
	matrix3x4_t   m_last_matrix[128];
	bool          m_fake_landing;
	// see CL_Predict
	static constexpr size_t m_max_choke = 15;
};

namespace hooked
{
	extern void     __fastcall    frame_stage_notify( void*, void*, ClientFrameStage_t );
	extern void     __fastcall    shutdown( void*, void* );
	extern bool     __fastcall    should_draw_fog( void* ecx, void* edx );
	extern bool     __fastcall    create_move( void*, void*, float isf, c_user_cmd* cmd );
	extern void     __fastcall    override_view( void*, void*, c_view_setup* view );
	extern void     __fastcall    override_mouse_input( void*, void*, float* x, float* y );
	extern void     __fastcall    emit_sound( void*, void*, void* filter, int index, int sprite_channel, const char* sound_entry, unsigned int sound_entry_hash, const char* name, float volume, float attenuation, int seed, int flags, int pitch_scale, const vec3* origin, const vec3* dir, int idk, bool update_positions, float sound_play_time, int emitting_ent );
	extern void     __fastcall    scene_end( void*, void* );
	extern bool     __fastcall    do_space_effects( void* ecx, void* edx, void* view );
	extern void     __fastcall    get_local_viewangles( void*, void*, vec3& angles );
	extern void     __fastcall    run_command( void*, void*, c_base_entity*, c_user_cmd*, c_move_helper* );
	extern void     __fastcall    post_data_received( void*, void*, int );
	extern bool     __fastcall    override_cfg( void*, void*, material_config_t*, bool );
	extern void     __fastcall    on_screen_size_changed( void* ecx, void* edx, int old_w, int old_h );
	extern bool     __fastcall    is_hltv_gate( void*, void* );
	extern bool     __fastcall    is_hltv( void*, uintptr_t );
	extern bool     __fastcall    is_connected( void*, void* );
	extern void     __fastcall    get_screen_size( void*, void*, int&, int& );
	extern int      __fastcall    post_screen_effects( void*, void*, CViewSetup* );
	extern void     __fastcall    in_prediction( void*, void* );
	extern void     __fastcall    lock_cursor( void*, void* );
	extern void     __fastcall    play_sound( void*, void*, const char* sound );
	extern void     __fastcall    paint_traverse( void*, void*, uint32_t panel, bool repaint, bool allow_force );
	extern void     __fastcall    draw_model_execute( void* ecx, void* edx, c_render_context* context, const int& state, const c_model_render_info& info, const matrix3x4_t* btw );
	extern uint16_t __fastcall    find_model( void* ecx, void* edx, char* );
	extern bool     __fastcall    is_paused( void*, void* );
	extern void     __fastcall    hdn_firegameevent( void* ecx, void* edx, int a2 );
	extern void*    __fastcall    find_or_add_file( void* ecx, void* edx, const char* path );

	extern long     __stdcall     wnd_proc( HWND hwnd, uint32_t msg, uint32_t uparam, long param );
	extern long     __stdcall     present( IDirect3DDevice9* device, RECT* src, const RECT* dst, HWND dst_window_override, const RGNDATA* dirty_region );
	extern long     __stdcall     reset( IDirect3DDevice9* device, void* params );


	namespace original
	{
		extern decltype( &should_draw_fog )                  o_should_draw_fog;
		extern decltype( &create_move )                      o_create_move;
		extern decltype( &override_view )                    o_override_view;
		extern decltype( &override_mouse_input )             o_override_mouse_input;
		extern decltype( &emit_sound )                       o_emit_sound;
		extern decltype( &frame_stage_notify )               o_frame_stage_notify;
		extern decltype( &shutdown )                         o_shutdown;
		extern decltype( &scene_end )                        o_scene_end;
		extern decltype( &get_local_viewangles )		     o_get_local_viewangles;
		extern decltype( &run_command )					     o_run_command;
		extern decltype( &override_cfg )	                 o_override_cfg;
		extern decltype( &post_data_received )	             o_post_data_received;
		extern decltype( &on_screen_size_changed )	         o_on_screen_size_changed;
		extern decltype( &is_hltv )                          o_is_hltv;
		extern decltype( &is_paused )                        o_is_paused;
		extern decltype( &is_connected )                     o_is_connected;
		extern decltype( &get_screen_size )                  o_get_screen_size;
		extern decltype( &post_screen_effects )              o_post_screen_effects;
		extern decltype( &in_prediction )                    o_in_prediction;
		extern decltype( &lock_cursor )						 o_lock_cursor;
		extern decltype( &play_sound )						 o_play_sound;
		extern decltype( &paint_traverse )                   o_paint_traverse;
		extern decltype( &draw_model_execute )               o_draw_model_execute;
		extern decltype( &find_model )                       o_find_model;
		extern decltype( &hdn_firegameevent )                o_hdn_firegameevent;
		extern decltype( &find_or_add_file )                 o_find_or_add_file;
		extern decltype( &wnd_proc )                         o_wnd_proc;
		extern decltype( &present )                          o_present;
		extern decltype( &reset )                            o_reset;
	}
}
namespace drawing
{
	template< class t = vec3 >
	bool screen_transform( const vec3& world, t& screen )
	{
		screen.x = ctx.m_matrix[0][0] * world[0] + ctx.m_matrix[0][1] * world[1] + ctx.m_matrix[0][2] * world[2] + ctx.m_matrix[0][3];
		screen.y = ctx.m_matrix[1][0] * world[0] + ctx.m_matrix[1][1] * world[1] + ctx.m_matrix[1][2] * world[2] + ctx.m_matrix[1][3];
		float w = ctx.m_matrix[3][0] * world[0] + ctx.m_matrix[3][1] * world[1] + ctx.m_matrix[3][2] * world[2] + ctx.m_matrix[3][3];

		bool behind = false;

		if ( w < 0.001f )
		{
			behind = true;
			screen.x *= 100000;
			screen.y *= 100000;
		}
		else
		{
			behind = false;
			float invw = 1.0f / w;
			screen.x *= invw;
			screen.y *= invw;
		}
		return behind;
	}

	template< class t = vec3 >
	bool w2s( const vec3& world, t& screen )
	{
		if ( !screen_transform( world, screen ) )
		{
			float x = float( ctx.m_screen_w ) / 2.0f;
			float y = float( ctx.m_screen_h ) / 2.0f;
			x += 0.5f * screen.x * ctx.m_screen_w + 0.5f;
			y -= 0.5f * screen.y * ctx.m_screen_h + 0.5f;
			screen.x = x;
			screen.y = y;
			return true;
		}
		return false;
	}

	inline bool screen_transform_r( const vec3& world, vec3& screen )
	{
		screen.x = ctx.m_matrix[0][0] * world[0] + ctx.m_matrix[0][1] * world[1] + ctx.m_matrix[0][2] * world[2] + ctx.m_matrix[0][3];
		screen.y = ctx.m_matrix[1][0] * world[0] + ctx.m_matrix[1][1] * world[1] + ctx.m_matrix[1][2] * world[2] + ctx.m_matrix[1][3];

		float w = ctx.m_matrix[3][0] * world[0] + ctx.m_matrix[3][1] * world[1] + ctx.m_matrix[3][2] * world[2] + ctx.m_matrix[3][3];
		screen.z = 0.0f;

		bool behind = false;

		float invw = 1.0f / w;
		screen.x *= invw;
		screen.y *= invw;
		return behind;
	}

	inline bool w2r( const vec3& world, vec3& screen )
	{
		if ( !screen_transform_r( world, screen ) )
		{
			float x = float( ctx.m_screen_w ) / 2.0f;
			float y = float( ctx.m_screen_h ) / 2.0f;
			x += 0.5f * screen.x * ctx.m_screen_w + 0.5f;
			y -= 0.5f * screen.y * ctx.m_screen_h + 0.5f;
			screen.x = x;
			screen.y = y;
			return true;
		}
		return false;
	}

	inline void circle3d( vec3 pos, color_t color, int point_count, float radius, bool fade = false, float rot_start = 0.f, float fade_start = 0.5f, float fade_length = 0.25f )
	{
		float step = math::pi * 2.0f / point_count;
		std::vector<vec3> points3d;

		int alpha = 255;
		int fade_start_point = 0;
		int fade_end_point = 0;
		int fade_step = 0;
		if ( fade )
		{
			fade_start_point = (int)( point_count * fade_start );
			fade_end_point = fade_start_point + (int)( point_count * fade_length );
			fade_step = 255 / ( fade_end_point - fade_start_point );
		}

		color_t outer_color = color_t( 0, 0, 0, 255 );
		color_t inner_color = color;

		for ( int i = 0; i < point_count; i++ )
		{
			if ( fade && i > fade_end_point )
				break;

			float theta = ( i * step ) - ( math::pi * 2.f * rot_start );

			vec3 world_start( radius * cosf( theta ) + pos.x, radius * sinf( theta ) + pos.y, pos.z );
			vec3 world_end( radius * cosf( theta + step ) + pos.x, radius * sinf( theta + step ) + pos.y, pos.z );

			vec2 start, end;
			if ( !w2s( world_start, start ) || !w2s( world_end, end ) )
				return;

			if ( fade && i >= fade_start_point )
			{
				alpha -= fade_step;

				if ( alpha < 0 )
					alpha = 0;
			}

			outer_color.a( alpha );
			inner_color.a( alpha );

			ctx.m_renderer->line( outer_color, start.x, start.y + 1, end.x, end.y + 1 );
			ctx.m_renderer->line( outer_color, start.x, start.y - 1, end.x, end.y - 1 );
			ctx.m_renderer->line( inner_color, start.x, start.y, end.x, end.y );
		}
	}

}