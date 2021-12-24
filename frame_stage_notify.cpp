#include "context.h"

#include "lagcomp.h"
#include "rage.h"
#include "playerlist.h"
#include "boner.h"
#include "misc.h"
#include "anims.h"

decltype( hooked::original::o_frame_stage_notify ) hooked::original::o_frame_stage_notify;

vec3 original_punch;
vec3 original_view_punch;

void world_modulation( )
{
	struct ugh_t
	{
		std::string mat_name; std::string mat_group;
	};

	static std::vector< ugh_t > mats = {};

	if ( ctx.m_settings.visuals_render_mode == 3 || ctx.m_settings.visuals_nosky || ctx.m_settings.visuals_asus_walls < 100 || ctx.m_settings.visuals_world_color )
	{
		if ( ctx.m_settings.visuals_render_mode == 3 )
		{
			static const auto load_sky = memory::pattern::first_code_match<void( __cdecl* )( const char* )>( csgo.m_engine.module( ), xors( "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45" ) );
			//load_sky( "sky_wasteland02" );
		}

		static bool is_night_mode = ctx.m_settings.visuals_render_mode == 3;

		for ( uint16_t i = csgo.m_material_system( )->first_material( ); i != csgo.m_material_system( )->invalid_material( ); i = csgo.m_material_system( )->next_material( i ) )
		{
			auto mat = csgo.m_material_system( )->get( i );

			if ( !mat )
				continue;

			std::string group = mat->GetTextureGroupName( );
			const float alpha = static_cast< float >( ctx.m_settings.visuals_asus_walls ) / 100;

			if ( ctx.m_settings.visuals_nosky )
			{
				if ( group.find( TEXTURE_GROUP_SKYBOX ) != std::string::npos )				
					mat->set_flag( MATERIAL_VAR_NO_DRAW, true );				
			}

			if ( group.find( TEXTURE_GROUP_WORLD ) != std::string::npos )
			{
				if ( ctx.m_settings.visuals_render_mode == 3 )
				{
					//mats.push_back( { mat->GetName( ), group } );
					mat->ColorModulate( 0.05f, 0.03f, 0.03f );
				}
				else if ( ctx.m_settings.visuals_world_color )
				{
					//mats.push_back( { mat->GetName( ), group } );
					mat->ColorModulate( ctx.m_settings.visuals_world_color_val( ).r( ) / 255.0f, ctx.m_settings.visuals_world_color_val( ).g( ) / 255.0f, ctx.m_settings.visuals_world_color_val( ).b( ) / 255.0f );
				}

				if ( !strstr( mat->GetName( ), xors( "dev/dev_measuregeneric01b" ) ) || !strstr( csgo.m_engine( )->GetLevelName( ), xors( "aim_" ) ) )
				{
					//mats.push_back( { mat->GetName( ), group } );
					mat->AlphaModulate( alpha );
				}
			}

			if ( is_night_mode )
			{
				if ( ctx.m_settings.visuals_render_mode == 3 )
				{
					if ( group.find( xors( "StaticProp" ) ) != std::string::npos )
					{
						//mats.push_back( { mat->GetName( ), group } );
						mat->ColorModulate( 0.4f, 0.4f, 0.4f );
						mat->AlphaModulate( alpha );
					}
					else if ( group.find( TEXTURE_GROUP_MODEL ) != std::string::npos || group.find( TEXTURE_GROUP_SKYBOX ) != std::string::npos )
					{
						//mats.push_back( { mat->GetName( ), group } );
						mat->ColorModulate( 0.4f, 0.4f, 0.4f );
					}
					else if ( group.find( TEXTURE_GROUP_VIEW_MODEL ) != std::string::npos )
					{
						//mats.push_back( { mat->GetName( ), group } );
						mat->ColorModulate( 0.1f, 0.1f, 0.1f );
					}
				}
			}
		}
	}

	/*if ( ctx.m_settings.visuals_render_mode != 3 && !mats.empty( ) )
	{
		for ( size_t i = 0; i < mats.size( ); i++ )
		{
			auto mat = csgo.m_material_system( )->find( mats[i].mat_name.c_str( ), mats[i].mat_group.c_str( ) );

			if ( !mat )
			{
				mats.erase( mats.begin( ) + i );
				continue;
			}

			mat->ColorModulate( 1.0f, 1.0f, 1.0f );
			mat->AlphaModulate( 1.0f );

			mats.erase( mats.begin( ) + i );
		}
	}*/
}
void pre_fsn( ClientFrameStage_t stage )
{
	switch ( stage )
	{
		case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
		{
			break;
		}

		case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
		{
			break;
		}

		case FRAME_NET_UPDATE_START:
		{
			break;
		}

		case FRAME_NET_UPDATE_END:
		{
			break;
		}

		case FRAME_RENDER_START:
		{
			if ( csgo.m_input( )->m_fCameraInThirdPerson )			
				ctx.m_local->v_angle( ) = ctx.m_settings.rage_anti_aim ? vec3( ctx.m_real_angle.x, ctx.m_real_angle.y, 0.0f ) : ctx.m_cmd->m_viewangles;			

			world_modulation( );

			if ( ctx.m_settings.visuals_no_flash )
			{
				static const uint32_t flash_offset = g_netvars.get_netvar( fnv( "DT_CSPlayer" ), fnv( "m_flFlashDuration" ) );
				*reinterpret_cast< float* >( ctx.m_local + flash_offset ) = 0.0f;
			}

			if ( ctx.m_settings.visuals_no_smoke )
			{
				// RemoveAllSmokeGrenades inlined func basically
				static int* dword_14ECAD34 = *memory::pattern::first_code_match< int** >( csgo.m_client.module( ), xors( "55 8B EC 83 EC 08 8B 15 ?? ?? ?? ?? 0F 57 C0" ), 8 );
				*dword_14ECAD34 = 0;

				const std::vector< const char* > materials =
				{
					xors( "particle/vistasmokev1/vistasmokev1_fire" ),
					xors( "particle/vistasmokev1/vistasmokev1_smokegrenade" ),
					xors( "particle/vistasmokev1/vistasmokev1_emods" ),
					xors( "particle/vistasmokev1/vistasmokev1_emods_impactdust" ),
				};
				
				for ( size_t i = 0; i < materials.size( ); ++i )				
					csgo.m_material_system( )->find( materials[i], TEXTURE_GROUP_OTHER )->set_flag( MATERIAL_VAR_WIREFRAME, true );				
			}

			// interp killa
			if ( ctx.m_settings.rage_aimbot_enabled )
			{
				for ( int i = 0; i < 65; i++ )
				{
					auto ent = csgo.m_entity_list( )->get_entity( i );

					if ( !ent || ent == ctx.m_local )
						continue;

					ent->set_interpolation( !ctx.m_settings.rage_aimbot_enabled );
				}
			}
		}

		default:
			break;
	}
}

void post_fsn( ClientFrameStage_t stage )
{
	switch ( stage )
	{
		case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
		{
			g_playerlist.update( );
			//resolver sets angles
			g_anims.update( );
			g_boner.update( );
			//g_lagcomp.fsn();

			g_misc.ragdolls( );
			break;
		}

		case FRAME_NET_UPDATE_POSTDATAUPDATE_END:
		{
			break;
		}

		case FRAME_NET_UPDATE_END:
		{
			break;
		}

		case FRAME_RENDER_START:
		{
			if ( csgo.m_input( )->m_fCameraInThirdPerson )
			{
				// set radar angle to lby
				ctx.m_local->v_angle( ).y = ctx.m_local->lowerbodyyaw( );
			}

			g_anims.update_local( );

			if ( csgo.m_input( )->m_fCameraInThirdPerson )
			{
				g_boner.update_local( );
			}
		}	

		default:
			break;
	}
}

void __fastcall hooked::frame_stage_notify( void* ecx, void* edx, ClientFrameStage_t stage )
{
	if ( csgo.m_engine( )->IsInGame( ) )
	{
		ctx.m_tickrate = static_cast<uint32_t>( 1.0f / csgo.m_globals( )->interval_per_tick );

		if ( stage == FRAME_NET_UPDATE_END )
			ctx.m_local = csgo.m_entity_list( )->get_entity( csgo.m_engine( )->GetLocalPlayer( ) );
	}
	else
	{
		ctx.m_tickrate  = 0;
		ctx.m_local     = nullptr;
	}

	if ( !ctx.m_screen_w || !ctx.m_screen_h )
		csgo.m_engine( )->GetScreenSize( ctx.m_screen_w, ctx.m_screen_h );

	if ( !csgo.m_engine( )->IsInGame( ) || !ctx.m_local || !ctx.m_cmd )
		return hooked::original::o_frame_stage_notify( ecx, edx, stage );

	pre_fsn( stage );

	hooked::original::o_frame_stage_notify( ecx, edx, stage );

	post_fsn( stage );
}
