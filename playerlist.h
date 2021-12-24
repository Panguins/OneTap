#pragma once
#include "player.h"

struct player_data_t
{
	int index{ };
	c_base_entity* base{ };

	// state info
	bool is_valid{ };
	bool validating_tick{ };

	bool is_dormant{ };
	bool is_alive{ };
	bool is_immune{ };

	bool is_teammate{ };
	bool is_cheater{ };

	vec3 origin{ };
	vec3 velocity{ };
	float speed{ };

	bool is_onground{ };
	bool is_fakewalking{ };

	bool is_simtime_update{ };
	bool is_lby_update{ };
	bool is_shot_update{ };
	bool is_moving{ };

	// data
	float simtime{ };
	float old_simtime{ };
	float animtime{ };
	float lby{ };
	float distance{ };

	int fakelag_ticks{ };
	int choked_ticks{ };
	int health{ };

	CAnimationLayer anim_layers[15];
	CAnimationLayer old_anim_layers[15];
};

class c_playerlist
{
	constexpr static float dist_epsilon = 2.5f;
	player_data_t players[65] { };

	bool on_ground( c_base_entity* entity );
public:
	void update( );

	__forceinline player_data_t* at( int i ) { return &players[i]; }
};

extern c_playerlist g_playerlist;