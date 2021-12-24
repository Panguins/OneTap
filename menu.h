#pragma once
#include "context.h"
#include "wrappers.h"

// changes to improve and not make it 1:1 supremacy

// tabs at top with tiny bar underneath them
// skeet bar at top
// 3 columns
// hover/highlight animation on items
// colorpicker stays open
// textured background
// tab in animations: all controls fade in, sliders fill up to their position
// checkboes fill up from center when pressed
// dropdown closing animation

namespace UI
{
	class c_cursor
	{
		int    cursor_x{ }, cursor_y{ };
		float  hue_cycle{ };
	public:
		void handle( )
		{
			g_input.get_cursor_pos( cursor_x, cursor_y );

			hue_cycle = std::clamp<float>( hue_cycle + 0.002f, 0.f, 1.f );

			draw( );
		}

		void draw( )
		{
			const UIcolor_t outer_color( 3, 6, 26, 215 );
			const UIcolor_t inner_color( 255, 255, 255, 255 ); //color_t::FromHSB( hue_cycle, 1.0f, 1.0f )

#pragma warning( push )
#pragma warning( disable : 4244 )

			UI::_filled_rect( outer_color, cursor_x + 1.0f, cursor_y, 1, 17 );

			for ( int i = 0; i < 11; i++ )
				UI::_filled_rect( outer_color, cursor_x + 2 + i, cursor_y + 1 + i, 1, 1 );

			UI::_filled_rect( outer_color, cursor_x + 8, cursor_y + 12, 5, 1 );
			UI::_filled_rect( outer_color, cursor_x + 8, cursor_y + 13, 1, 1 );
			UI::_filled_rect( outer_color, cursor_x + 9, cursor_y + 14, 1, 2 );
			UI::_filled_rect( outer_color, cursor_x + 10, cursor_y + 16, 1, 2 );
			UI::_filled_rect( outer_color, cursor_x + 8, cursor_y + 18, 2, 1 );
			UI::_filled_rect( outer_color, cursor_x + 7, cursor_y + 16, 1, 2 );
			UI::_filled_rect( outer_color, cursor_x + 6, cursor_y + 14, 1, 2 );
			UI::_filled_rect( outer_color, cursor_x + 5, cursor_y + 13, 1, 1 );
			UI::_filled_rect( outer_color, cursor_x + 4, cursor_y + 14, 1, 1 );
			UI::_filled_rect( outer_color, cursor_x + 3, cursor_y + 15, 1, 1 );
			UI::_filled_rect( outer_color, cursor_x + 2, cursor_y + 16, 1, 1 );

			for ( int i = 0; i < 4; i++ )
				UI::_filled_rect( inner_color, cursor_x + 2 + i, cursor_y + 2 + i, 1, 14 - ( i * 2 ) );

			UI::_filled_rect( inner_color, cursor_x + 6, cursor_y + 6, 1, 8 );
			UI::_filled_rect( inner_color, cursor_x + 7, cursor_y + 7, 1, 9 );

			for ( int i = 0; i < 4; i++ )
				UI::_filled_rect( inner_color, cursor_x + 8 + i, cursor_y + 8 + i, 1, 4 - i );

			UI::_filled_rect( inner_color, cursor_x + 8, cursor_y + 14, 1, 4 );
			UI::_filled_rect( inner_color, cursor_x + 9, cursor_y + 16, 1, 2 );

#pragma warning( pop )

		}
	};

	class c_control
	{
	public:
		vec2 m_start;
		vec2 m_bounds;
		vec2 m_end;

		int m_cursor_x;
		int m_cursor_y;
		float m_drag_x;
		float m_drag_y;

		float m_alpha = 255;

		bool m_dragging;
		bool m_should_draw = true;
		bool m_is_inside;

		int m_tab;
		size_t m_current_tab;

		__forceinline bool is_inside( const float& x, const float& y, const float& w, const float& h )
		{
			return m_cursor_x > x && m_cursor_x < x + w && m_cursor_y > y && m_cursor_y < y + h + 1.0f;
		}

		__forceinline bool __is_inside( const float& x, const float& y, const float& w, const float& h )
		{
			return m_cursor_x > x && m_cursor_x < w && m_cursor_y > y && m_cursor_y < h + 1.0f;
		}
	};

	class c_window_data
	{
	public:
		bool m_first_click{ };
		bool m_left_click{ };
		bool m_right_click{ };
		bool m_ignore{ };

		float m_x{ };
		float m_y{ };

		UIcolor_t m_color = UIcolor_t( 181, 0, 41, 255 );
		int m_alpha = 0;
		float m_animation_progress = 0.f;
	};

	class c_window : public c_control
	{
		bool m_is_open{ };
		bool m_was_open{ };
		bool m_should_open = true;

		vec2 m_window_size{ };
		c_window_data m_data{ };
		const float   m_edge_size = 8.f;
		bool          m_is_inside_edge{ };

		void draw_box( float x, float y, float w, float h ) const
		{
			// draw black background
			UI::filled_rect( UIcolor_t( 1, 1, 1, m_data.m_alpha ), x, y, x + w, y + h );

			// draw grey edge
			UI::filled_rect( UIcolor_t( 44, 44, 44, m_data.m_alpha ), x + 1.f, y + 1.f, x + w - 1.f, y + h - 1.f );

			// draw inner box
			UI::filled_rect( UIcolor_t( 9, 9, 9, m_data.m_alpha ), x + 2.f, y + 2.f, x + w - 2.f, y + h - 2.f );
		};

		void draw_watermark ( float x, float y, float w, float h )
		{
			const UIcolor_t text_shadow( 0, 0, 0, m_data.m_alpha );
			const UIcolor_t text_color( 206, 206, 206, m_data.m_alpha );

			std::string time = xors( __DATE__ );
			std::string username = xors( "SenatorII" );
			std::string watermark = time + xors( " | " ) + username;

			float text_w = UI::text_width( watermark.c_str( ) );

			float text_x = x + w - text_w - 5.f;
			float text_y = y + h - 16.f;

			UI::string( text_shadow, text_x + 1.f, text_y + 1.f, false, watermark.c_str( ) );
			UI::string( text_color, text_x, text_y, false, watermark.c_str( ) );
		};

	public:
		c_window( float x = 100.f, float y = 100.f, float w = 482.f, float h = 500.f )
		{
			m_start = vec2( x , y );
			m_window_size = vec2( w, h );
			m_bounds = vec2( w + 40.f, h + 60.f );
			m_end = m_start + m_bounds;
		}

		bool is_open( ) const { return m_is_open; }

		bool handle( std::vector<std::string>& tabs, UIcolor_t color )
		{
			if ( g_input.is_key_pressed( KEYS_INSERT ) )
			{
				if ( !m_was_open )
				{
					m_should_open = !m_should_open;
					UI::util::set_mouse_state( !m_should_open );
				}

				m_was_open = true;
			}
			else
				m_was_open = false;

			if ( ctx.m_settings.menu_fade_speed > 0 )
				m_data.m_alpha = std::clamp<int>( m_should_open ? ( m_data.m_alpha + ctx.m_settings.menu_fade_speed ) : ( m_data.m_alpha - ctx.m_settings.menu_fade_speed ), 0, 255 );
			else
				m_data.m_alpha = std::clamp<int>( m_should_open ? ( m_data.m_alpha + 255 ) : ( m_data.m_alpha - 255 ), 0, 255 ); // sorry

			if ( ctx.m_settings.menu_rainbow )			
				ctx.m_settings.menu_color( ) = ( m_data.m_color = UI::UIcolor_t::get_rainbow( m_data.m_alpha ) ).to_renderer_color( );			
			else
				m_data.m_color = color;

			m_data.m_color.A = m_data.m_alpha;

			m_is_open = m_data.m_alpha > 0;
			if ( !m_is_open )
				return false;

			g_input.get_cursor_pos( m_cursor_x, m_cursor_y );

			m_is_inside = m_cursor_x > m_start.x && m_cursor_x < m_start.x + m_bounds.x && m_cursor_y > m_start.y && m_cursor_y < m_start.y + m_bounds.y;
			m_is_inside_edge = m_is_inside && ( ( m_cursor_x > m_start.x + m_bounds.x - m_edge_size || m_cursor_x < m_start.x + m_edge_size ) || ( m_cursor_y > m_start.y + m_bounds.y - m_edge_size || m_cursor_y < m_start.y + m_edge_size ) );
			
			// update data
			m_data.m_first_click = !m_data.m_left_click && g_input.is_key_pressed( KEYS_MOUSE1 );
			m_data.m_left_click = g_input.is_key_pressed( KEYS_MOUSE1 );
			m_data.m_right_click = g_input.is_key_pressed( KEYS_MOUSE2 );
			m_data.m_animation_progress = std::clamp<float>( m_data.m_animation_progress + 0.05f, 0.f, 1.f );

			if ( !m_data.m_left_click )
				m_data.m_ignore = false;

			if ( m_dragging && !m_data.m_left_click )
				m_dragging = false;

			if ( m_data.m_first_click && m_is_inside_edge )
			{
				m_dragging = true;
				m_drag_x = m_cursor_x - m_start.x;
				m_drag_y = m_cursor_y - m_start.y;
			}

			if ( m_dragging )
			{
				m_start.x = std::clamp( static_cast< float >( m_cursor_x - m_drag_x ), 0.0f, static_cast< float >( UI::util::screen_width() ) - m_bounds.x );			
				m_start.y = std::clamp( static_cast< float >( m_cursor_y - m_drag_y ), 0.0f, static_cast< float >( UI::util::screen_height( ) ) - m_bounds.y );
			}


			m_end = m_start + m_bounds;

			// very transparent background
			{
				// i also want to draw a texture here, cheat logo or something, instead of text here v
				UI::lily_string( UIcolor_t( 1.0f, 1.0f, 1.0f, 0.5f * m_data.m_alpha ), 100.0f, 10.0f, false, "placeholderbot" );
				UI::filled_rect( UIcolor_t( 1, 1, 1, 0.5f * m_data.m_alpha ), 0.0f, 0.0f, UI::util::screen_width( ), UI::util::screen_height( ) );
			}

			// draw black background
			UI::filled_rect( UIcolor_t( 1, 1, 1, m_data.m_alpha ), m_start.x, m_start.y, m_end.x, m_end.y );

			// draw outer light grey box
			UI::filled_rect( UIcolor_t( 57, 57, 57, m_data.m_alpha ), m_start.x + 1.f, m_start.y + 1.f, m_end.x - 1.f, m_end.y - 1.f );

			// draw dark grey box
			UI::filled_rect( UIcolor_t( 35, 35, 35, m_data.m_alpha ), m_start.x + 2.f, m_start.y + 2.f, m_end.x - 2.f, m_end.y - 2.f );

			// draw inner light grey box
			UI::filled_rect( UIcolor_t( 57, 57, 57, m_data.m_alpha ), m_start.x + 5.f, m_start.y + 5.f, m_end.x - 5.f, m_end.y - 5.f );

			// draw main menu background
			UI::filled_rect( UIcolor_t( 4, 4, 4, m_data.m_alpha ), m_start.x + 6.f, m_start.y + 6.f, m_end.x - 6.f, m_end.y - 6.f );

			// draw skeet bar
			UI::skeet_rect( m_data.m_alpha, m_start.x + 6.0f, m_start.y + 6.0f, m_end.x - 6.0f, m_start.y + 8.0f );

			// draw main box
			draw_box( m_start.x + 20.f, m_start.y + 40.f, m_window_size.x, m_window_size.y );

			// draw date/username
			draw_watermark( m_start.x + 20.f, m_start.y + 40.f, m_window_size.x, m_window_size.y );

			// tabz
			const UIcolor_t text_color( 153, 153, 153, m_data.m_alpha );
			const UIcolor_t text_color_shadow( 0, 0, 0, m_data.m_alpha );

			const UIcolor_t tab_color ( 36, 36, 36, m_data.m_alpha );
			const UIcolor_t tab_color_shadow ( 0, 0, 0, m_data.m_alpha / 2 );
			const UIcolor_t tab_color_selected = m_data.m_color;
			UIcolor_t tab_color_hovered = tab_color_selected;
			tab_color_hovered.A /= 2;

			// spacing per tab
			float tab_spacing = 3.f;

			// total used by spacing
			float spacing = ( tabs.size( ) ) * tab_spacing; 

			// tab width
			float tab_width = ( m_window_size.x - spacing ) / tabs.size( );

			for ( size_t i = 0; i < tabs.size( ); i++ )
			{
				float tab_x = m_start.x + 20.f + ( tab_width + tab_spacing ) * i;
				float tab_y = m_start.y + 30.f;

				float text_x = tab_x + ( tab_width * 0.5f );
				float text_y = tab_y - 14.f;

				UI::string( text_color_shadow, text_x + 1.f, text_y + 1.f, true, tabs.at( i ).c_str( ) );
				UI::string( text_color, text_x, text_y, true, tabs.at( i ).c_str( ) );

				UI::filled_rect( m_current_tab == i ? tab_color_selected : tab_color, tab_x, tab_y, tab_x + tab_width, tab_y + 4.f );
				UI::filled_rect( tab_color_shadow, tab_x, tab_y + 2.f, tab_x + tab_width, tab_y + 4.f );


				if ( is_inside( tab_x, tab_y - 16.f, tab_width, 20.f ) && m_current_tab != i )
				{
					//select it if we are inside it and pressing
					if ( m_data.m_first_click )
					{
						m_current_tab = i;
						m_data.m_animation_progress = 0.f;
					}
					else
					{
						UI::filled_rect( tab_color_hovered, tab_x, tab_y, tab_x + tab_width, tab_y + 4.f );
						UI::filled_rect( tab_color_shadow, tab_x, tab_y + 2.f, tab_x + tab_width, tab_y + 4.f );
					}
				}
			}

			return true;
		}

		void column_left ( )
		{
			m_data.m_x = m_start.x + 50.f;
			m_data.m_y = m_start.y + 60.f;
		}

		void column_right( )
		{
			m_data.m_x = m_start.x + 280.f;
			m_data.m_y = m_start.y + 60.f;
		}

		c_window_data* data()
		{
			return &m_data;
		}
	};

	class c_groupbox : public c_control
	{
		const char* m_text { };

	public:
		void start( c_window_data* data, const char* text )
		{
			m_text = text;
			m_start = vec2( data->m_x - 10.f, data->m_y + 5.f );

			// setup coord for next item
			data->m_y += 15.f;
		}

		void end( c_window_data* data )
		{
			m_end = vec2( data->m_x - 10.f, data->m_y + 5.f );

			// setup coord for next item
			data->m_y += 15.f;

			draw( data );
		}

		void draw( c_window_data* data )
		{
			UIcolor_t text_color( 206, 206, 206, data->m_alpha );
			UIcolor_t shadow_color( 0, 0, 0, data->m_alpha );
			UIcolor_t edge_color( 44, 44, 44, data->m_alpha );

			float width = 211.f;

			float text_width = UI::text_width( m_text );

			// top
			UI::line( edge_color, m_start.x, m_start.y, m_start.x + 8.f, m_start.y );
			UI::line( edge_color, m_start.x + 12.f + text_width, m_start.y, m_start.x + width, m_start.y );
			// left
			UI::line( edge_color, m_start.x, m_start.y, m_end.x, m_end.y );
			// right
			UI::line( edge_color, m_start.x + width, m_start.y, m_end.x + width, m_end.y + 1.f );
			UI::line( shadow_color, m_start.x + width + 1.f, m_start.y, m_end.x + width + 1.f, m_end.y + 1.f );
			// bottom
			UI::line( edge_color, m_end.x, m_end.y, m_end.x + width + 1.f, m_end.y );
			UI::line( shadow_color, m_end.x, m_end.y + 1.f, m_end.x + width + 2.f, m_end.y + 1.f);

			if ( m_text )
			{
				UI::string( shadow_color, m_start.x + 10.f + 1.f, m_start.y + 1.f - 6.f, false, m_text );
				UI::string( text_color, m_start.x + 10.f, m_start.y - 6.f, false, m_text );
			}
		}
	};

	class c_label : public c_control
	{
		const char* m_text { };
	public:
		c_label( )
		{

		}

		void draw( c_window_data* data )
		{
			UIcolor_t text_color ( 206, 206, 206, data->m_alpha );
			UIcolor_t text_shadow ( 0, 0, 0, data->m_alpha );

			if ( m_text )
			{
				UI::string( text_shadow, m_start.x + 20.f + 1.f, m_start.y + 1.f - 3.f, false, m_text );
				UI::string( text_color, m_start.x + 20.f, m_start.y - 3.f, false, m_text );
			}
		}

		void handle( c_window_data* data, const char* text )
		{
			m_text = text;
			m_start = vec2( data->m_x, data->m_y );

			// setup coord for next item
			data->m_y += 15.f;

			draw( data );
		}
	};

	class c_checkbox : public c_control
	{
		const char* m_text{ };
		bool* m_value{ };
	public:
		c_checkbox( )
		{
			m_bounds = vec2( 8, 8 );
		}

		void draw( c_window_data* data )
		{
			UIcolor_t text_color ( 206, 206, 206, data->m_alpha );
			UIcolor_t text_shadow ( 0, 0, 0, data->m_alpha );

			if ( m_text )
			{
				UI::string( text_shadow, m_start.x + 20.f + 1.f, m_start.y + 1.f - 3.f, false, m_text );
				UI::string( text_color, m_start.x + 20.f, m_start.y - 3.f, false, m_text );
			}

			// outer edge
			UI::filled_rect( UIcolor_t( 0, 0, 0, data->m_alpha ), m_start.x, m_start.y, m_start.x + m_bounds.x, m_start.y + m_bounds.y );

			// inner box empty
			UIcolor_t empty_color_top = UIcolor_t( 69, 69, 69, data->m_alpha );
			UIcolor_t empty_color_bottom
			(
				empty_color_top.R > 25 ? empty_color_top.R - 25 : 0,
				empty_color_top.G > 25 ? empty_color_top.G - 25 : 0,
				empty_color_top.B > 25 ? empty_color_top.B - 25 : 0,
				data->m_alpha
			);

			UI::rect_gradient( empty_color_top, empty_color_bottom, m_start.x + 1.f, m_start.y + 1.f, m_bounds.x - 2.f, m_bounds.y - 2.f );

			// inner box
			if ( *m_value )
			{
				int anim_alpha = ( float( data->m_alpha ) * data->m_animation_progress );

				UIcolor_t checkbox_color_top = data->m_color;
				UIcolor_t checkbox_color_bottom
				(
					checkbox_color_top.R > 25 ? checkbox_color_top.R - 25 : 0,
					checkbox_color_top.G > 25 ? checkbox_color_top.G - 25 : 0,
					checkbox_color_top.B > 25 ? checkbox_color_top.B - 25 : 0,
					anim_alpha
				);

				UI::rect_gradient( checkbox_color_top, checkbox_color_bottom, m_start.x + 1.f, m_start.y + 1.f, m_bounds.x - 2.f, m_bounds.y - 2.f );
			}
		}

		void handle( c_window_data* data, const char* name, bool* setting )
		{
			g_input.get_cursor_pos( m_cursor_x, m_cursor_y );

			m_text = name;
			m_value = setting;

			m_start = vec2( data->m_x, data->m_y );
			m_end = m_start + m_bounds;

			// setup coord for next item
			data->m_y += 15.f;

			float text_width = m_text ? UI::text_width( m_text ) : 0.f;

			m_is_inside = m_cursor_x > m_start.x && m_cursor_x < m_start.x + 20.f + text_width + 5.f && m_cursor_y > m_start.y - 3.f && m_cursor_y < m_end.y + 3.f;

			if ( m_value && data->m_left_click && !data->m_ignore && m_is_inside )
			{
				*m_value = !*m_value;
				data->m_ignore = true;
			}

			draw( data );
		}
	};

	class c_checkbox_smoll : public c_control
	{
		bool* m_value{ };
	public:
		c_checkbox_smoll( )
		{
			m_bounds = vec2( 8, 8 );
		}

		void draw( c_window_data* data )
		{
			// outer edge
			UI::filled_rect( UIcolor_t( 0, 0, 0, data->m_alpha ), m_start.x, m_start.y, m_start.x + m_bounds.x, m_start.y + m_bounds.y );

			// inner box empty
			UIcolor_t empty_color_top = UIcolor_t( 69, 69, 69, data->m_alpha );
			UIcolor_t empty_color_bottom
			(
				empty_color_top.R > 25 ? empty_color_top.R - 25 : 0,
				empty_color_top.G > 25 ? empty_color_top.G - 25 : 0,
				empty_color_top.B > 25 ? empty_color_top.B - 25 : 0,
				data->m_alpha
			);

			UI::rect_gradient( empty_color_top, empty_color_bottom, m_start.x + 1.f, m_start.y + 1.f, m_bounds.x - 2.f, m_bounds.y - 2.f );

			// inner box
			if ( *m_value )
			{
				int anim_alpha = ( float( data->m_alpha ) * data->m_animation_progress );

				UIcolor_t checkbox_color_top = data->m_color;
				UIcolor_t checkbox_color_bottom
				(
					checkbox_color_top.R > 25 ? checkbox_color_top.R - 25 : 0,
					checkbox_color_top.G > 25 ? checkbox_color_top.G - 25 : 0,
					checkbox_color_top.B > 25 ? checkbox_color_top.B - 25 : 0,
					anim_alpha
				);

				UI::rect_gradient( checkbox_color_top, checkbox_color_bottom, m_start.x + 1.f, m_start.y + 1.f, m_bounds.x - 2.f, m_bounds.y - 2.f );
			}
		}

		void handle( c_window_data* data, bool* setting )
		{
			g_input.get_cursor_pos( m_cursor_x, m_cursor_y );

			m_value = setting;

			m_start = vec2( data->m_x, data->m_y );
			m_end = m_start + m_bounds;

			m_is_inside = m_cursor_x > m_start.x && m_cursor_x < m_start.x + 20.f + 100.f && m_cursor_y > m_start.y - 3.f && m_cursor_y < m_end.y + 3.f;

			if ( m_value && data->m_left_click && !data->m_ignore && m_is_inside )
			{
				*m_value = !*m_value;
				data->m_ignore = true;
			}

			draw( data );
		}
	};

	class c_slider : public c_control
	{
		int* m_value;
		int m_min;
		int m_max;

		const char* m_text;
		const wchar_t* m_suffix;

		vec2 m_slider_start;
		float m_slider_pos;

		float m_slider_width;
		float m_slider_height;

	public:
		c_slider( )
		{
			m_slider_width = 150.f;
			m_slider_height = 6.f;

			m_bounds = vec2( m_slider_width, m_slider_height );
		}

	public:
		void draw( c_window_data* data )
		{
			if ( m_text )
			{
				const UIcolor_t text_color ( 206, 206, 206, data->m_alpha );
				const UIcolor_t text_shadow ( 0, 0, 0, data->m_alpha );

				UI::string( text_shadow, m_start.x + 20.f + 1.f, m_start.y + 1.f - 3.f, false, m_text );
				UI::string( text_color, m_start.x + 20.f, m_start.y - 3.f, false, m_text );
			}

			// outer edge
			UI::filled_rect( UIcolor_t( 0, 0, 0, data->m_alpha ), m_slider_start.x - 1.f, m_slider_start.y - 1.f, m_end.x + 1.f, m_end.y + 1.f );

			// inner background
			UIcolor_t background_color_top ( 69, 69, 69, data->m_alpha );
			UIcolor_t background_color_bottom ( 44, 44, 44, data->m_alpha );

			UI::rect_gradient( background_color_top, background_color_bottom, m_slider_start.x, m_slider_start.y, m_bounds.x, m_bounds.y );

			// slider bar
			UIcolor_t slider_color_top = data->m_color;  //should be menu colour setting
			UIcolor_t slider_color_bottom
				(
					slider_color_top.R > 25 ? slider_color_top.R - 25 : 0,
					slider_color_top.G > 25 ? slider_color_top.G - 25 : 0,
					slider_color_top.B > 25 ? slider_color_top.B - 25 : 0
				);

			UI::rect_gradient( slider_color_top, slider_color_bottom, m_slider_start.x, m_slider_start.y, m_slider_pos, m_bounds.y );

			std::wstring text_value( std::wstring( L"%d" ) + std::wstring( m_suffix ) );
			UI::string( UIcolor_t( 0, 0, 0, data->m_alpha ), m_slider_start.x + m_slider_pos + 1.f, m_slider_start.y + 1.f, true, text_value.c_str( ), *m_value );
			UI::string( UIcolor_t( 255, 255, 255, data->m_alpha ), m_slider_start.x + m_slider_pos, m_slider_start.y, true, text_value.c_str( ), *m_value );
		}

		void handle( c_window_data* data, const char* name, int* setting, int min, int max, const wchar_t* suffix = L"" )
		{
			g_input.get_cursor_pos( m_cursor_x, m_cursor_y );

			m_text = strlen( name ) > 1 ? name : nullptr;
			m_min = min;
			m_max = max;

			m_value = setting;
			m_suffix = suffix;

			m_start = vec2( data->m_x, data->m_y );
			m_slider_start = vec2( m_start.x + 20.f, m_start.y );
			m_end = m_slider_start + m_bounds;

			data->m_y += 15.f;

			if ( m_text )
			{
				m_slider_start.y += 15.f;
				m_end.y += 15.f;
				data->m_y += 15.f;
			}

			m_is_inside = m_cursor_x > m_slider_start.x - 1.0f && m_cursor_x < m_end.x + 1.0f && m_cursor_y > m_slider_start.y - 1.0f && m_cursor_y < m_end.y + 1.0f;


			if ( m_dragging && !data->m_left_click )
				m_dragging = false;

			if ( m_value )
			{
				*m_value = std::clamp< int >( *m_value, m_min, m_max );

				if ( m_dragging )
				{
					float dragged_pos = std::clamp< float >( m_cursor_x - m_slider_start.x, 0, m_slider_width );
					*m_value = m_min + ( ( m_max - m_min ) * dragged_pos / m_slider_width );
				}

				if ( data->m_first_click && m_is_inside )
				{
					m_dragging = true;
					data->m_ignore = true;
				}

				m_slider_pos = ( float( *m_value - m_min ) / float( m_max - m_min ) * m_slider_width );
				m_slider_pos = std::clamp<float>( m_slider_pos, 0.f, m_slider_width * data->m_animation_progress );
			}

			draw( data );
		}
	};

	class c_button : public c_control
	{
		const char* m_text{ };
	public:
		void draw( c_window_data* data )
		{
			filled_rect( UIcolor_t( 0, 0, 0, data->m_alpha ), m_start.x - 1.f, m_start.y - 1.f, m_end.x + 1.f, m_end.y + 1.f );
			filled_rect( UIcolor_t( 36, 36, 36, data->m_alpha ), m_start.x, m_start.y, m_end.x, m_end.y );

			if ( m_text )
				string( UIcolor_t( 206, 206, 206, data->m_alpha ), m_start.x + m_bounds.x * 0.5f, m_start.y + ( m_bounds.y * 0.5f ) - 6.0f, true, m_text );
		}

		void handle( c_window_data* data, const char* text, std::function< void( ) > function )
		{
			g_input.get_cursor_pos( m_cursor_x, m_cursor_y );

			m_text = text;
			m_start = vec2( data->m_x + 20.f, data->m_y );
			m_bounds = vec2( 150.f, 20.f );
			m_end = m_start + m_bounds;

			data->m_y += 25.f;

			m_is_inside = m_cursor_x > m_start.x && m_cursor_x < m_end.x && m_cursor_y > m_start.y && m_cursor_y < m_end.y;

			if ( data->m_left_click && !data->m_ignore && m_is_inside && function )
			{
				function( );
				data->m_ignore = true;
			}

			draw( data );
		}
	};

	class c_key_bind : public c_control
	{
		const char* m_text{ };
		int* m_key{ };

		vec2 m_box_start;
		bool m_is_waiting{ };
	public:
		void handle( c_window_data* data, const char* text, int* key )
		{
			g_input.get_cursor_pos( m_cursor_x, m_cursor_y );

			m_text = text;
			m_key = key;

			m_start = vec2( data->m_x + 20.f , data->m_y );
			m_box_start = vec2( m_start.x, m_start.y + 12.f );

			data->m_y += 40.f;

			m_bounds = vec2( 150, 20 );

			m_end = m_box_start + m_bounds;

			m_is_inside = m_cursor_x > m_box_start.x && m_cursor_x < m_end.x && m_cursor_y > m_box_start.y && m_cursor_y < m_end.y;

			if ( data->m_left_click && !data->m_ignore && m_is_inside )
			{
				m_is_waiting = true;
				data->m_ignore = true;
			}

			if ( m_is_waiting )
			{
				*m_key = KEYS_NONE;

				VirtualKeys_t start_key = data->m_ignore ? KEYS_MOUSE1 : KEYS_NONE;

				if ( g_input.is_any_key_pressed( ) == KEYS_ESCAPE )
				{
					m_is_waiting = false;
					*m_key = KEYS_NONE;
				}
				else if ( g_input.is_any_key_pressed( ) > start_key )
				{
					*m_key = g_input.is_any_key_pressed( );
					m_is_waiting = false;
					data->m_ignore = true;
				}
			}

			draw( data );
		}

		void draw( c_window_data* data )
		{
			UIcolor_t text_color( 206, 206, 206, data->m_alpha );
			UIcolor_t text_shadow( 0, 0, 0, data->m_alpha );

			if ( m_text )
			{
				UI::string( text_shadow, m_start.x + 1.f, m_start.y + 1.f - 3.f, false, m_text ); //supwemacy dont have no shadow
				UI::string( text_color, m_start.x, m_start.y - 3.f, false, m_text );
			}

			filled_rect( UIcolor_t( 0, 0, 0, data->m_alpha ), m_box_start.x - 1.f, m_box_start.y - 1.f, m_end.x + 1.f, m_end.y + 1.f );
			filled_rect( UIcolor_t( 36, 36, 36, data->m_alpha ), m_box_start.x, m_box_start.y, m_end.x, m_end.y );

			//supwemacy does m_box_start.y + 4.f, but it looks off (cuz it is off)
			if ( m_is_waiting )
				UI::string( data->m_color, m_box_start.x + 10.0f, m_box_start.y + 3.f, false, xors( "press key" ) );
			else if ( *m_key )
				UI::string( UIcolor_t( 153, 153, 153, data->m_alpha ), m_box_start.x + 10.f, m_box_start.y + 3.f, false, g_input.get_key_name( *reinterpret_cast< VirtualKeys_t* >( m_key ) ) );
		}
	};

	class c_dropdown : public c_control
	{
		const char* m_text{ };
		std::vector<std::string> m_items{ };

		int* m_setting{ };
		bool m_is_open{ };

		vec2 m_box_start;
		vec2 m_box_end;
		bool m_is_inside_items{ };
		float m_dropdown_end{ };

		const float m_item_height = 16.f;
	public:

		void handle ( c_window_data* data, const char* text, const std::vector<std::string>& items, int* setting )
		{
			g_input.get_cursor_pos( m_cursor_x, m_cursor_y );

			m_text = strlen( text ) > 1 ? text : nullptr;
			m_items = items;
			m_setting = setting;

			m_start = vec2( data->m_x + 20.f, data->m_y );
			m_box_start = vec2( m_start.x, m_start.y );

			data->m_y += 25.f;

			if ( m_text )
			{
				m_box_start.y += 15.f;
				data->m_y += 15.f;
			}

			m_bounds = vec2( 150, 20 );

			m_end = m_box_start + m_bounds;

			m_box_end = m_end;

			if ( m_is_open )
			{
				float h = items.size( ) * m_item_height;
				m_box_end.y += h + 3.f;
			}

			m_is_inside = m_cursor_x > m_box_start.x && m_cursor_x < m_end.x && m_cursor_y > m_box_start.y && m_cursor_y < m_end.y;
			m_is_inside_items = m_cursor_x > m_box_start.x && m_cursor_x < m_box_end.x && m_cursor_y > m_box_start.y && m_cursor_y < m_box_end.y;

			if ( data->m_left_click && !data->m_ignore )
			{
				if ( m_is_inside )
				{
					m_is_open = !m_is_open;
					data->m_ignore = true;
				}
				else if ( m_is_open && m_is_inside_items )
				{
					float offset = m_cursor_y - m_end.y;
					*m_setting = (int)( offset / m_item_height );
					data->m_ignore = true;

					m_is_open = false;
				}
				else
				{
					m_is_open = false;
				}
			}

			//clamp setting to avoid cwashing
			*m_setting = std::clamp<int>( *m_setting, 0, std::max<int>( 0, items.size( ) - 1 ) );

			draw( data );
		}

		void draw( c_window_data* data )
		{
			UIcolor_t text_color( 206, 206, 206, data->m_alpha );
			UIcolor_t text_shadow( 0, 0, 0, data->m_alpha );

			if ( m_text )
			{
				UI::string( text_shadow, m_start.x + 1.f, m_start.y + 1.f - 3.f, false, m_text ); //supwemacy dont have no shadow
				UI::string( text_color, m_start.x, m_start.y - 3.f, false, m_text );
			}

			// outer edge
			UI::filled_rect( UIcolor_t( 0, 0, 0, data->m_alpha ), m_box_start.x - 1.f, m_box_start.y - 1.f, m_end.x + 1.f, m_end.y + 1.f );

			// inner background
			UI::filled_rect( UIcolor_t( 36, 36, 36, data->m_alpha ), m_box_start.x, m_box_start.y, m_end.x, m_end.y );

			UI::string( UIcolor_t( 153, 153, 153, data->m_alpha ), m_box_start.x + 10.f, m_box_start.y + 3.f, false, m_items.at( *m_setting ).c_str( ) );

			// bester arrow
			const UIcolor_t arrow_color( 153, 153, 153, data->m_alpha );

			float x = m_end.x - 11.f;
			float y = m_box_start.y + 9.f;

			if ( m_is_open )
			{
				UI::line( arrow_color, x + 2.f, y, x + 3.f, y );
				UI::line( arrow_color, x + 1.f, y + 1.f, x + 4.f, y + 1.f );
				UI::line( arrow_color, x , y + 2.f, x + 5.f, y + 2.f );
			}
			else
			{
				UI::line( arrow_color, x + 2.f, y + 2.f, x + 3.f, y + 2.f );
				UI::line( arrow_color, x + 1.f, y + 1.f, x + 4.f, y + 1.f );
				UI::line( arrow_color, x, y, x + 5.f, y );
			}
		}

		void dropdown( c_window_data* data )
		{
			// adjust end of dropdown

			// sorry for ugly code
			// as soon as m_is_open is false, m_box_end is set to bounding of button, so cant be used to just clamp it in 1 line, it would break close animation
			// maybe this is reason supremacy also doesnt have close animation :>
			if ( m_is_open )
			{
				m_dropdown_end += 2.f;

				if ( m_dropdown_end + m_end.y > m_box_end.y )
					m_dropdown_end = m_box_end.y - m_end.y;
			}
			else
			{
				m_dropdown_end -= 2.f;

				if ( m_dropdown_end <= 0.f )
					m_dropdown_end = 0.f;
			}

			// shop is closed!
			if ( m_dropdown_end <= 0.f )
				return;

			UI::filled_rect( UIcolor_t( 0, 0, 0 ), m_box_start.x - 1.f, m_end.y + 2.f, m_box_end.x + 1.f, m_end.y + 2.f + m_dropdown_end );
			UI::filled_rect( UIcolor_t( 36, 36, 36 ), m_box_start.x, m_end.y + 2.f + 1.f, m_box_end.x, m_end.y + 2.f + m_dropdown_end - 1.f );

			for ( size_t i = 0; i < m_items.size( ); i++ )
			{
				float x = m_box_start.x + 10.f;
				float y = m_end.y + 3.f + i * m_item_height;

				if ( y + ( m_item_height * 0.5f ) > m_end.y + m_dropdown_end )
					continue;

				UI::string( *m_setting == i ? data->m_color : UIcolor_t( 153, 153, 153 ), x, y, false, m_items.at( i ).c_str( ) );
			}
		}
	};

	struct dropdown_item_t
	{
		bool* m_setting;
		std::string m_text;
	};

	class c_multi_dropdown : public c_control
	{
		const char* m_text{ };
		std::vector<dropdown_item_t> m_items{ };

		bool m_is_open{ };

		vec2 m_box_start;
		vec2 m_box_end;
		bool m_is_inside_items{ };
		float m_dropdown_end{ };

		const float m_item_height = 16.f;
	public:

		void handle( c_window_data* data, const char* text, const std::vector<dropdown_item_t>& items )
		{
			g_input.get_cursor_pos( m_cursor_x, m_cursor_y );

			m_text = strlen( text ) > 1 ? text : nullptr;
			m_items = items;

			m_start = vec2( data->m_x + 20.f, data->m_y );
			m_box_start = vec2( m_start.x, m_start.y );

			data->m_y += 25.f;

			if ( m_text )
			{
				m_box_start.y += 15.f;
				data->m_y += 15.f;
			}

			m_bounds = vec2( 150, 20 );

			m_end = m_box_start + m_bounds;

			m_box_end = m_end;

			if ( m_is_open )
			{
				float h = items.size( ) * m_item_height;
				m_box_end.y += h + 3.f;
			}

			m_is_inside = m_cursor_x > m_box_start.x && m_cursor_x < m_end.x && m_cursor_y > m_box_start.y && m_cursor_y < m_end.y;
			m_is_inside_items = m_cursor_x > m_box_start.x && m_cursor_x < m_box_end.x && m_cursor_y > m_box_start.y && m_cursor_y < m_box_end.y;

			if ( data->m_left_click && !data->m_ignore )
			{
				if ( m_is_inside )
				{
					m_is_open = !m_is_open;
					data->m_ignore = true;
				}
				else if ( m_is_open && m_is_inside_items )
				{
					float offset = m_cursor_y - m_end.y;
					int clicked_item = std::clamp<int>( (int)( offset / m_item_height ), 0, std::max<int>( 0, m_items.size( ) - 1 ) );
					*m_items.at( clicked_item ).m_setting = !*m_items.at( clicked_item ).m_setting;
					data->m_ignore = true;
				}
				else
				{
					m_is_open = false;
				}
			}

			draw( data );
		}

		void draw( c_window_data* data )
		{
			UIcolor_t text_color( 206, 206, 206, data->m_alpha );
			UIcolor_t text_shadow( 0, 0, 0, data->m_alpha );

			if ( m_text )
			{
				UI::string( text_shadow, m_start.x + 1.f, m_start.y + 1.f - 3.f, false, m_text ); //supwemacy dont have no shadow
				UI::string( text_color, m_start.x, m_start.y - 3.f, false, m_text );
			}

			UI::filled_rect( UIcolor_t( 0, 0, 0, data->m_alpha ), m_box_start.x - 1.f, m_box_start.y - 1.f, m_end.x + 1.f, m_end.y + 1.f );
			UI::filled_rect( UIcolor_t( 36, 36, 36, data->m_alpha ), m_box_start.x, m_box_start.y, m_end.x, m_end.y );

			std::string items_text;
			for ( size_t i = 0; i < m_items.size( ); i++ )
			{
				if ( !*m_items.at( i ).m_setting )
					continue;

				const float width_limit = 120.f;

				float new_width = UI::text_width( std::string( items_text + m_items.at( i ).m_text ).c_str( ) );

				if ( new_width > width_limit )
				{
					items_text += xors( ", ..." );
					break;
				}

				if ( !items_text.empty( ) )
					items_text += ", ";

				items_text += m_items.at( i ).m_text;
			}
			UI::string( UIcolor_t( 153, 153, 153, data->m_alpha ), m_box_start.x + 10.f, m_box_start.y + 3.f, false, items_text.c_str( ) );

			// bester arrow
			const UIcolor_t arrow_color( 153, 153, 153, data->m_alpha );

			float x = m_end.x - 11.f;
			float y = m_box_start.y + 9.f;

			if ( m_is_open )
			{
				UI::line( arrow_color, x + 2.f, y, x + 3.f, y );
				UI::line( arrow_color, x + 1.f, y + 1.f, x + 4.f, y + 1.f );
				UI::line( arrow_color, x, y + 2.f, x + 5.f, y + 2.f );
			}
			else
			{
				UI::line( arrow_color, x + 2.f, y + 2.f, x + 3.f, y + 2.f );
				UI::line( arrow_color, x + 1.f, y + 1.f, x + 4.f, y + 1.f );
				UI::line( arrow_color, x, y, x + 5.f, y );
			}
		}

		void dropdown( c_window_data* data )
		{
			// adjust end of dropdown

			// sorry for ugly code
			// as soon as m_is_open is false, m_box_end is set to bounding of button, so cant be used to just clamp it in 1 line, it would break close animation
			// maybe this is reason supremacy also doesnt have close animation :>
			if ( m_is_open )
			{
				m_dropdown_end += 2.f;

				if ( m_dropdown_end + m_end.y > m_box_end.y )
					m_dropdown_end = m_box_end.y - m_end.y;
			}
			else
			{
				m_dropdown_end -= 2.f;

				if ( m_dropdown_end <= 0.f )
					m_dropdown_end = 0.f;
			}

			// shop is closed!
			if ( m_dropdown_end <= 0.f )
				return;

			UI::filled_rect( UIcolor_t( 0, 0, 0 ), m_box_start.x - 1.f, m_end.y + 2.f, m_box_end.x + 1.f, m_end.y + 2.f + m_dropdown_end );
			UI::filled_rect( UIcolor_t( 36, 36, 36 ), m_box_start.x, m_end.y + 2.f + 1.f, m_box_end.x, m_end.y + 2.f + m_dropdown_end - 1.f );

			for ( size_t i = 0; i < m_items.size( ); i++ )
			{
				float x = m_box_start.x + 10.f;
				float y = m_end.y + 3.f + i * m_item_height;

				if ( y + ( m_item_height * 0.5f ) > m_end.y + 2.f + m_dropdown_end )
					continue;

				UI::string( *m_items.at(i).m_setting ? data->m_color : UIcolor_t( 153, 153, 153 ), x, y, false, m_items.at( i ).m_text.c_str( ) );
			}
		}
	};

	class c_color_picker : public c_control
	{
		uint8_t temp_r{ };
		uint8_t temp_g{ };
		uint8_t temp_b{ };

		const char* m_text{ };

		bool m_is_open{ };

		vec2 m_box_start{ };
		vec2 m_box_end{ };
		vec2 m_box_bounds{ 20, 8 };

		vec2 m_picker_start{ };
		vec2 m_picker_end{ };
		vec2 m_picker_bounds{ 256, 256 };

		int m_alpha_value{ };
		c_slider m_alpha_slider{ };
	public:
		void draw( c_window_data* data )
		{
			UIcolor_t text_color( 206, 206, 206, data->m_alpha );
			UIcolor_t text_shadow( 0, 0, 0, data->m_alpha );

			if ( m_text )
			{
				UI::string( text_shadow, m_start.x + 20.f + 1.f, m_start.y + 1.f - 3.f, false, m_text );
				UI::string( text_color, m_start.x + 20.f, m_start.y - 3.f, false, m_text );
			}

			// outer edge
			UI::filled_rect( UIcolor_t( 0, 0, 0, data->m_alpha ), m_box_start.x - 1.f, m_box_start.y - 1.f, m_box_end.x + 1.f, m_box_end.y + 1.f );

			// inner box
			UIcolor_t box_color_top ( temp_r, temp_g, temp_b, data->m_alpha );
			UIcolor_t box_color_bottom
			(
				box_color_top.R > 25 ? box_color_top.R - 25 : 0,
				box_color_top.G > 25 ? box_color_top.G - 25 : 0,
				box_color_top.B > 25 ? box_color_top.B - 25 : 0,
				data->m_alpha
			);

			UI::rect_gradient( box_color_top, box_color_bottom, m_box_start.x, m_box_start.y, m_box_bounds.x, m_box_bounds.y );
		}

		void handle( c_window_data* data, const char* text, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a = nullptr )
		{
			g_input.get_cursor_pos( m_cursor_x, m_cursor_y );

			m_text = text;
			m_start = vec2( data->m_x, data->m_y );

			m_box_start = vec2( data->m_x + 150.f, data->m_y );
			m_box_end = m_box_start + m_box_bounds;

			m_picker_start = vec2( m_box_start.x, m_box_start.y + 10 );
			m_picker_end = m_picker_start + m_picker_bounds;

			temp_r = *r;
			temp_g = *g;
			temp_b = *b;
			// setup coord for next item
			data->m_y += 15.f;

			if ( a )
			{
				m_alpha_value = (int)( (float)( *a ) / 2.55f );

				m_alpha_slider.handle( data, "", &m_alpha_value, 0, 100, L"%" );

				m_alpha_value = std::clamp<int>( m_alpha_value, 0, 100 );

				*a = (uint8_t)( (float)( m_alpha_value ) * 2.55f );
			}

			m_is_inside = m_cursor_x > m_box_start.x && m_cursor_x < m_box_end.x && m_cursor_y > m_box_start.y && m_cursor_y < m_box_end.y;

			if ( m_is_inside && data->m_first_click && !data->m_ignore )
			{
				m_is_open = true;
				data->m_ignore = true;
			}

			if ( m_is_open )
			{
				if ( m_cursor_x > m_picker_start.x && m_cursor_x < m_picker_end.x && m_cursor_y > m_picker_start.y && m_cursor_y < m_picker_end.y )
				{
					//set temp color
					float height = m_picker_bounds.y - 2;
					float half_height = height * 0.5f;

					float x_offset = m_cursor_x - ( m_picker_start.x + 1 );
					float y_offset = m_cursor_y - ( m_picker_start.y + 1 );

					float hue = x_offset / height;

					UIcolor_t color;

					if ( y_offset > half_height )
					{
						float brightness = 1.f - std::clamp<float>( ( y_offset - half_height ) / half_height, 0.f, 1.f );
						color = UIcolor_t::hsb_to_rgb( hue, 1.f, brightness );
					}
					else
					{
						float saturation = std::clamp<float>( y_offset / half_height, 0.f, 1.f );
						color = UIcolor_t::hsb_to_rgb( hue, saturation, 1.f );
					}

					temp_r = color.R;
					temp_g = color.G;
					temp_b = color.B;
				}

				if ( !data->m_left_click )
				{
					*r = temp_r;
					*g = temp_g;
					*b = temp_b;
					m_is_open = false;
				}
			}

			draw( data );
		}

		void picker( c_window_data* data )
		{
			if ( !m_is_open )
				return;

			// outer edge
			UI::filled_rect( UIcolor_t( 0, 0, 0, data->m_alpha ), m_picker_start.x, m_picker_start.y, m_picker_end.x, m_picker_end.y );

			// picker
			int height = m_picker_bounds.y - 2;
			int half_height = height / 2;

			for ( int i = 0; i < height; i++ )
			{
				float hue = float( i ) / float( height );

				UIcolor_t top = UIcolor_t::hsb_to_rgb( hue, 0.f, 1.f );
				UIcolor_t middle = UIcolor_t::hsb_to_rgb( hue, 1.f, 1.f );
				UIcolor_t bottom = UIcolor_t::hsb_to_rgb( hue, 1.f, 0.f );

				top.A = data->m_alpha;
				middle.A = data->m_alpha;
				bottom.A = data->m_alpha;

				UI::rect_gradient( top, middle, m_picker_start.x + 1 + i, m_picker_start.y + 1, 1, half_height );
				UI::rect_gradient( middle, bottom, m_picker_start.x + 1 + i, m_picker_start.y + 1 + half_height, 1, half_height );
			}
		}
	};

	//class c_text_input
}
