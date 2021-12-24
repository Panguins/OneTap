#include "context.h"
#include "netvar_compression.h"

c_netvar_compression g_netvar_compression;

void c_netvar_compression::start()
{
	if (!ctx.m_local || !ctx.m_tickrate)
		return;

	//m_punch.resize( m_tickrate );
	m_tickbase = ctx.m_local->tickbase();
	m_punch[ctx.m_local->tickbase() % 128] = ctx.m_local->punch();
	m_punch_velocity[ctx.m_local->tickbase() % 128] = ctx.m_local->punch_velocity();
}

void c_netvar_compression::finish()
{
	// storing viable information for other players is also possible :3

	if (!ctx.m_local || m_tickbase != ctx.m_local->tickbase())
		return;

	if (is_worthy(ctx.m_local->punch(), m_punch[ctx.m_local->tickbase() % 128]))
		ctx.m_local->punch() = m_punch[ctx.m_local->tickbase() % 128];

	if (is_worthy(ctx.m_local->punch_velocity(), m_punch_velocity[ctx.m_local->tickbase() % 128]))
		ctx.m_local->punch_velocity() = m_punch_velocity[ctx.m_local->tickbase() % 128];
}
