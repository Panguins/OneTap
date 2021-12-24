#pragma once

class c_fire_bullet_params
{
public:
	vec3             m_src;
	c_game_trace     m_enter_tr;
	vec3             m_dir;
	c_trace_filter   m_filter;
	float            m_range;
	float            m_len;
	float            m_remaining_len;
	float            m_dmg;
	int              m_pen_count;
	c_base_entity*   m_target;

	c_base_entity*   m_shooter;
	weapon_info_t*   m_weapon_info;

	bool             m_simulated_shot;
};

class c_auto_wall
{
	void scale_damage(int hitgroup, c_base_entity* ent, float armor_rat, float& out_dmg) const
	{
		auto has_armor = [](c_base_entity* player, int armor, int hitgroup)
		{
			bool result = false;

			if (armor > 0)
			{
				switch (hitgroup)
				{
				case HITGROUP_GENERIC:
				case HITGROUP_CHEST:
				case HITGROUP_STOMACH:
				case HITGROUP_LEFTARM:
				case HITGROUP_RIGHTARM:
					result = (armor > 0);
					break;
				case HITGROUP_HEAD:
					result = player->has_helmet();
					break;
				}
			}

			return result;
		};
		auto get_dmg = [](int hitgroup)
		{
			switch (hitgroup)
			{
			case HITGROUP_HEAD:
				return 4.0f;
			case HITGROUP_CHEST:
			case HITGROUP_GEAR:
			case HITGROUP_GENERIC:
				return 1.0f;
			case HITGROUP_STOMACH:
				return 1.5f;
			case HITGROUP_LEFTARM:
			case HITGROUP_RIGHTARM:
				return 1.0f;
			case HITGROUP_LEFTLEG:
			case HITGROUP_RIGHTLEG:
				return 0.75f;
			default:
				return 1.0f;
				break;
			}
		};

		auto armor = ent->armor();

		switch (hitgroup)
		{
		case HITGROUP_HEAD:
			out_dmg *= 4.f;
			break;
		case HITGROUP_STOMACH:
			out_dmg *= 1.25f;
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			out_dmg *= 0.75f;
			break;
		}

		if (has_armor(ent, armor, hitgroup))
		{
			auto v47 = 1.f, armor_bonus_ratio = 0.5f, armor_ratio = armor_rat * 0.5f;

			auto dmg = out_dmg * armor_ratio;

			if (((out_dmg - (out_dmg * armor_ratio)) * (v47 * armor_bonus_ratio)) > armor)
				dmg = out_dmg - (armor / armor_bonus_ratio);

			out_dmg = dmg;
		}
	}

	bool fire_bullet(c_fire_bullet_params& params) const;
	bool handle_bullet_penetration(c_fire_bullet_params& params) const;
	bool trace_to_exit(vec3 src, vec3 dir, vec3& out_end, const c_game_trace& enter, c_game_trace& exit) const;

public:
	static bool is_visible( const vec3& start, const vec3& end, c_base_entity* start_ent = nullptr, c_base_entity* end_ent = nullptr );
	float get_damage( c_base_entity* shooter, c_base_entity* target, const vec3& point, weapon_info_t* custom_weapon = nullptr, const vec3* start = nullptr ) const;
	void clip_trace_to_players( const vec3& start, const vec3& end, int mask, c_game_trace* old_trace, c_base_entity* ent ) const;

	static weapon_info_t* get_big_fucking_gun( )
	{
		static weapon_info_t big_fucking_gun{ };
		big_fucking_gun.damage = 200;
		big_fucking_gun.range_modifier = 1.0f;
		big_fucking_gun.penetration = 6.0f;
		big_fucking_gun.armor_ratio = 2.0f;
		big_fucking_gun.range = 8192.f;
		return &big_fucking_gun;
	}

	static weapon_info_t* get_awp( )
	{
		static weapon_info_t awp{ };
		awp.damage = 115;
		awp.range_modifier = 0.99f;
		awp.penetration = 2.5f;
		awp.armor_ratio = 1.95f; //awp 1.95
		awp.range = 8192.f;
		return &awp;
	}
}; extern c_auto_wall g_auto_wall;