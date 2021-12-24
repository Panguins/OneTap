#pragma once

class c_esp
{
	struct esp_box_t
	{
		int32_t minx, maxx, miny, maxy;

		esp_box_t( )
			: minx( std::numeric_limits< int32_t >::max( ) ), maxx( std::numeric_limits< int32_t >::min( ) ), miny( std::numeric_limits< int32_t >::max( ) ), maxy( std::numeric_limits< int32_t >::min( ) ) { }

		bool create( c_base_entity* ent )
		{
			auto tran_frame = ent->rgfl_coordinate_frame( );

			auto min = ent->mins( ), max = ent->maxs( );

			vec3 points[] =
			{
				vec3( min.x, min.y, min.z ),
				vec3( min.x, max.y, min.z ),
				vec3( max.x, max.y, min.z ),
				vec3( max.x, min.y, min.z ),
				vec3( max.x, max.y, max.z ),
				vec3( min.x, max.y, max.z ),
				vec3( min.x, min.y, max.z ),
				vec3( max.x, min.y, max.z )
			};

			vec3 temp_point;
			for ( auto i = 0; i < 8; ++i )
			{
				if ( !drawing::w2s( math::vector_transform( points[i], tran_frame ), temp_point ) )
					return false;

				minx = std::min( minx, static_cast< int32_t >( temp_point.x ) );
				maxx = std::max( maxx, static_cast< int32_t >( temp_point.x ) );
				miny = std::min( miny, static_cast< int32_t >( temp_point.y ) );
				maxy = std::max( maxy, static_cast< int32_t >( temp_point.y ) );
			}

			return true;
		}
	};

	struct esp_player_t
	{
		int index;
		c_base_entity* player;
		float dist;

		void draw( );
		void draw_skeleton( color_t );
		void draw_health( vec2, vec2, int, uint8_t, color_t );
		void draw_ammo( vec2, vec2, color_t, color_t );
		void draw_offscreen_arrows( const bool& enemy );
	};

public:
	void init( );
	void lists_init( );
private:
	void draw_non_players( );
	void draw_spectator_list( );
public:

	static __forceinline void draw_ent_box( color_t color, esp_box_t& box, bool outlined = true, color_t outline_color = color_t( 0, 0, 0, 175 ) )
	{
		if ( outlined )
		{
			ctx.m_renderer->rect( outline_color, box.minx + 1.f, box.miny + 1.f, box.maxx - 1.f, box.maxy - 1.f );
			ctx.m_renderer->rect( outline_color, box.minx - 1.f, box.miny - 1.f, box.maxx + 1.f, box.maxy + 1.f );
		}

		ctx.m_renderer->rect( color, box.minx, box.miny, box.maxx, box.maxy );
	}
};

extern c_esp g_esp;
