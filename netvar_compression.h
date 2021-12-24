#pragma once
#include <array>

class c_netvar_compression
{
public:
	void start();
	void finish();

	inline bool is_worthy(const vec3& current, const vec3& stored) const
	{
		// WARNING!!! HOMO CODE!!!
		return ((std::abs(current.x - stored.x) <= m_dist_epsilon) && (std::abs(current.x - stored.x) <= m_dist_epsilon) && (std::abs(current.x - stored.x) <= m_dist_epsilon));
	}

	inline bool is_worthy(const float& current, const float& stored) const
	{
		// WARNING!!! HOMO CODE!!!
		return std::abs(current - stored) <= m_dist_epsilon;
	}

private:
	// WARNING!!! HOMO CODE!!!
	int m_tickbase;
	std::array<vec3, 128> m_punch;
	std::array<vec3, 128> m_punch_velocity;

	static constexpr float m_dist_epsilon = 0.03125f;
};

extern c_netvar_compression g_netvar_compression;
