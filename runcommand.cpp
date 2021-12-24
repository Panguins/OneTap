#include "context.h"
#include "netvar_compression.h"

decltype( hooked::original::o_run_command ) hooked::original::o_run_command;

void __fastcall hooked::run_command(void* ecx, void* edx, c_base_entity* player, c_user_cmd* cmd, c_move_helper* movehelper)
{
	hooked::original::o_run_command(ecx, edx, player, cmd, movehelper);

	if (player == ctx.m_local)
		g_netvar_compression.finish();
}
