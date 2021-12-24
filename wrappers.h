#pragma once
#include "context.h"
#include "renderer.h"

namespace UI
{
	struct UIcolor_t
	{
		uint8_t R = 255;
		uint8_t G = 255;
		uint8_t B = 255;
		uint8_t A = 255;

		constexpr UIcolor_t( ) : R( 0 ), G( 0 ), B( 0 ), A( 0 ) { }

		constexpr UIcolor_t( uint8_t r, uint8_t g, uint8_t b, uint8_t a ) : R( r ), G( g ), B( b ), A( a ) { }

		constexpr UIcolor_t( uint8_t r, uint8_t g, uint8_t b ) : R( r ), G( g ), B( b ), A( 255 ) { }

		__forceinline color_t to_renderer_color( ) const
		{
			return color_t( R, G, B, A );
		}

		__forceinline static UIcolor_t get_rainbow( const float& alpha, const float& speed = 0.0005f )
		{
			static float hue_cycle{};
			UIcolor_t ret;

			hue_cycle = std::fmod( hue_cycle + speed, 1.0f );

			ret = hsb_to_rgb( hue_cycle, 0.7f, 1.0f );

			return ret;
		}

		__forceinline static UIcolor_t get_rainbow( const float& alpha, float& hue_cycle )
		{
			UIcolor_t ret;

			hue_cycle = std::fmod( hue_cycle + 0.003f, 1.0f );

			ret = hsb_to_rgb( hue_cycle, 0.7f, 1.0f );

			return ret;
		}

		__forceinline static UIcolor_t get_sinbow( )
		{
			static float cycle{};
			UIcolor_t ret;

			cycle = std::fmod( cycle + 0.003f, 3.0f );

			ret.R = cosf( cycle ) * .5f + .5f;
			ret.G = cosf( cycle - 2.f * math::pi / 3.f ) * .5f + .5f;
			ret.B = cosf( cycle - 4.f * math::pi / 3.f ) * .5f + .5f;

			ret.R *= 255.f;
			ret.G *= 255.f;
			ret.B *= 255.f;

			return ret;
		}

		__forceinline static UIcolor_t hsb_to_rgb( float hue, float saturation, float brightness )
		{
			float h = hue == 1.0f ? 0 : hue * 6.0f;
			float f = h - ( int ) h;
			float p = brightness * ( 1.0f - saturation );
	 		float q = brightness * ( 1.0f - saturation * f );
			float t = brightness * ( 1.0f - saturation * ( 1.0f - f ) );

			if ( h < 1 )
			{
				return UIcolor_t(
					( uint8_t ) ( brightness * 255 ),
					( uint8_t ) ( t * 255 ),
					( uint8_t ) ( p * 255 )
				);
			}
			if ( h < 2 )
			{
				return UIcolor_t(
					( uint8_t ) ( q * 255 ),
					( uint8_t ) ( brightness * 255 ),
					( uint8_t ) ( p * 255 )
				);
			}
			if ( h < 3 )
			{
				return UIcolor_t(
					( uint8_t ) ( p * 255 ),
					( uint8_t ) ( brightness * 255 ),
					( uint8_t ) ( t * 255 )
				);
			}
			if ( h < 4 )
			{
				return UIcolor_t(
					( uint8_t ) ( p * 255 ),
					( uint8_t ) ( q * 255 ),
					( uint8_t ) ( brightness * 255 )
				);
			}
			if ( h < 5 )
			{
				return UIcolor_t(
					( uint8_t ) ( t * 255 ),
					( uint8_t ) ( p * 255 ),
					( uint8_t ) ( brightness * 255 )
				);
			}
			return UIcolor_t(
				( uint8_t ) ( brightness * 255 ),
				( uint8_t ) ( p * 255 ),
				( uint8_t ) ( q * 255 )
			);
		}
	};

	__forceinline void filled_rect( UIcolor_t color, float x, float y, float w, float h )
	{
		color_t renderer_color = color.to_renderer_color( );

		ctx.m_renderer->filled_rect( renderer_color, x, y, w, h );
	}

	__forceinline void _filled_rect( UIcolor_t color, float x, float y, float w, float h )
	{
		color_t renderer_color = color.to_renderer_color( );

		ctx.m_renderer->filled_rect( renderer_color, x, y, x + w, y + h );
	}

	__forceinline void rect_gradient( UIcolor_t top, UIcolor_t bottom, float x, float y, float w, float h )
	{
		color_t renderer_top = top.to_renderer_color( );
		color_t renderer_bottom = bottom.to_renderer_color( );

		ctx.m_renderer->rect_gradient( gradient_t::vertical, renderer_top, renderer_bottom, x, y, w, h );
	}

	__forceinline void skeet_rect( const float& alpha, const float& x0, const float& y0, const float& x1, const float& y1 )
	{

	}

	__forceinline void line( UIcolor_t color, float x0, float y0, float x1, float y1 )
	{
		color_t renderer_color = color.to_renderer_color( );

		ctx.m_renderer->line( renderer_color, x0, y0, x1, y1 );
	}

	static void lily_string( UIcolor_t color, float x, float y, bool centered, const char* fmt, ... )
	{
		char buffer[256];
		va_list args;
		va_start( args, fmt );
		vsprintf( buffer, fmt, args );
		va_end( args );

		const font_t font = font_t::font_bbc;

		color_t renderer_color = color.to_renderer_color( );

		ctx.m_renderer->string( font, renderer_color, x, y, centered, buffer );
	}

	static void string( UIcolor_t color, float x, float y, bool centered, const char* fmt, ... )
	{
		char buffer[256];
		va_list args;
		va_start( args, fmt );
		vsprintf( buffer, fmt, args );
		va_end( args );

		const font_t font = font_t::font_menu; // Tahoma | NONANTIALIASED_QUALITY | FW_NORMAL | 12

		color_t renderer_color = color.to_renderer_color( );

		ctx.m_renderer->string( font, renderer_color, x, y, centered, buffer );
	}

	static void string( UIcolor_t color, float x, float y, bool centered, const wchar_t* fmt, ... )
	{
		wchar_t buffer[256];
		va_list args;
		va_start( args, fmt );
		vswprintf( buffer, 256, fmt, args );
		va_end( args );

		const font_t font = font_t::font_menu; // Tahoma | NONANTIALIASED_QUALITY | FW_NORMAL | 12

		color_t renderer_color = color.to_renderer_color( );

		ctx.m_renderer->wide_string( font, renderer_color, x, y, centered, buffer );
	}

	static float text_width( const char* fmt, ...)
	{
		char buffer[256];
		va_list args;
		va_start( args, fmt );
		vsprintf( buffer, fmt, args );
		va_end( args );

		const font_t font = font_t::font_menu; // Tahoma | NONANTIALIASED_QUALITY | FW_NORMAL | 12

		return ctx.m_renderer->text_width( font, buffer );
	}

	namespace util
	{
		static inline int screen_width( )
		{
			return ctx.m_screen_w;
		}

		static inline int screen_height( )
		{
			return ctx.m_screen_h;
		}

		enum mouse_state_t
		{
			MOUSE_INACTIVE,
			MOUSE_ACTIVE
		};
		static inline void set_mouse_state( const bool& state )
		{
			static const auto cl_mouseenable = csgo.m_engine_cvars( )->FindVar( xors( "cl_mouseenable" ) );

			cl_mouseenable->set_value( state );
		}
	}
}