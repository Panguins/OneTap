#include "context.h"

#include "esp.h"
#include "nade_prediction.h"

decltype( hooked::original::o_present ) hooked::original::o_present;

HRESULT __stdcall hooked::present(IDirect3DDevice9* device, RECT* src, const RECT* dst, HWND dst_window_override, const RGNDATA* dirty_region)
{
	if ( !ctx.m_init )
	{
		ctx.m_renderer  = std::make_unique<c_render>( device );
		ctx.m_init      = true;
	}

	static auto movups_14a781e4 = memory::pattern::first_code_match( csgo.m_client.module( ), xors( "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9" ), 3 );
	ctx.m_matrix = *reinterpret_cast< VMatrix* >( *reinterpret_cast< uintptr_t* >( movups_14a781e4 ) + 176 );

	anti_rin( csgo.m_engine( )->GetScreenSize( ctx.m_screen_w, ctx.m_screen_h ) );

	ctx.m_renderer->start();
	{
		// draw watermark
		{
			// obviously this needs reworking and stuff :p
			// doesn't handle formatted strings, bad system, etc
			auto draw_watermark = [&]( std::vector< std::string > items_horizontal, std::vector< std::string > items_vertical = {} )
			{ 
				std::string final_string{};

				ctx.m_renderer->filled_rect( color_t( 185, 90, 105, 160 ), ctx.m_screen_w - 280.0f, 10.0f, ( ctx.m_screen_w - 280.0f ) + 270.0f, 10.0f + 20.0f );

				for ( size_t i = 0; i < items_horizontal.size( ); ++i )
				{
					if ( i < items_horizontal.size( ) - 1 )
						final_string += items_horizontal[i] + " | ";
					else
						final_string += items_horizontal[i];
				}

				ctx.m_renderer->string( font_t::font_menu, color_t( 255, 255, 255, 150 ), ctx.m_screen_w - 275.0f, 14.0f, false, final_string.c_str( ) );
			};

			std::vector< std::string > watermark_horizontal_items = { xors( "big nigga hook" ) };

			if ( csgo.m_engine( )->IsInGame( ) && ctx.m_local )
			{
				watermark_horizontal_items.push_back( std::string( xors( "rate: " ) + std::to_string( ctx.m_tickrate ) ) );
				watermark_horizontal_items.push_back( std::string( xors( "choke: " ) + std::to_string( csgo.m_client_state( )->chokedcommands ) ) );
				watermark_horizontal_items.push_back( std::string( xors( "shots: " ) + std::to_string( ctx.m_local->shots_fired( ) ) ) );
			}

			draw_watermark( watermark_horizontal_items );

			watermark_horizontal_items.clear( );
		}

		if ( ctx.m_settings.visuals_active || ctx.m_settings.players_esp )
			g_esp.init( );		

		if ( ctx.m_settings.visuals_active )
		{
			if ( ctx.m_settings.visuals_nade_prediction )
				g_grenade_predictor.draw( );

			if ( csgo.m_engine( )->IsInGame( ) && ctx.m_local )
			{
				int elements{ };

				auto get_color = []( const bool& state )
				{
					return state ? color_t( 50, 255, 50 ) : color_t( 255, 50, 50 );
				};

				if ( ctx.m_settings.visuals_indicate_lby )
				{
					const bool state = std::abs( ctx.m_real_angle.y - ctx.m_local->lowerbodyyaw( ) ) > 35.0f;

					ctx.m_renderer->string( font_t::font_bbc, get_color( state ), 150.0f, ( ctx.m_screen_h - 600.0f ) + ( elements++ * 28.0f ), true, xors( "LBY" ) );
				}

				if ( ctx.m_settings.visuals_indicate_ns )
				{
					static const auto weapon_accuracy_nospread = csgo.m_engine_cvars( )->FindVar( xors( "weapon_accuracy_nospread" ) );
					static const auto usercmd_custom_random_seed = csgo.m_engine_cvars( )->FindVar( xors( "sv_usercmd_custom_random_seed" ) );

					const bool state = weapon_accuracy_nospread->get_int( ) > 0 || usercmd_custom_random_seed->get_int( ) == 0;

					ctx.m_renderer->string( font_t::font_bbc, get_color( state ), 150.0f, ( ctx.m_screen_h - 600.0f ) + ( elements++ * 28.0f ), true, xors( "NS" ) );
				}

				if ( ctx.m_settings.visuals_indicate_lc )
				{
					const bool state = ctx.m_lag_handler->breaking_lag_comp( );

					ctx.m_renderer->string( font_t::font_bbc, get_color( state ), 150.0f, ( ctx.m_screen_h - 600.0f ) + ( elements++ * 28.0f ), true, xors( "LC" ) );
				}
			}		
		}

		ctx.m_gui.draw();

		g_esp.lists_init();
	}
	ctx.m_renderer->finish();

	return hooked::original::o_present(device, src, dst, dst_window_override, dirty_region);
}
