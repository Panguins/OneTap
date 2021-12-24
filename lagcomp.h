#pragma once
#include "context.h"
#include <deque>
#include <array>

class c_lagcomp
{
	struct record_entry_t
	{
		explicit record_entry_t( c_base_entity* entity );

		vec3 m_origin, m_eye_angles, m_abs_angles, m_render_angles, m_bbmin, m_bbmax, m_velocity;
		float m_simtime, m_curtime;
		int m_flags;
		bool m_lby_updated, m_lby_broke_lag_comp, m_fakewalking;

		c_base_entity* m_ent;
		CCSGOPlayerAnimState m_anim_state;
		CAnimationLayer m_anim_layers[15];
		matrix3x4_t m_bone_matrix[128];
		float m_pose_params[24];

		// Resolver Record
	};

	struct record
	{
		c_base_entity* m_ent;
		std::deque< record_entry_t > m_data;

		void reset( );
	};

	c_user_cmd* m_cmd{ };
	std::array< record, 65 > m_records{ };

	void store_records( );
	void set_tickcount( );
public:
	static float get_interpolation( );
	static bool is_tick_valid( int tickcount );

	void create_move( c_user_cmd* cmd );
	void frame_net_update_end( );
};

extern c_lagcomp g_lagcomp;
