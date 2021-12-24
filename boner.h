#pragma once
#include <mutex>

class c_boner
{
	//400kb worth of floats, big boner
	bool valid_bone_matrix[65]{ };
	matrix3x4_t latest_bone_matrix[65][128];
	std::mutex mutex[65];
public:
	void update( );
	void update_local( );

	bool get_latest( int i, matrix3x4_t* matrix )
	{
		if ( !valid_bone_matrix[i] )
			return false;

		mutex[i].lock( );
		memcpy( matrix, latest_bone_matrix[i], sizeof( matrix3x4_t ) * 128 );
		mutex[i].unlock( );

		return valid_bone_matrix[i];
	}
}; extern c_boner g_boner;