#pragma once

enum antiaim_pitch_t
{
	emotion = 1,
	down,
	fakedown,
	fakeup,
	zero,
	fake_nigga,
};

enum antiaim_yaw_t
{
	backwards = 1,
	spin,
	sideways,
	jitter,
	svitch,
};

class c_anti_aim
{
	bool is_safe_angle( float yaw );
	int get_freestand_yaw( float* yaw );

	float get_ideal_pitch( );
	float get_ideal_yaw( );

	void handle_lby( );

	bool m_is_freestanding;
public:
	void init( );

	bool should_run( );

public:
	// i'd make this all constexpr if you'd allow me to add some libs like openstd/ some random math lib
	// CBasePlayerAnimState::ComputePoseParam_BodyPitch
	static inline float calculate_real_angle( float viewangle )
	{
		// purpose:
		// show real pitch angle in thirdperson
		// via recreating how server sees these magic number angles

		if ( viewangle > 180.0f )
			viewangle -= 360.0f;

		return std::clamp( viewangle, -90.0f, 90.0f );
	}

	static inline bool is_fake_angle( const float& viewangle, const float& goal_fake )
	{
		float eye_angles = std::abs( math::normalize( viewangle ) );
		float server = calculate_real_angle( viewangle );

		if ( eye_angles == goal_fake && server != eye_angles )
			return true;

		return false;
	}

	static inline bool calculate_smallest_fake_angle( const float& goal, float& out )
	{
		for ( float angle = 270.0f; angle < 1000.0f; angle += 1.0f )
		{
			if ( is_fake_angle( angle, std::abs( goal ) ) )
			{
				if ( goal < 0.0f )
					out = -angle;
				else
					out = angle;

				return true;
			}
		}

		return false;
	}
}; extern c_anti_aim g_antiaim;