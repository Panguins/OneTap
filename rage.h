#pragma once

enum player_t
{
	invalid = 1337,
};

class c_anti_aim;
class c_aimbot
{
public:

	void init( );
	c_base_entity* m_target;
	vec3 m_aim_point;
	uint32_t m_target_hitbox;

	bool can_hit_point(const vec3& angle, c_base_entity* ent, bool anywhere_on_ent = true, uint32_t hitbox = 0);
	bool can_hit_point_estimate(c_base_entity* ent, uint32_t hitbox_index);

private:
	void aim( );
	void nospread( );
	int  find_ideal_target( );
}; extern c_aimbot g_aimbot;