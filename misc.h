#pragma once

class c_misc
{
public:
	void init();

	void bunnyhop();
	void auto_strafe();
	void placeholder_strafe( c_user_cmd* cmd );
	void fakelag();
	void display_ranks();
	void clantag();
	void ragdolls( );
	void auto_defuse( );

	class c_strafe
	{
	public:
		static float get_move_angle(float speed);
		static bool get_closest_plane(vec3* plane);
		static bool will_hit_obstacle_in_future(float predict_time, float step);
		static void circle_strafe(c_user_cmd* cmd, float* circle_yaw);
		static void strafe(c_user_cmd* cmd);
	};
};

extern c_misc g_misc;

class c_move_fix
{
public:
	inline void start( c_user_cmd* cmd )
	{
		if ( !cmd )
			return;

		m_viewangles = cmd->m_viewangles;
		m_move_dir = cmd->m_move_dir;
	}

	inline void finish( c_user_cmd* cmd )
	{
		// nigga ass move fix
		// no ladder fix etc, cuz fuck that shit, we climb scoreboard not ladders

		// calculate delta
		float old_yaw = math::float_normalize( m_viewangles.y, 0.f, 360.f );
		float new_yaw = math::float_normalize( cmd->m_viewangles.y, 0.f, 360.f );

		float yaw_delta = new_yaw < old_yaw ? 360.f - abs( new_yaw - old_yaw ) : abs( old_yaw - new_yaw );

		// calculate new move
		float forward_move = cos( math::deg2rad( yaw_delta ) ) * m_move_dir[0] + cos( math::deg2rad( yaw_delta + 90.f ) ) * m_move_dir[1];
		float side_move    = sin( math::deg2rad( yaw_delta ) ) * m_move_dir[0] + sin( math::deg2rad( yaw_delta + 90.f ) ) * m_move_dir[1];

		// polak fix
		float pitch = math::float_normalize( cmd->m_viewangles.x, -180.f, 180.f );
		if ( pitch == -180.f || pitch == 180.f )
			forward_move = -forward_move;

		// lets not get untrusted, theres no reason not to clamp it anyways
		cmd->m_move_dir[0] = std::clamp<float>( forward_move, -450.f, 450.f );
		cmd->m_move_dir[1] = std::clamp<float>( side_move, -450.f, 450.f );

		if ( ctx.m_local->move_type( ) != MOVETYPE_LADDER )
		{
			cmd->m_buttons &= ~( IN_BACK | IN_FORWARD | IN_MOVELEFT | IN_MOVERIGHT );

			// maybe make an option to invert so u can intentionally skatewalk

			if ( cmd->m_move_dir[0] != 0.f )
				cmd->m_buttons |= cmd->m_move_dir[0] > 0.f ? IN_FORWARD : IN_BACK;

			if ( cmd->m_move_dir[0] != 0.f )
				cmd->m_buttons |= cmd->m_move_dir[1] > 0.f ? IN_MOVERIGHT : IN_MOVELEFT;
		}
	}

private:
	vec3 m_viewangles;
	vec3 m_move_dir;
};

extern c_move_fix g_move_fix;
