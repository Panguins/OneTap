#include "context.h"
#include "lagcomp_legit.h"

decltype( hooked::original::o_draw_model_execute ) hooked::original::o_draw_model_execute;

void __fastcall hooked::draw_model_execute(
	void* ecx,
	void* edx,
	c_render_context* context,
	const int& state,
	const c_model_render_info& info,
	const matrix3x4_t* btw
)
{
	static IMaterial* glowy_mat{ };
	static IMaterial* chams_mat{ };
	static IMaterial* trans_mat{ };

	if ( !glowy_mat )
	{
		// pulse mat
		glowy_mat = csgo.m_material_system( )->find( xors( "models/inventory_items/dogtags/dogtags_outline" ), TEXTURE_GROUP_MODEL, true, 0 );

		// tell engine there's a new mat in town
		glowy_mat->AddRef( );
	}

	if ( !chams_mat )
	{
		// pulse mat
		chams_mat = csgo.m_material_system( )->find( xors( "debug/debugambientcube" ), TEXTURE_GROUP_MODEL, true, 0 );

		// tell engine there's a new mat in town
		chams_mat->AddRef( );
	}

	if ( !trans_mat )
	{
		trans_mat = csgo.m_material_system( )->find( xors( "debug/debugtranslucentsinglecolor" ), TEXTURE_GROUP_MODEL, true, 0 );

		// tell engine there's a new mat in town
		trans_mat->AddRef( );
	}

	ctx.m_render_ctx = context;

	/*auto ent = csgo.m_entity_list( )->get_entity( info.m_ent_index );

	bool draw_this_call = true;

	if ( ent )
	{
		const  bool  good_ent_enemy  = ent->team( ) != ctx.m_local->team( ) && ctx.m_settings.players_chams_enemy;
		const  bool  good_ent_team   = ent->team( ) == ctx.m_local->team( ) && ctx.m_settings.players_chams_friendly;
		const  bool  good_ent        = good_ent_team || good_ent_enemy;

		if ( good_ent && strstr( csgo.m_model_info( )->GetModelName( info.m_model ), "player" ) )
			draw_this_call = false;
	}

	if ( draw_this_call )*/
	hooked::original::o_draw_model_execute( ecx, edx, context, state, info, btw );

	if ( ctx.m_settings.rage_aimbot_enabled && info.m_ent_index == ctx.m_local->index( ) && csgo.m_input( )->m_fCameraInThirdPerson )
	{
		if ( ctx.m_settings.misc_fakelag_visualize && ctx.m_local->velocity( ).length2d( ) > 1.0f )
		{
			color_t chams_color = ctx.m_settings.misc_fakelag_visualize_color;

			if ( ctx.m_settings.visuals_indicate_lc && ctx.m_lag_handler->breaking_lag_comp( ) )
				chams_color = color_t( 100, 250, 100 );

			csgo.m_render_view( )->SetColorModulation( chams_color.base( ).raw( ) );
			csgo.m_model_render( )->ForcedMaterialOverride( glowy_mat );

			hooked::original::o_draw_model_execute( ecx, edx, context, state, info, ctx.m_lag_handler->get_server_matrix( ) );

			csgo.m_model_render( )->ForcedMaterialOverride( 0 );
		}

		if ( ( ctx.m_settings.rage_yaw > 0 && ctx.m_settings.rage_fake_yaw > 0 ) && ctx.m_settings.visuals_indicate_lby && btw )
		{
			matrix3x4_t rotated[128];

			for ( size_t i = 0; i < 128; ++i )
			{
				math::MatrixCopy( btw[i], rotated[i] );
				math::RotateMatrix( info.m_angles, info.m_origin, info.m_angles.y - ctx.m_local->lowerbodyyaw( ), rotated[i] );
			}

			// make red if last update is less than 200ms old - FLYINGLLAMA

			csgo.m_render_view( )->SetColorModulation( color_t( 230, 230, 100, 200 ).base( ).raw( ) );
			csgo.m_model_render( )->ForcedMaterialOverride( trans_mat );

			hooked::original::o_draw_model_execute( ecx, edx, context, state, info, rotated );

			csgo.m_model_render( )->ForcedMaterialOverride( 0 );
		}		
	}

	if ( ctx.m_settings.legit_enabled && ctx.m_settings.legit_backtrack_visualize && g_lagcomp_legit.current_target && info.m_ent_index == g_lagcomp_legit.current_target->index( ) )
	{
		color_t chams_color = ctx.m_settings.misc_fakelag_visualize_color;

		csgo.m_render_view( )->SetColorModulation( chams_color.base( ).raw( ) );
		csgo.m_model_render( )->ForcedMaterialOverride( chams_mat );

		hooked::original::o_draw_model_execute( ecx, edx, context, state, info, g_lagcomp_legit.current_matrix );

		csgo.m_model_render( )->ForcedMaterialOverride( 0 );
	}
}