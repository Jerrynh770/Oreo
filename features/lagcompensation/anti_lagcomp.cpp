#include "anti_lagcomp.h"
#include "../../sdk/interfaces/IClientEntity.hpp"

void anti_lagcomp::createmove(ClientFrameStage_t stage)
{
	this->antiexploit();
}

void anti_lagcomp::antiexploit()
{
	static auto is_command_set = false;
	if (is_command_set != g_cfg.ragebot.anti_defensive)
	{
		auto old_team = g_ctx.local()->m_iTeamNum();

		std::string command = "kill; jointeam 1; cl_lagcompensation ";
		command += std::to_string(g_cfg.ragebot.anti_defensive ? 0 : 1) + ";";
		command += "jointeam " + std::to_string(old_team) + "; ";

		m_engine()->ExecuteClientCmd(command.data());
		is_command_set = g_cfg.ragebot.anti_defensive;
	}
}