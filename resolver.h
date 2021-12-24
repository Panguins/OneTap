#pragma once
#include "resolver_sets.h"
#include "context.h"
#include "lagcomp.h"

namespace resolver_data
{
	struct simulate_record
	{
		bool valid_record{ };

		float simtime = 0.f;
		int tickcount = 0;

		vec3 origin, angles;
		vec3 abs_origin, abs_angles;

		vec3 mins, maxs;
		matrix3x4_t bone_matrix[128];

		int flags;

		vec3 velocity;

		float pose_params[24];
		CAnimationLayer anim_layers[15];


		void create( c_base_entity* ent, float* overwrite_yaw = nullptr )
		{
			simtime = ent->simulation_time( );
			tickcount = math::time::to_ticks( simtime + c_lagcomp::get_interpolation( ) );

			origin = ent->origin( );
			abs_origin = ent->abs_origin( );

			angles = ent->eye_angles( );
			abs_angles = ent->abs_angles( );

			mins = ent->mins( );
			maxs = ent->maxs( );

			flags = ent->flags( );
			velocity = ent->velocity( );

			if ( overwrite_yaw )
			{
				angles.y = *overwrite_yaw;
				abs_angles.y = *overwrite_yaw;
			}

			matrix3x4_t* bone_cache = ent->bone_cache( );
			size_t bone_cache_count = ent->bone_cache_count( );

			valid_record = !!bone_cache && !!bone_cache_count;

			if ( valid_record )
			{
				memcpy( pose_params, ent->pose_parameters( ), 24 * sizeof( float ) );
				memcpy( anim_layers, ent->anim_overlay( ), 15 * sizeof CAnimationLayer );
				memcpy( bone_matrix, bone_cache, bone_cache_count * sizeof( matrix3x4_t ) );
			}
		}

		void restore( c_base_entity* ent )
		{
			if ( !valid_record )
				return;

			ent->simulation_time( ) = simtime;

			ent->origin( ) = origin;
			*(vec3*)( (uintptr_t)ent + 0xA0 ) = abs_origin;
			ent->set_abs_origin( abs_origin );

			ent->eye_angles( ) = angles;
			*(vec3*)( (uintptr_t)ent + 0xC4 ) = abs_angles;
			ent->set_abs_angles( abs_angles );

			ent->mins( ) = mins;
			ent->maxs( ) = maxs;

			ent->velocity( ) = velocity;
			ent->flags( ) = flags;

			matrix3x4_t* bone_cache = ent->bone_cache( );
			size_t bone_cache_count = ent->bone_cache_count( );

			valid_record = !!bone_cache && !!bone_cache_count;

			if ( valid_record )
				memcpy( bone_cache, bone_matrix, bone_cache_count * sizeof( matrix3x4_t ) );
		}

		bool is_valid( )
		{
			return valid_record && c_lagcomp::is_tick_valid( tickcount );
		}
	};

	struct cm_shot_t
	{
		float time;
		int target_id;
		int hitgroup;

		resolver_steps_t step;

		vec3 origin;
		vec3 pos;

		bool did_backtrack;

		bool has_record;
		simulate_record record;
	};

	struct player_hurt_t
	{
		float time;
		int target_id;
		int hitgroup;
		int damage;
	};

	struct impact_t
	{
		float time;
		vec3 end_pos;
		std::vector<vec3> impacts;
	};

	struct match_t
	{
		bool miss;
		cm_shot_t shot;
		impact_t impact;
		player_hurt_t hurt;
	};

	struct resolver_data_t
	{
		c_base_entity* ent{ };

		int shots{ };
		bool valid_log{ };
		resolver_steps_t logged_step{ };

		bool valid_last_moving{ };
		float last_moving_lby{ };
	};

	struct impact_resolves_t
	{
		std::vector<simulate_record> records;

		void create( c_base_entity* ent, float resolved_yaw )
		{
			clean( );

			simulate_record new_record;
			new_record.create( ent, &resolved_yaw );
			records.push_back( new_record );
		}

		void clean( )
		{
			if ( records.empty( ) )
				return;

			for ( size_t i = 0; i < records.size( ); i++ )
			{
				if ( !records.at( i ).is_valid( ) )
				{
					records.erase( records.begin( ) + i );
					i--;
				}
			}
		}
	};
}

class c_resolver
{
	std::deque<resolver_data::cm_shot_t>     m_shots;
	std::deque<resolver_data::player_hurt_t> m_hurts;
	std::deque<resolver_data::impact_t>      m_impacts;

	std::deque<resolver_data::match_t>       m_matches;

	resolver_data::resolver_data_t           m_data[65]{ };

	void sort_impacts( );
	void match_impacts( );
	void resolve_matches( );

	float get_freestand_yaw( c_base_entity* target );

	float get_yaw_step( c_base_entity* ent, resolver_data::resolver_data_t* data );
	float get_pitch_step( c_base_entity* ent, resolver_data::resolver_data_t* data );

	bool missed_due_to_spread( resolver_data::match_t* match );
	bool missed_due_to_trace( resolver_data::match_t* match );

public:
	void impact( const vec3& pos );
	void hurt( int target_id, int hitgroup, int damage );
	void shot( int target_id = 0, int hitgroup = 0, vec3 origin = vec3( ), vec3 target = vec3( ), c_base_entity* ent = nullptr, bool did_backtrack = false );

	void fsn_update( );

	__forceinline bool has_logged_shot( int i ) const { return m_data[i].valid_log; }

	vec3 resolver_angles[65]{ };
};

extern c_resolver g_resolver;