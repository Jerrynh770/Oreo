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

        auto main_time = (int)(ticks / intervals) % 50;

        if (main_time != time && !m_clientstate()->iChokedCommands)
        {
            auto tag = crypt_str("");

            switch (main_time)
            {
            case 0:
                tag = ("[_______] ");
                break;
            case 1:
                tag = ("[______d] ");
                break;
            case 2:
                tag = ("[_____d_] ");
                break;
            case 3:
                tag = ("[____d__] ");
                break;
            case 4:
                tag = ("[___d___] ");
                break;
            case 5:
                tag = ("[__d____] ");
                break;
            case 6:
                tag = ("[_d_____] ");
                break;
            case 7:
                tag = ("[d______] ");
                break;
            case 8:
                tag = ("[d______] ");
                break;
            case 9:
                tag = ("[d_____e] ");
                break;
            case 10:
                tag = ("[d____e_] ");
                break;
            case 11:
                tag = ("[d___e__] ");
                break;
            case 12:
                tag = ("[d__e___] ");
                break;
            case 13:
                tag = ("[d_e____] ");
                break;
            case 14:
                tag = ("[de_____] ");
                break;
            case 15:
                tag = ("[de_____] ");
                break;
            case 16:
                tag = ("[de____v] ");
                break;
            case 17:
                tag = ("[de___v_] ");
                break;
            case 18:
                tag = ("[de__v__] ");
                break;
            case 19:
                tag = ("[de_v___] ");
                break;
            case 20:
                tag = ("[dev____] ");
                break;
            case 21:
                tag = ("[dev____] ");
                break;
            case 22:
                tag = ("[dev___i] ");
                break;
            case 23:
                tag = ("[dev__i_] ");
                break;
            case 24:
                tag = ("[dev_i__] ");
                break;
            case 25:
                tag = ("[devi___] ");
                break;
            case 26:
                tag = ("[devi___] ");
                break;
            case 27:
                tag = ("[devi__l] ");
                break;
            case 28:
                tag = ("[devi_l_] ");
                break;
            case 29:
                tag = ("[devil__] ");
                break;
            case 30:
                tag = ("[devil_r] ");
                break;
            case 31:
                tag = ("[devilr_] ");
                break;
            case 32:
                tag = ("[devilry] ");
                break;
            case 33:
                tag = ("[9evilry] ");
                break;
            case 34:
                tag = ("[d4vilry] ");
                break;
            case 35:
                tag = ("[de1ilry] ");
                break;
            case 36:
                tag = ("[dev2lry] ");
                break;
            case 37:
                tag = ("[de1i1ry] ");
                break;
            case 38:
                tag = ("[de1il7y] ");
                break;
            case 39:
                tag = ("[devilry] ");
                break;
            case 40:
                tag = ("[devilry] ");
                break;
            case 41:
                tag = ("[devilry] ");
                break;
            case 42:
                tag = ("[evilry_] ");
                break;
            case 43:
                tag = ("[vilry__] ");
                break;
            case 44:
                tag = ("[ilry___] ");
                break;
            case 45:
                tag = ("[lry____] ");
                break;
            case 46:
                tag = ("[ry_____] ");
                break;
            case 47:
                tag = ("[y______] ");
                break;
            case 48:
                tag = ("[y______] ");
                break;
            case 49:
                tag = ("[_______] ");
                break;
            }

            apply(tag);
            time = main_time;
        }

        removed = false;
    }
}