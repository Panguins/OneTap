#pragma once

struct anim_data_t
{
	int flags{ };
	float duck_amount{ };

	int o_flags{ };
	float o_duck_amount{ };

	void backup( c_base_entity* player );
	void restore( c_base_entity* player );
};

struct local_data_t
{
	bool m_should_update_anims{ };
	bool m_is_fakewalking{ };
	int m_anim_update_tickbase{ };
	vec3 m_real_angle{ };
	vec3 m_anim_origin{ };
	float m_next_lby_update{ };

	bool is_lby_update( ); //call between prediction start and end
};

class c_anims
{
	local_data_t m_local_data{ };
	anim_data_t m_data[65]{ };

	void duck_fix( c_base_entity* player );
	void jump_fix( c_base_entity* player );
	void origin_fix( c_base_entity* player );
	void velocity_fix( c_base_entity* player );

	void pre_anim_update( c_base_entity* player );
	void anim_update( c_base_entity* player );
	void post_anim_update( c_base_entity* player );
public:
	local_data_t* local_data( ) { return &m_local_data; }

	void update( );
	void update_local( );

}; extern c_anims g_anims;