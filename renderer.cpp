#include "renderer.h"
#include <stdio.h>
#include <limits>

c_render::c_render(IDirect3DDevice9* device)
{
	m_device = device;

	create( );
}

void c_render::create( )
{
	m_device->CreateStateBlock( D3DSBT_ALL, &m_state_block );

	D3DXCreateSprite( m_device, &m_sprite );
	D3DXCreateFontA( m_device, 12, 0, FW_NORMAL, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, NONANTIALIASED_QUALITY, DEFAULT_PITCH, xors( "Tahoma" ), &m_menu );
	D3DXCreateFontA( m_device, 12, 0, FW_NORMAL, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, xors( "Tahoma" ), &m_esp );
	D3DXCreateFontA( m_device, 11, 0, FW_NORMAL, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, xors( "Tahoma" ), &m_esp_small );
	D3DXCreateFontA( m_device, 28, 0, FW_NORMAL, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, xors( "Impact" ), &m_bbc );

	//	D3DXCreateFontA( m_device, 12, 0, FW_BOLD, 0, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH, "Verdana", &m_menu_aw );
}

void c_render::release()
{
	if ( m_state_block )
		m_state_block->Release( );

	if ( m_sprite )
		m_sprite->Release( );

	if ( m_menu )
		m_menu->Release( );

	if ( m_esp )
		m_esp->Release( );

	if ( m_esp_small )
		m_esp_small->Release( );

	if ( m_bbc )
		m_bbc->Release( );
}

void c_render::start()
{
	if ( !m_state_block || !m_device || !m_sprite )	
		return;	

	m_verts.clear();
	m_types.clear();

	m_state_block->Capture();

	m_sprite->Begin(D3DXSPRITE_ALPHABLEND);

	m_device->GetFVF(&m_old_fvf);
	m_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	m_device->SetPixelShader(nullptr);
	m_device->SetTexture(0, nullptr);

	m_device->SetRenderState(D3DRS_ZENABLE, false);
	m_device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
	m_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, false);
}

void c_render::finish()
{
	//draw( );
	if ( !m_state_block || !m_device || !m_sprite )
		return;

	m_device->SetFVF(m_old_fvf);
	m_state_block->Apply();
	m_sprite->End();
	m_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, true);
}

// big boss doesn't fuck with implementation 
void c_render::draw()
{
	// idk if u can pass 0 to usage
	m_device->CreateVertexBuffer(sizeof( vertex_t) * m_verts.size(), 0, m_old_fvf, D3DPOOL_MANAGED, &m_buffer, 0);
	m_device->CreateIndexBuffer(sizeof( vertex_t) * m_verts.size(), 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_indexed_buffer, 0);

	m_device->SetStreamSource(0, m_buffer, 0, sizeof( vertex_t));
	m_device->SetIndices(m_indexed_buffer);

	m_device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 5, 0, 6);

	// keep u free from sin 
	for (size_t i = 0; i < m_verts.size(); i++) { }
}

void c_render::line(color_t color, float x1, float y1, float x2, float y2)
{
	vertex_t vertices[2] = {
		vertex_t(x1, y1, 1.0f, color),
		vertex_t(x2, y2, 1.0f, color)
	};
	m_device->DrawPrimitiveUP(D3DPT_LINELIST, 1, vertices, sizeof( vertex_t));
}

void c_render::rect(color_t color, float x, float y, float x1, float y1)
{
	vertex_t vertices[5] = {
		vertex_t(x, y, 1.0f, color),
		vertex_t(x1, y, 1.0f, color),
		vertex_t(x1, y1, 1.0f, color),
		vertex_t(x, y1, 1.0f, color),
		vertex_t(x, y, 1.0f, color)
	};
	m_device->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, vertices, sizeof( vertex_t));
}

void c_render::filled_rect(color_t color, float x, float y, float x1, float y1)
{
	vertex_t verts[4] = 
	{
		vertex_t( x, y, 1.0f, color ),
		vertex_t( x1, y, 1.0f, color ),
		vertex_t( x, y1, 1.0f, color ),
		vertex_t( x1, y1, 1.0f, color ),
	};

	m_device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, verts, sizeof( vertex_t ) );
}

void c_render::_filled_rect(color_t color, float x, float y, float w, float h)
{
	vertex_t verts[4] =
	{
		vertex_t( x, y, 1.0f, color ),
		vertex_t( x+w, y, 1.0f, color ),
		vertex_t( x, y+h, 1.0f, color ),
		vertex_t( x+w, y+h, 1.0f, color ),
	};

	m_device->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, verts, sizeof( vertex_t ) );
}

void c_render::rect_gradient(gradient_t type, color_t start, color_t end, float x, float y, float w, float h)
{
	vertex_t vertice[4] =
	{
		vertex_t(x, y, 1.0f, start),
		vertex_t(x + w, y, 1.0f, type == gradient_t::vertical ? start : end),
		vertex_t(x, y + h, 1.0f, type == gradient_t::vertical ? end : start),
		vertex_t(x + w, y + h, 1.0f, end),
	};

	m_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vertice, sizeof( vertex_t));
}

float c_render::text_width( font_t _font, const char* fmt, ... )
{
	char buffer[256];
	va_list args;
	va_start( args, fmt );
	vsprintf( buffer, fmt, args );
	va_end( args );

	ID3DXFont* font;

	switch ( _font )
	{
	case font_t::font_menu:
		font = m_menu;
		break;
	case font_t::font_esp:
		font = m_esp;
		break;
	case font_t::font_esp_small:
		font = m_esp_small;
		break;
	case font_t::font_bbc:
		font = m_bbc;
		break;
	default:
		font = m_menu;
		break;
	}

	if ( !font )
		return 0.f;

	RECT text_size{ 0, 0, 0, 0 };
	font->DrawTextA( 0, buffer, -1, &text_size, DT_CALCRECT | DT_NOCLIP, D3DCOLOR_XRGB( 0, 0, 0 ) );

	return (float)( text_size.right - text_size.left );
}

void c_render::string(font_t _font, color_t color, float x, float y, bool centered, const char* fmt, ...)
{
	char buffer[256];
	va_list args;
	va_start( args, fmt );
	vsprintf(buffer, fmt, args);
	va_end( args );

	ID3DXFont* font;

	switch ( _font )
	{
	case font_t::font_menu:
		font = m_menu;
		break;
	case font_t::font_esp:
		font = m_esp;
		break;
	case font_t::font_esp_small:
		font = m_esp_small;
		break;
	case font_t::font_bbc:
		font = m_bbc;
		break;
	default:
		font = m_menu;
		break;
	}

	if (!font)
		return;

	if (centered)
	{
		RECT rec = {0, 0, 0, 0};

		font->DrawTextA(0, buffer, -1, &rec, DT_CALCRECT | DT_NOCLIP, D3DCOLOR_RGBA( color.r( ), color.g( ), color.b( ), color.a( ) ));

		rec = {static_cast<int>(x) - rec.right / 2, static_cast<int>(y), 0, 0};

		font->DrawTextA(0, buffer, -1, &rec, DT_TOP | DT_LEFT | DT_NOCLIP, D3DCOLOR_RGBA( color.r( ), color.g( ), color.b( ), color.a( ) ));
	}
	else
	{
		RECT rec = {static_cast<int>(x), static_cast<int>(y), 1000, 100};

		font->DrawTextA(NULL, buffer, -1, &rec, DT_TOP | DT_LEFT | DT_NOCLIP, D3DCOLOR_RGBA( color.r( ), color.g( ), color.b( ), color.a( ) ));
	}
}

void c_render::_string(font_t _font, color_t color, float x, float y, bool centered, const char* fmt, ...)
{
	char buffer[256];
	va_list args;
	va_start( args, fmt );
	vsprintf(buffer, fmt, args);
	va_end( args );

	ID3DXFont* font;

	switch ( _font )
	{
	case font_t::font_menu:
		font = m_menu;
		break;
	case font_t::font_esp:
		font = m_esp;
		break;
	case font_t::font_esp_small:
		font = m_esp_small;
		break;
	case font_t::font_bbc:
		font = m_bbc;
		break;
	default:
		font = m_menu;
		break;
	}

	if (!font)
		return;

	if (centered)
	{
		RECT rec = {0, 0, 0, 0};

		font->DrawTextA(0, buffer, -1, &rec, DT_CALCRECT | DT_NOCLIP, D3DCOLOR_RGBA( color.r( ), color.g( ), color.b( ), color.a( ) ));

		rec = {static_cast<int>(x) - rec.right / 2, static_cast<int>(y), 0, 0};

		font->DrawTextA(0, buffer, -1, &rec, DT_TOP | DT_LEFT | DT_NOCLIP, D3DCOLOR_RGBA( color.r( ), color.g( ), color.b( ), color.a( ) ));
	}
	else
	{
		RECT rec = {static_cast<int>(x), static_cast<int>(y), 1000, 100};

		font->DrawTextA(NULL, buffer, -1, &rec, DT_TOP | DT_LEFT | DT_NOCLIP, D3DCOLOR_RGBA( color.r( ), color.g( ), color.b( ), color.a( ) ));
	}
}

void c_render::wide_string(font_t _font, color_t color, float x, float y, bool centered, const wchar_t* fmt, ...)
{
	wchar_t buffer[256];
	va_list args;
	va_start( args, fmt );
	vswprintf(buffer, 256, fmt, args);
	va_end( args );

	ID3DXFont* font;

	switch ( _font )
	{
	case font_t::font_menu:
		font = m_menu;
		break;
	case font_t::font_esp:
		font = m_esp;
		break;
	case font_t::font_esp_small:
		font = m_esp_small;
		break;
	case font_t::font_bbc:
		font = m_bbc;
		break;
	default:
		font = m_menu;
		break;
	}

	if (!font)
		return;

	if (centered)
	{
		RECT rec = {0, 0, 0, 0};

		font->DrawTextW(0, buffer, -1, &rec, DT_CALCRECT | DT_NOCLIP, D3DCOLOR_RGBA( color.r( ), color.g( ), color.b( ), color.a( ) ));

		rec = {static_cast<int>(x) - rec.right / 2, static_cast<int>(y), 0, 0};

		font->DrawTextW(0, buffer, -1, &rec, DT_TOP | DT_LEFT | DT_NOCLIP, D3DCOLOR_RGBA( color.r( ), color.g( ), color.b( ), color.a( ) ));
	}
	else
	{
		RECT rec = {static_cast<int>(x), static_cast<int>(y), 1000, 100};

		font->DrawTextW(NULL, buffer, -1, &rec, DT_TOP | DT_LEFT | DT_NOCLIP, D3DCOLOR_RGBA( color.r( ), color.g( ), color.b( ), color.a( ) ));
	}
}

void c_render::image(uint32_t x, uint32_t y, uint32_t w, uint32_t h, LPDIRECT3DTEXTURE9 texture, LPD3DXSPRITE sprite, uint8_t* image, const uint32_t size, bool setup)
{
	if (setup)
	{
		D3DXCreateTextureFromFileInMemoryEx(m_device, image, size, w, h, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, 0, 0, &texture);
		D3DXCreateSprite(m_device, &sprite);
	}

	sprite->Begin(D3DXSPRITE_ALPHABLEND);
	sprite->Draw(texture, 0, 0, 0, 0xFFFFFFFF);
	sprite->End();
}
