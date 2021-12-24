#pragma once
#include <list>

enum GRENADE_ACT
{
	ACT_NONE,
	ACT_THROW,
	ACT_LOB,
	ACT_DROP,
};

class c_grenade_predictor
{
	std::list<vec3> grenade_path;
	std::vector<vec3> grenade_bounce;

	int type = 0;
	int act = 0;

	void trace_hull( vec3& src, vec3& end, c_game_trace& tr );
	void add_gravity_move( vec3& move, vec3& vel, float frametime, bool onground );
	bool check_detonate( vec3& throw_speed, c_game_trace& tr, int tick, float interval );
	void resolve_fly_collision( c_game_trace& tr, vec3& velocity, float interval );
	int physics_clip_velocity( const vec3& in, const vec3& normal, vec3& out, float overbounce );

	int step( vec3& source, vec3& throw_speed, int tick, float interval );
	void setup( vec3& source, vec3& throw_speed );
	void simulate( );

public:
	void create_move( int buttons );
	void draw( );
}; extern c_grenade_predictor g_grenade_predictor;