#include "context.h"
#include "esp.h"
#include "boner.h"

c_esp g_esp;

void c_esp::esp_player_t::draw( )
{
	esp_box_t box;
	color_t box_color, ammo_color, skeleton_color, outline_color;
	uint8_t alpha, alpha_flags;
	bool enemy;
	int elements = 0;

	enemy = ctx.m_local->team( ) != player->team( );

	if ( !enemy && ctx.m_settings.players_esp == 1 )
		return;

	if ( ctx.m_settings.players_radar && enemy )
		player->spotted( ) = true;

	if ( !box.create( player ) )
	{
		if ( enemy && !ctx.m_settings.players_offscreen_enemy )
			return;

		if ( !enemy && !ctx.m_settings.players_offscreen_friendly )
			return;

		draw_offscreen_arrows( enemy );

		return;
	}

	// setup colours
	box_color = enemy ? ctx.m_settings.players_box_enemy_color : ctx.m_settings.players_box_friendly_color;
	ammo_color = ctx.m_settings.players_ammo_color;
	skeleton_color = ctx.m_settings.players_skeleton_color;
	outline_color = color_t( 0, 0, 0, 175 );
	alpha = 255;
	alpha_flags = 200;

	// change if dormant
	if ( player->dormant( ) )
	{
		box_color = color_t( 255, 255, 255, 150 );
		ammo_color = color_t( 255, 255, 255, 150 );
		outline_color = color_t( 0, 0, 0, 150 );
		alpha = 150;
		alpha_flags = 100;
	}

	// draw box
	if ( ( enemy && ctx.m_settings.players_box_enemy ) || ( !enemy && ctx.m_settings.players_box_friendly ) )
		draw_ent_box( box_color, box, true, outline_color );

	// draw name
	if ( ctx.m_settings.players_name )
	{
		std::wstring name = player->name( );

		ctx.m_renderer->wide_string( font_t::font_esp, color_t( 0, 0, 0, alpha ), ( box.maxx + box.minx ) / 2 + 1, box.miny - 11.0f, true, name.c_str( ) );
		ctx.m_renderer->wide_string( font_t::font_esp, color_t( 255, 255, 255, alpha ), ( box.maxx + box.minx ) / 2, box.miny - 12.0f, true, name.c_str( ) );
	}

	// draw healthbar
	if ( ctx.m_settings.players_health )
		draw_health( { box.minx, box.maxy }, { box.minx, box.miny }, player->health( ), alpha, outline_color );

	// draw ballin level
	if ( ctx.m_settings.players_money )
	{
		int money = player->money( );

		float x_pos = box.maxx + 3.f;
		float y_pos = box.miny + 1.f + ( 12.f * elements++ );

		ctx.m_renderer->string( font_t::font_esp_small, color_t( 0, 0, 0, alpha_flags ), x_pos + 1, y_pos + 1, false, xors( "$%d" ), money );
		ctx.m_renderer->string( font_t::font_esp_small, color_t( 0, 255, 0, alpha_flags ), x_pos, y_pos, false, xors( "$%d" ), money );
	}

	// draw armor flag
	if ( ctx.m_settings.players_armor )
	{
		bool helmet = player->has_helmet( );
		bool kevlar = player->armor( ) > 0;

		if ( helmet || kevlar )
		{
			char armor_string[3];

			armor_string[0] = helmet ? 'H' : 'K';
			armor_string[1] = helmet ? 'K' : 0;
			armor_string[2] = 0;

			float x_pos = box.maxx + 3.f;
			float y_pos = box.miny + 1.f + ( 12.f * elements++ );

			ctx.m_renderer->string( font_t::font_esp_small, color_t( 0, 0, 0, alpha_flags ), x_pos + 1.f, y_pos + 1.f, false, armor_string );
			ctx.m_renderer->string( font_t::font_esp_small, color_t( 255, 255, 255, alpha_flags ), x_pos, y_pos, false, armor_string );
		}
	}

	// draw extra flags
	if ( ctx.m_settings.players_flashed && player->is_flashed( ) )
	{
		float x_pos = box.maxx + 3.f;
		float y_pos = box.miny + 1.f + ( 12.f * elements++ );

		ctx.m_renderer->string( font_t::font_esp_small, color_t( 0, 0, 0, alpha_flags ), x_pos + 1.f, y_pos + 1.f, false, xors( "FLASH" ) );
		ctx.m_renderer->string( font_t::font_esp_small, color_t( 255, 50, 50, alpha_flags ), x_pos, y_pos, false, xors( "FLASH" ) );
	}
	else if ( ctx.m_settings.players_reload && player->is_reloading( ) )
	{
		float x_pos = box.maxx + 3.f;
		float y_pos = box.miny + 1.f + ( 12.f * elements++ );

		ctx.m_renderer->string( font_t::font_esp_small, color_t( 0, 0, 0, alpha_flags ), x_pos + 1.f, y_pos + 1.f, false, xors( "RELOAD" ) );
		ctx.m_renderer->string( font_t::font_esp_small, color_t( 50, 200, 255, alpha_flags ), x_pos, y_pos, false, xors( "RELOAD" ) );
	}
	else if ( ctx.m_settings.players_zoom && player->in_scope( ) )
	{
		float x_pos = box.maxx + 3.f;
		float y_pos = box.miny + 1.f + ( 12.f * elements++ );

		ctx.m_renderer->string( font_t::font_esp_small, color_t( 0, 0, 0, alpha_flags ), x_pos + 1.f, y_pos + 1.f, false, xors( "ZOOM" ) );
		ctx.m_renderer->string( font_t::font_esp_small, color_t( 50, 200, 255, alpha_flags ), x_pos, y_pos, false, xors( "ZOOM" ) );
	}

	// draw weapon
	if ( ctx.m_settings.players_weapon && player->weapon( ) )
	{
		bool ammo_bar = ctx.m_settings.players_ammo && player->weapon( )->clip1( ) != -1;
		std::string name = util::to_lower( player->weapon( )->print_name( ) );

		float y_pos = ammo_bar ? box.maxy + 7.f : box.maxy + 2.f;
		ctx.m_renderer->string( font_t::font_esp_small, color_t( 0, 0, 0, alpha ), ( box.maxx + box.minx ) / 2.f + 1, y_pos + 1, true, name.c_str( ) );
		ctx.m_renderer->string( font_t::font_esp_small, color_t( 255, 255, 255, alpha ), ( box.maxx + box.minx ) / 2.f, y_pos, true, name.c_str( ) );

		if ( ammo_bar )
			draw_ammo( { box.minx, box.maxy }, { box.maxx, box.maxy }, ammo_color, outline_color );
	}

	// draw skeleton
	if ( ctx.m_settings.players_skeleton )
		draw_skeleton( skeleton_color );
}

void c_esp::esp_player_t::draw_health( vec2 bot, vec2 top, int health, uint8_t alpha, color_t outline_color )
{
	int h = std::clamp< int >( health, 0, 100 );

	float fraction = h * 0.01f;
	color_t color( std::min( 255 * ( 100 - health ) / 100, 255 ), 255.f * fraction, 0, alpha );

	float height = bot.y - top.y;
	float pos = height - ( height * fraction );

	ctx.m_renderer->line( outline_color, top.x - 6.f, top.y - 1.f, top.x - 6.f, bot.y + 1.f );
	ctx.m_renderer->line( outline_color, top.x - 5.f, top.y - 1.f, top.x - 5.f, bot.y + 1.f );
	ctx.m_renderer->line( outline_color, top.x - 4.f, top.y - 1.f, top.x - 4.f, bot.y + 1.f );
	ctx.m_renderer->line( outline_color, top.x - 3.f, top.y - 1.f, top.x - 3.f, bot.y + 1.f );

	ctx.m_renderer->line( color, top.x - 5.f, top.y + pos, top.x - 5.f, bot.y );
	ctx.m_renderer->line( color, top.x - 4.f, top.y + pos, top.x - 4.f, bot.y );

	if ( health != 100 )
	{
		ctx.m_renderer->string( font_t::font_esp_small, color_t( 0, 0, 0 ), top.x - 4.f, top.y + pos - 4.0f, true, xors( "%i" ), health );
		ctx.m_renderer->string( font_t::font_esp_small, color_t( 255, 255, 255 ), top.x - 5.f, top.y + pos - 5.0f, true, xors( "%i" ), health );
	}
}

void c_esp::esp_player_t::draw_ammo( vec2 bot_left, vec2 bot_right, color_t ammo_color, color_t outline_color )
{
	int clip = player->weapon( )->clip1( );
	int full_clip = player->weapon( )->max_clip1( );

	if ( clip == -1 )
		return;

	float fraction = player->is_reloading( ) ? player->get_reload_progress( ) : float( clip ) / float( full_clip );

	ctx.m_renderer->line( outline_color, bot_left.x - 1.f, bot_left.y + 3.f, bot_right.x + 1.f, bot_right.y + 3.f );
	ctx.m_renderer->line( outline_color, bot_left.x - 1.f, bot_left.y + 4.f, bot_right.x + 1.f, bot_right.y + 4.f );
	ctx.m_renderer->line( outline_color, bot_left.x - 1.f, bot_left.y + 5.f, bot_right.x + 1.f, bot_right.y + 5.f );
	ctx.m_renderer->line( outline_color, bot_left.x - 1.f, bot_left.y + 6.f, bot_right.x + 1.f, bot_right.y + 6.f );

	float width = ( bot_right.x - bot_left.x ) * fraction;
	ctx.m_renderer->line( ammo_color, bot_left.x, bot_left.y + 4.f, bot_left.x + width, bot_right.y + 4.f );
	ctx.m_renderer->line( ammo_color, bot_left.x, bot_left.y + 5.f, bot_left.x + width, bot_right.y + 5.f );
}

void c_esp::esp_player_t::draw_offscreen_arrows( const bool& enemy )
{
	vec3 pos;

	drawing::w2s( player->abs_origin( ), pos );
	math::find_position_rotation( pos.x, pos.y, ctx.m_screen_w, ctx.m_screen_h );

	ctx.m_renderer->line( enemy ? ctx.m_settings.players_offscreen_enemy_color : ctx.m_settings.players_offscreen_friendly_color,
	                      ctx.m_screen_w / 2, ctx.m_screen_h / 2, pos.x, pos.y );
}

void c_esp::esp_player_t::draw_skeleton( color_t color )
{
	studiohdr_t* hdr;
	mstudiobone_t* bone;

	vec3 parent_world, child_world;
	vec3 parent, child;

	hdr = csgo.m_model_info( )->GetStudioModel( player->model( ) );

	if ( !hdr )
		return;

	matrix3x4_t matrix[128];

	if ( !g_boner.get_latest( player->index( ), matrix ) )
		return;

	vec3 upper_direction = player->bone_position( 7, matrix ) - player->bone_position( 6, matrix );
	vec3 breast_bone = player->bone_position( 6, matrix ) + upper_direction / 2;

	for ( size_t i = 0; i < hdr->numbones; i++ )
	{
		bone = hdr->GetBone( i );

		if ( !bone || !( bone->flags & BONE_USED_BY_HITBOX ) || bone->parent == -1 )
			continue;

		parent_world = player->bone_position( bone->parent, matrix );
		child_world = player->bone_position( i, matrix );

		vec3 child_delta = child_world - breast_bone;
		vec3 parent_delta = parent_world - breast_bone;

		if ( parent_delta.length( ) < 9.f && child_delta.length( ) < 9.f )
			parent_world = breast_bone;

		if ( i == 5 )
			child_world = breast_bone;

		if ( abs( child_delta.z ) < 5.f && ( parent_delta.length( ) < 5.f && child_delta.length( ) < 5.f ) || i == 6 )
			continue;

		if ( drawing::w2s( parent_world, parent ) && drawing::w2s( child_world, child ) )
		{
			// force alpha to 200
			color.a( 200 );

			ctx.m_renderer->line( color, parent.x, parent.y, child.x, child.y );

			// extra thicc
			ctx.m_renderer->line( color, parent.x, parent.y + 1, child.x, child.y + 1 );
			ctx.m_renderer->line( color, parent.x + 1, parent.y + 1, child.x + 1, child.y + 1 );
			ctx.m_renderer->line( color, parent.x + 1, parent.y, child.x + 1, child.y );
		}
	}
}

void c_esp::lists_init( )
{
	if ( ctx.m_settings.visuals_spec_list )
		draw_spectator_list( );
}

void c_esp::init( )
{
	if ( !ctx.m_local || !csgo.m_engine( )->IsInGame( ) )
		return;

	if ( ctx.m_settings.visuals_active )
		draw_non_players( );

	if ( !ctx.m_settings.players_esp )
		return;

	// grab players
	std::vector< esp_player_t > players;

	for ( int i = 1; i < 65; ++i )
	{
		auto player = csgo.m_entity_list( )->get_entity( i );

		if ( !player
		     || ( player == ctx.m_local && !csgo.m_input( )->m_fCameraInThirdPerson )
		     || !player->alive( )
		     || ( player->dormant( ) && !ctx.m_settings.players_dormant ) )
			continue;

		float distance = ctx.m_local->origin( ).dist( player->origin( ) );

		players.push_back( { i, player, distance } );
	}

	// sort players
	auto dist_sort_fn = []( const esp_player_t& a, const esp_player_t& b ) -> bool
			{
				return a.dist > b.dist;
			};

	std::sort( players.begin( ), players.end( ), dist_sort_fn );

	// draw players
	for ( size_t i{ }; i < players.size( ); i++ )
		players.at( i ).draw( );
}

void c_esp::draw_spectator_list( )
{
	// fuck you // * 2 
	std::vector< std::string > spectators;

	if ( csgo.m_engine( )->IsInGame( ) && ctx.m_local )
	{
		for ( auto i = 0; i < csgo.m_globals( )->max_clients; i++ )
		{
			auto player = csgo.m_entity_list( )->get_entity( i );

			if ( !player || player == ctx.m_local )
				continue;

			auto target = player->spec_target( );

			if ( target == INVALID_EHANDLE_INDEX )
				continue;

			if ( csgo.m_entity_list( )->get_entity_from_base_handle( target ) == ctx.m_local )
			{
				player_info_t info;

				if ( !player->info( &info ) )
					continue;

				spectators.push_back( info.name );
			}
		}
	}

	static vec2 pos( 20, 200 );
	if ( !spectators.empty( ) )
	{
		for ( size_t i = 0; i < spectators.size( ); ++i )
			ctx.m_renderer->_string( font_t::font_menu, color_t::boxes::box_text( ), pos.x, pos.y + 12.0f * i, false, spectators.at( i ).c_str( ) );
	}

	spectators.clear( );
}

void c_esp::draw_non_players( )
{
	color_t box_color;

	// setup colours

	for ( size_t i = 1; i < csgo.m_entity_list( )->highest_index( ); i++ )
	{
		esp_box_t box;

		auto ent = csgo.m_entity_list( )->get_entity( i );

		if ( !ent || ent->dormant( ) || !box.create( ent ) )
			continue;

		if ( ctx.m_settings.visuals_dropped_weapons && ent->is_weapon( ) && ent->owner( ) == INVALID_EHANDLE_INDEX )
		{
			auto weapon = static_cast< c_base_weapon* >( ent );

			if ( weapon )
			{
				draw_ent_box( ctx.m_settings.visuals_dropped_weapons_color, box );

				ctx.m_renderer->string( font_t::font_esp, color_t::boxes::box_text( ), ( box.maxx + box.minx ) / 2.f, box.maxy + 2.0f, true, weapon->print_name( ).c_str( ) );

				// need 2 draw an ammo bar based on clip1 remaining out of max_clip1
				//if ( weapon->clip1( ) != -1 )
				//	ctx.m_renderer->string( font_t::esp, color_t::boxes::box_text( ), bot.x + bot_width * 0.5f, bot.y + 1.0f, true, "%i / %i", weapon->clip1( ), weapon->max_clip1( ) );
			}
		}

		int ent_class = ent->client( )->m_class_id;

		if ( ctx.m_settings.visuals_projectiles )
		{
			// im sure theres a vfunc for this

			const char* model_name = csgo.m_model_info( )->GetModelName( ent->model( ) );

			if ( strstr( model_name, xors( "thrown" ) ) )
			{
				const char* print_name = nullptr;

				if ( strstr( model_name, xors( "rag" ) ) )
					print_name = xors( "he grenade" );
				else if ( strstr( model_name, xors( "moke" ) ) )
					print_name = xors( "smoke grenade" );
				else if ( strstr( model_name, xors( "lash" ) ) )
					print_name = xors( "flash grenade" );
				else if ( strstr( model_name, xors( "incen" ) ) || strstr( model_name, xors( "molo" ) ) )
					print_name = xors( "fire grenade" );
				else if ( strstr( model_name, xors( "ecoy" ) ) )
					print_name = xors( "decoy grenade" );

				if ( print_name )
				{
					draw_ent_box( ctx.m_settings.visuals_other_ents_color, box );
					ctx.m_renderer->string( font_t::font_esp, color_t::boxes::box_text( true ), ( box.maxx + box.minx ) / 2.f, box.miny - 12.0f, true, print_name );
				}
			}
		}

		if ( ctx.m_settings.visuals_bomb && ent_class == CPlantedC4 )
		{
			const float explosion_time = ent->detonation_time( ) - csgo.m_globals( )->curtime;

			if ( explosion_time > 0.0f )
			{
				draw_ent_box( ctx.m_settings.visuals_other_ents_color, box );

				ctx.m_renderer->string( font_t::font_esp, color_t::boxes::box_text( true ), ( box.maxx + box.minx ) / 2.f, box.miny - 12.0f, true, xors( "bomb" ) );
				ctx.m_renderer->string( font_t::font_esp, ctx.m_settings.visuals_bomb_color, ( box.maxx + box.minx ) / 2.f, box.maxy + 2.0f, true, xors( "%.1f" ), explosion_time );
			}
		}

		if ( ctx.m_settings.visuals_other_ents && ent_class == CHostage )
		{
			draw_ent_box( ctx.m_settings.visuals_other_ents_color, box );

			ctx.m_renderer->string( font_t::font_esp, color_t::boxes::box_text( ), ( box.maxx + box.minx ) / 2.f, box.miny - 12.0f, true, xors( "hostage" ) );
			//draw_health( box.screen.bot_left, box.screen.top_left, player->health( ), 255, color_t( 0, 0, 0, 175 ) );
		}
	}
}
