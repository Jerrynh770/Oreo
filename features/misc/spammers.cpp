#include "spammers.h"

void spammers::clan_tag()
{
    auto apply = [](const char* tag) -> void
    {
        using Fn = int(__fastcall*)(const char*, const char*);
        static auto fn = reinterpret_cast<Fn>(util::FindSignature(crypt_str("engine.dll"), crypt_str("53 56 57 8B DA 8B F9 FF 15")));

        fn(tag, tag);
    };

    static auto removed = false;

    if (!g_cfg.misc.clantag_spammer && !removed)
    {
        removed = true;
        apply(crypt_str(""));
        return;
    }

    if (g_cfg.misc.clantag_spammer)
    {
        auto nci = m_engine()->GetNetChannelInfo();

        if (!nci)
            return;

        static auto time = 1;

        auto ticks = TIME_TO_TICKS(nci->GetAvgLatency(FLOW_OUTGOING)) + (float)m_globals()->m_tickcount;
        auto intervals = 0.3f / m_globals()->m_intervalpertick;

        auto main_time = (int)(ticks / intervals) % 8;

        if (main_time != time && !m_clientstate()->iChokedCommands)
        {
            auto tag = crypt_str("");

            switch (main_time)
            {
            case 0:
                tag = (crypt_str("[____]"));
                break;
            case 1:
                tag = (crypt_str("[O___]"));
                break;
            case 2:
                tag = (crypt_str("[Or__]"));
                break;
            case 3:
                tag = (crypt_str("[Ore_]"));
                break;
            case 4:
                tag = (crypt_str("[Oreo]"));
                break;
            case 5:
                tag = (crypt_str("[_Ore]"));
                break;
            case 6:
                tag = (crypt_str("[__Or]"));
                break;
            case 7:
                tag = (crypt_str("[___O]"));
                break;
            }

            apply(tag);
            time = main_time;
        }

        removed = false;
    }
}