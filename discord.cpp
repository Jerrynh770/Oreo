#include "discord.h"
#include "../../disrpc/Includes/discord_rpc.h"

void Discord::Initialize()
{
    DiscordEventHandlers Handle;
    memset(&Handle, 0, sizeof(Handle));
    Discord_Initialize("", &Handle, 1, NULL);
}

void Discord::Update()
{
    DiscordRichPresence discord;
    memset(&discord, 0, sizeof(discord));
    discord.details = "Playing CS:GO";
    discord.state = "With Oreo";
    discord.largeImageKey = "image2";
    discord.smallImageKey = "image1";
    Discord_UpdatePresence(&discord);
}