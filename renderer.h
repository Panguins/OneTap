#pragma once
#include <d3dx9.h>
#include <stdint.h>
#include <memory>

#include "sdk.h"

enum gradient_t
{
	horizontal,
	vertical,
};

enum font_t
{
	font_menu,
	font_esp,
	font_esp_small,
	font_bbc
};

class c_render
{
protected:
	struct vertex_t
	{
		vertex_t( float x, float y, float z, color_t color )
			: m_x( x ), m_y( y ), m_z( z ), m_color( D3DCOLOR_RGBA( color.r( ), color.g( ), color.b( ), color.a( ) ) )
		{
		}

		float m_x;
		float m_y;
		float m_z;
		float m_rhw = 0;
		D3DCOLOR m_color;
	};

public:
	c_render( IDirect3DDevice9* );

	void create( );
	void release( );

	void start( );
	void finish( );
	void draw( ); // big boss shit

	void rect( color_t color, float x, float y, float w, float h );
	void filled_rect( color_t color, float x, float y, float w, float h );
	void _filled_rect( color_t color, float x, float y, float w, float h );
	void rect_gradient( gradient_t, color_t start, color_t end, float x, float y, float w, float h );
	void line( color_t color, float x1, float y1, float x2, float y2 );
	float text_width( font_t, const char* fmt, ... );
	void string( font_t, color_t color, float x, float y, bool centered, const char* fmt, ... );
	void _string( font_t, color_t color, float x, float y, bool centered, const char* fmt, ... );
	void wide_string( font_t, color_t color, float x, float y, bool centered, const wchar_t* fmt, ... );
	void image( uint32_t x, uint32_t y, uint32_t w, uint32_t h, LPDIRECT3DTEXTURE9 texture, LPD3DXSPRITE sprite, uint8_t* image, const uint32_t size, bool setup );

	inline void draw_primitive( vertex_t* vert, const D3DPRIMITIVETYPE& type )
	{
		m_verts.push_back( vert );
		m_types.push_back( type );
	}

private:
	IDirect3DDevice9 * m_device;
	IDirect3DStateBlock9*   m_state_block;
	ID3DXSprite*            m_sprite;
	IDirect3DVertexBuffer9* m_buffer;
	IDirect3DIndexBuffer9*  m_indexed_buffer;
	D3DVIEWPORT9*           m_view_port;
	DWORD                   m_old_fvf;
	std::vector< vertex_t* > m_verts;
	std::vector< D3DPRIMITIVETYPE > m_types; // 2d < 1d 

	inline void add_vert( vertex_t* vert )
	{
		m_verts.push_back( vert );
	}

	ID3DXFont*              m_menu;
	ID3DXFont*              m_esp;
	ID3DXFont*              m_esp_small;
	ID3DXFont*              m_bbc;
};

// unused:
class render_list_t : protected c_render
{
public:
	render_list_t( size_t max_verts );

	void empty( );
	void batch( );

	inline void add_vert( const vertex_t& vert )
	{
		m_verts.push_back( vert );
	}

	std::vector< vertex_t > m_verts;
};
