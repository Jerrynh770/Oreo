#pragma once

#include "..\..\includes.hpp"
#include "..\..\sdk\structs.hpp"
enum Prediction_stage
{
    SETUP,
    PREDICT,
    FINISH
};

class engineprediction : public singleton <engineprediction>
{
    struct Netvars_data
    {
        int tickbase = INT_MIN;

        Vector m_velocity = Vector(0, 0, 0);
        Vector m_aimPunchAngle = Vector(0, 0, 0);
        Vector m_aimPunchAngleVel = Vector(0, 0, 0);
        Vector m_viewPunchAngle = Vector(0, 0, 0);
        Vector m_vecViewOffset = Vector(0, 0, 0);
        bool ground = false;

        float m_fall_velocity = 0.f;
        float m_velocity_modifier = 0.f;
        float walking = 0.f;
        float flag = 0.f;
        float layer = 0.f;
        float index = 0.f;
        float m_duckAmount = 0.f;
        float m_duckSpeed = 0.f;
    };

    struct Backup_data
    {
        int flags = 0;
        Vector velocity = Vector(0, 0, 0);
    };

    struct Prediction_data
    {
        void reset()
        {
            prediction_stage = SETUP;
            old_curtime = 0.0f;
            old_frametime = 0.0f;
        }

        Prediction_stage prediction_stage = SETUP;
        float old_curtime = 0.0f;
        float old_frametime = 0.0f;
        float old_tickcount = 0.0f;
        int* prediction_random_seed = nullptr;
        int* prediction_player = nullptr;
    };

    struct Viewmodel_data
    {
        weapon_t* weapon = nullptr;

        int viewmodel_index = 0;
        int sequence = 0;
        int animation_parity = 0;

        float cycle = 0.0f;
        float animation_time = 0.0f;
    };
public:
    Netvars_data netvars_data[MULTIPLAYER_BACKUP];
    Backup_data backup_data;
    Prediction_data prediction_data;
    Viewmodel_data viewmodel_data;
    float m_flSpread;
    float m_flInaccuracy;
    float GetSpread();
    float GetInaccuracy();
    void StartCommand(player_t* player, CUserCmd* cmd);
    int GetTickbase(CUserCmd* pCmd, ctx_t* pLocal);
    void store_netvars();
    void restore_netvars();
    void setup();
    void predict(CUserCmd* m_pcmd);
    void finish();
};