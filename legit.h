#pragma once

class c_legit
{
	class c_aim_utils
	{
	public:

		float get_time_delta( )
		{
			static  float  old_time      = csgo.m_globals( )->realtime;

			const   float  current_time  = csgo.m_globals( )->realtime;
			const   float  time_delta    = current_time - old_time;

			old_time = current_time;

			return time_delta;
		}

		bool trace_hit_smoke( vec3 src, vec3 dst )
		{
			static const auto LineGoesThroughSmoke = memory::pattern::first_code_match< bool( __cdecl* )( vec3, vec3 ) >( csgo.m_client.module( ), xors( "80 B9 ? ? ? ? ? 75 6C" ), -0x57 );

			return LineGoesThroughSmoke( src, dst );
		}

		c_base_entity* get_aim_target( )
		{
			static vec3     viewangles;
			float           ideal_fov = static_cast< float >( ctx.m_settings.legit_aim_fov );

			csgo.m_engine( )->GetViewAngles( viewangles );

			for ( size_t i = 1; i < csgo.m_globals( )->max_clients; ++i )
			{
				auto ent = csgo.m_entity_list( )->get_entity( i );

				if ( !ent || !ent->is_player( ) || ( ent->team( ) == ctx.m_local->team( ) && !ctx.m_settings.legit_aim_friendlies ) )
					continue;

				if ( ent == ctx.m_local || ent->dormant( ) || !ent->alive( ) || ent->immune( ) )
					continue;

				vec3 delta_angle = math::_vector_angles( ctx.m_local->head_position( ), ent->head_position( ) ).clamped( );

				const float real_fov = ( delta_angle - viewangles ).clamped( ).length2d( );

				if ( real_fov < ideal_fov )
				{
					ideal_fov = real_fov;
					return ent;
				}
			}

			return nullptr;
		}

		float m_last_aim_time;
		float m_next_aim_time;
	};

public:
	void init_create_move( );
	void init_override_mouse_input( float* x, float* y );

private:
	void triggerbot( );
	void aimbot( float* mouse_x, float* mouse_y );

private:	

	c_aim_utils m_aim_utils;
}; extern c_legit g_legit;