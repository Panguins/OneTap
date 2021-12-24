#pragma once

#pragma once
#include "lagcomp.h"
#include <deque>

// ugly ass legit lagcomp
// who gives a shit tho
// never has to be touched again

class c_lagcomp_legit
{
	struct legit_record_entry
	{
		int tickcount{ };
		vec3 aim_pos{ };
		matrix3x4_t bone_matrix[128];
	};

	struct legit_record
	{
		c_base_entity* m_entity{ };
		std::deque<legit_record_entry> m_data;

		void update( c_base_entity* entity, int tickcount );

		void reset( )
		{
			m_entity = nullptr;
			m_data.clear( );
		}
	};

	legit_record m_records[65]{ };

	void backtrack_target( c_base_entity* target, const vec3& viewangles );
	bool find_closest_target( c_base_entity** target, const vec3& viewangles );

	void update( );
	void set_tick( );
public:
	void init( );

	c_base_entity* current_target{ };
	matrix3x4_t current_matrix[128];
}; extern c_lagcomp_legit g_lagcomp_legit;
