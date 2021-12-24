#pragma once
#include "playerlist.h"

struct possible_point_t
{
	int m_target_hitbox;
	float m_damage;
	bool m_checked_damage, m_checked_hitchance;
	vec3 m_aimpoint;

	possible_point_t( const vec3& aimpoint, int target_hitbox, float damage = 0.f )
		: m_target_hitbox( target_hitbox ), m_damage( damage ), m_checked_damage( false ), m_checked_hitchance( false ), m_aimpoint( aimpoint ) {}
};

struct possible_target_t
{
	player_data_t* m_data;
	std::vector< possible_point_t > m_points;
	vec3 m_final_aimpoint;

	explicit possible_target_t( player_data_t* data = nullptr, const std::vector< possible_point_t >& points = std::vector< possible_point_t >{ } )
		: m_data( data ), m_points( points ) { }
};

class c_target_selector
{
	bool is_hitbox_checked( int index, int hitbox );
	bool find_normal_target( possible_target_t& target );
	bool find_backtrack_target( possible_target_t& target );

	static __forceinline vec3 lerp( const vec3& start, const vec3& end, float lerp )
	{
		return start * lerp + end * ( 1.f - lerp );
	}

public:
	bool find_target( possible_target_t& target );
	int get_highest_threat( );

	bool has_remaining_shots[65];
	std::vector< possible_target_t > possible_targets;
};

extern c_target_selector g_target_selector;
