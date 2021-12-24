#include <algorithm>
#include "interfaces.h"
#include "context.h"
#include "player.h"
#include "rage.h"

decltype( hooked::original::o_scene_end ) hooked::original::o_scene_end;

void __fastcall hooked::scene_end( void* ecx, void* edx )
{
	if ( ( !ctx.m_settings.players_chams_friendly && !ctx.m_settings.players_chams_enemy ) || !ctx.m_local )
		return hooked::original::o_scene_end( ecx, edx );

	hooked::original::o_scene_end( ecx, edx );

	static IMaterial* mat{};

	if ( !mat )
	{
		// polak material, textured but has an issue of pulsing in brightness over time
		// other notable materials:
		//
		// debug/debugdrawflat - flat chams
		// vgui/achievements/glow - ghost nigga
		// models/inventory_items/dogtags/dogtags_outline - pulsates like POLICE
		// debug/debugtranslucentsinglecolor - translucnt
		mat = csgo.m_material_system( )->find( xors( "debug/debugambientcube" ), TEXTURE_GROUP_MODEL, true, 0 );

		// tell engine there's a new mat in town
		mat->AddRef( );

		// idk if any of these are necessary
		mat->set_flag( MATERIAL_VAR_NOFOG, true );
		mat->set_flag( MATERIAL_VAR_NOALPHAMOD, true );
		mat->set_flag( MATERIAL_VAR_BASEALPHAENVMAPMASK, true );
		mat->set_flag( MATERIAL_VAR_OPAQUETEXTURE, true );
	}

	if ( ctx.m_settings.player_chams_reflectivity > 0 )
	{
		const float new_reflectivity = static_cast< float >( ctx.m_settings.player_chams_reflectivity ) / 100.0f;
		
		mat->FindVar( xors( "$reflectivity" ), nullptr, false )->SetVecValue( new_reflectivity, new_reflectivity, new_reflectivity );
	}

	if ( ctx.m_settings.player_chams_shine > 0 )
	{
		auto basetexture       = mat->FindVar( xors( "$basetexture" ), nullptr, false );
		auto envmap            = mat->FindVar( xors( "$reflectivity" ), nullptr, false );
		auto envmaptint        = mat->FindVar( xors( "$envmaptint" ), nullptr, false );
		auto envmapcontrast    = mat->FindVar( xors( "$envmapcontrast" ), nullptr, false );
		auto envmapsaturation  = mat->FindVar( xors( "$envmapsaturation" ), nullptr, false );

		const float shine = static_cast< float >( ctx.m_settings.player_chams_shine ) / 100.0f;

		basetexture->SetStringValue( xors( "vgui/white" ) );
		envmap->SetStringValue( xors( "env_cubemap" ) );
		envmaptint->SetVecValue( shine, shine, shine );
		envmapsaturation->SetFloatValue( shine );
	}

	for ( size_t i = 1; i < csgo.m_globals( )->max_clients; ++i )
	{
		auto ent = csgo.m_entity_list( )->get_entity( i );

		// 4096 ref: https://developer.valvesoftware.com/wiki/Entity_limit
		if ( !ent || i > 4096 )
			continue;

		// ghetto way of checking if the ent is a C_BasePlayer
		// see C_BaseEntity::IsPlayer, 1 before IsBaseCombatCharacter, or is_weapon in our hack
		if ( ent->client( )->m_class_id != CCSPlayer /* || player->is_weapon( ) */ )
			continue;

		// dormant ent, could store bone matrix of last non-dormant tick
		// and use that, for dormancy esp
		if ( ent->dormant( ) || !ent->alive( ) )
			continue;

		// team check based on user's desire for enemy only or both teams chams
		{
			if ( ent->team( ) != ctx.m_local->team( ) && !ctx.m_settings.players_chams_enemy )
				continue;

			if ( ent->team( ) == ctx.m_local->team( ) && !ctx.m_settings.players_chams_friendly )
				continue;
		}

		// only draw chams on local if we're in thirdperson
		if ( ent->index( ) == ctx.m_local->index( ) && !csgo.m_input( )->m_fCameraInThirdPerson )
			continue;

		const  bool  xqz_enemy     = ent->team( ) != ctx.m_local->team( ) && ctx.m_settings.players_xqz_enemy;
		const  bool  xqz_friendly  = ent->team( ) == ctx.m_local->team( ) && ctx.m_settings.players_xqz_friendly;
		const  bool  xqz           = xqz_enemy || xqz_friendly;

		mat->set_flag( MATERIAL_VAR_IGNOREZ, xqz );

		color_t chams_color = ent->team( ) != ctx.m_local->team( ) ? ctx.m_settings.players_chams_enemy_color : ctx.m_settings.players_chams_friendly_color;
		csgo.m_render_view( )->SetColorModulation( chams_color.base( ).raw( ) );
		csgo.m_model_render( )->ForcedMaterialOverride( mat );

		ent->draw_model( 979 );

		// fps killer but it makes the chams look way nicer by fixing transparency
		if ( xqz )
		{
			mat->set_flag( MATERIAL_VAR_IGNOREZ, false );
			ent->draw_model( 979 );
		}	
	}

	csgo.m_model_render( )->ForcedMaterialOverride( nullptr );
}
