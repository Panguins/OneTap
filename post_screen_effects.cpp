#include "context.h"

decltype( hooked::original::o_post_screen_effects ) hooked::original::o_post_screen_effects;

int __fastcall hooked::post_screen_effects( void* ecx, void* edx, CViewSetup* view )
{
	if ( !ctx.m_local )
		return hooked::original::o_post_screen_effects( ecx, edx, view );

	if ( !ctx.m_settings.players_esp || ( !ctx.m_settings.players_glow_enemy && !ctx.m_settings.players_glow_friendly ) || !ctx.m_render_ctx )
		return hooked::original::o_post_screen_effects( ecx, edx, view );

	auto glow = [&]( const ColorF& color, c_base_entity* ent )
	{
		static  c_stencil_state  state{};
		static  IMaterial*       glow_mat{};

		if ( !state.m_enabled )
		{
			state.m_enabled = true;
			state.m_reference_value = 1;
			state.m_compare_function = STENCILCOMPARISONFUNCTION_ALWAYS;
			state.m_pass_operation = STENCILOPERATION_REPLACE;
			state.m_fail_operation = STENCILOPERATION_KEEP;
			state.m_z_fail_op = STENCILOPERATION_REPLACE;
		}

		if ( !glow_mat )
		{
			glow_mat = csgo.m_material_system( )->find( "dev/glow_color", TEXTURE_GROUP_OTHER );
			glow_mat->AddRef( );
		}

		ctx.m_render_ctx->set_stencil_state( state );

		csgo.m_model_render( )->ForcedMaterialOverride( glow_mat );
		csgo.m_render_view( )->SetColorModulation( color.raw( ) );
		csgo.m_render_view( )->SetBlend( color.a( ) );

		ent->draw_model( 1 );
	
		state.m_enabled = false;
		ctx.m_render_ctx->set_stencil_state( state );
	};

	static  auto   object  = *memory::pattern::first_code_match< c_object_manager** >( csgo.m_client.module( ), xors( "75 4B 0F 57 C0" ), 0x12 );

	if ( !object )
		return hooked::original::o_post_screen_effects( ecx, edx, view );

	if ( !object->m_size || object->m_size > object->m_max_size )
		return hooked::original::o_post_screen_effects( ecx, edx, view );

	for ( size_t i = 0; i < object->m_size; ++i )
	{
		auto definition  = &object->m_glow_object_definitions[i];
		auto ent         = definition->m_ent;

		if ( !ent )
			continue;
			
		if ( ent->is_weapon( ) )
		{
			//definition->glow( color_t::boxes::ent( ), alpha );
		}
		else
		{
			switch ( ent->client( )->m_class_id )
			{
				case CCSPlayer:
				{
					bool enemy = ent->team( ) != ctx.m_local->team( );

					if ( !enemy && ctx.m_settings.players_esp != 2 )
						continue;

					if ( ( enemy && !ctx.m_settings.players_glow_enemy ) || ( !enemy && !ctx.m_settings.players_glow_friendly ) )
						continue;

					color_t color = enemy ? ctx.m_settings.players_glow_enemy_color : ctx.m_settings.players_glow_friendly_color;

					definition->glow( color );
					//glow( color.base( ), ent );
					break;
				}

				case CHostage:
				{
					//definition->glow( color_t::boxes::character( ), alpha );
					break;
				}

				case CPlantedC4:
				{
					//definition->glow( color_t::boxes::t( ), alpha );
					break;
				}

				case CEconEntity:
				{
					//definition->glow( color_t::boxes::ct( ), alpha );
					break;
				}

				case CBaseCSGrenadeProjectile:
				case CSmokeGrenadeProjectile:
				case CMolotovProjectile:
				case CDecoyProjectile:
				{
					//definition->glow( color_t::boxes::ent( ), alpha );
					break;
				}
			}
		}
	}

	return hooked::original::o_post_screen_effects( ecx, edx, view );
}