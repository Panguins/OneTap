#pragma once

class c_prediction
{
public:
	void            start( );

	CMoveData       m_move_data;

	int             m_old_buttons;
	uint8_t         m_old_move_type;
	int             m_old_flags;
	int             m_old_tickbase;
	c_global_vars   m_old_globals;
	vec3            m_old_velocity;
	float           m_old_fall_velocity;
	vec3            m_old_base_velocity;

}; extern c_prediction g_prediction;