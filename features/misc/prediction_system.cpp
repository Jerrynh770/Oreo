#include "prediction_system.h"

void engineprediction::store_netvars()
{
    auto data = &netvars_data[(m_clientstate()->iCommandAck - 1) % MULTIPLAYER_BACKUP];

    data->tickbase = g_ctx.local()->m_nTickBase();
    data->m_aimPunchAngle = g_ctx.local()->m_aimPunchAngle();
    data->m_aimPunchAngleVel = g_ctx.local()->m_aimPunchAngleVel();
    data->m_viewPunchAngle = g_ctx.local()->m_viewPunchAngle();
    data->m_vecViewOffset = g_ctx.local()->m_vecViewOffset();
    data->m_duckAmount = g_ctx.local()->m_flDuckAmount();
    data->m_duckSpeed = g_ctx.local()->m_flDuckSpeed();
}

void engineprediction::restore_netvars()
{
    int time;

    auto data = &netvars_data[time % MULTIPLAYER_BACKUP];


    if (data->tickbase != g_ctx.local()->m_nTickBase())
        return;

    auto aim_punch_angle_delta = data->m_aimPunchAngle - g_ctx.local()->m_aimPunchAngle();
    auto aim_punch_angle_vel_delta = data->m_aimPunchAngleVel - g_ctx.local()->m_aimPunchAngleVel();
    auto view_punch_angle_delta = data->m_viewPunchAngle - g_ctx.local()->m_viewPunchAngle();
    auto view_offset_delta = data->m_vecViewOffset - g_ctx.local()->m_vecViewOffset();
    auto velocity_delta = data->m_vecVelocity - g_ctx.local()->m_vecVelocity();

    auto thirdperson_recoil_delta = data->m_flThirdpersonRecoil - g_ctx.local()->m_flThirdpersonRecoil();
    auto duck_amount_delta = data->m_flDuckAmount - g_ctx.local()->m_flDuckAmount();
    auto fall_velocity_delta = data->m_flFallVelocity - g_ctx.local()->m_flFallVelocity();
    auto velocity_modifier_delta = data->m_flVelocityModifier - g_ctx.local()->m_flVelocityModifier();

    if (std::fabsf(aim_punch_angle_delta.x) <= 0.03125f && std::fabsf(aim_punch_angle_delta.y) <= 0.03125f && std::fabsf(aim_punch_angle_delta.z) <= 0.03125f)
        g_ctx.local()->m_aimPunchAngle() = data->m_aimPunchAngle;

    if (std::fabsf(aim_punch_angle_vel_delta.x) <= 0.03125f && std::fabsf(aim_punch_angle_vel_delta.y) <= 0.03125f && std::fabsf(aim_punch_angle_vel_delta.z) <= 0.03125f)
        g_ctx.local()->m_aimPunchAngleVel() = data->m_aimPunchAngleVel;

    if (std::fabsf(view_punch_angle_delta.x) <= 0.03125f && std::fabsf(view_punch_angle_delta.y) <= 0.03125f && std::fabsf(view_punch_angle_delta.z) <= 0.03125f)
        g_ctx.local()->m_viewPunchAngle() = data->m_viewPunchAngle;

    if (std::fabsf(view_offset_delta.x) <= 0.03125f && std::fabsf(view_offset_delta.y) <= 0.03125f && std::fabsf(view_offset_delta.z) <= 0.03125f)
        g_ctx.local()->m_vecViewOffset() = data->m_vecViewOffset;

    if (std::fabsf(velocity_delta.x) <= 0.03125f && std::fabsf(velocity_delta.y) <= 0.03125f && std::fabsf(velocity_delta.z) <= 0.03125f)
        g_ctx.local()->m_vecVelocity() = data->m_vecVelocity;

    if (std::fabs(thirdperson_recoil_delta) <= 0.03125f)
        g_ctx.local()->m_flThirdpersonRecoil() = data->m_flThirdpersonRecoil;

    if (std::fabsf(duck_amount_delta) <= 0.03125f)
        g_ctx.local()->m_flDuckAmount() = data->m_flDuckAmount;

    if (std::fabsf(fall_velocity_delta) <= 0.03125f)
        g_ctx.local()->m_flFallVelocity() = data->m_flFallVelocity;

    if (std::fabsf(velocity_modifier_delta) <= 0.00625f)
        g_ctx.local()->m_flVelocityModifier() = data->m_flVelocityModifier;
}


void engineprediction::setup()
{
    if (prediction_data.prediction_stage != SETUP)
        return;

    backup_data.flags = g_ctx.local()->m_fFlags();
    backup_data.velocity = g_ctx.local()->m_vecVelocity();

    prediction_data.old_curtime = m_globals()->m_curtime;
    prediction_data.old_frametime = m_globals()->m_frametime;

    m_prediction()->InPrediction = true;
    m_prediction()->IsFirstTimePredicted = false;

    m_globals()->m_curtime = TICKS_TO_TIME(g_ctx.globals.fixed_tickbase);
    m_globals()->m_frametime = m_prediction()->EnginePaused ? 0.0f : m_globals()->m_intervalpertick;

    prediction_data.prediction_stage = PREDICT;
}

void engineprediction::predict(CUserCmd* m_pcmd)
{
    if (prediction_data.prediction_stage != PREDICT)
        return;

    if (m_clientstate()->iDeltaTick > 0)
        m_prediction()->Update(m_clientstate()->iDeltaTick, true, m_clientstate()->nLastCommandAck, m_clientstate()->nLastCommandAck + m_clientstate()->iChokedCommands);

    if (!prediction_data.prediction_random_seed)
        prediction_data.prediction_random_seed = *reinterpret_cast <int**> (util::FindSignature(crypt_str("client.dll"), crypt_str("A3 ? ? ? ? 66 0F 6E 86")) + 0x1);

    *prediction_data.prediction_random_seed = MD5_PseudoRandom(m_pcmd->m_command_number) & INT_MAX;

    if (!prediction_data.prediction_player)
        prediction_data.prediction_player = *reinterpret_cast <int**> (util::FindSignature(crypt_str("client.dll"), crypt_str("89 35 ? ? ? ? F3 0F 10 48")) + 0x2);

    *prediction_data.prediction_player = reinterpret_cast <int> (g_ctx.local());

    m_gamemovement()->StartTrackPredictionErrors(g_ctx.local());
    m_movehelper()->set_host(g_ctx.local());

    CMoveData move_data;
    memset(&move_data, 0, sizeof(CMoveData));

    m_prediction()->SetupMove(g_ctx.local(), m_pcmd, m_movehelper(), &move_data);
    m_gamemovement()->ProcessMovement(g_ctx.local(), &move_data);
    m_prediction()->FinishMove(g_ctx.local(), m_pcmd, &move_data);

    m_gamemovement()->FinishTrackPredictionErrors(g_ctx.local());
    m_movehelper()->set_host(nullptr);

    auto viewmodel = g_ctx.local()->m_hViewModel().Get();

    if (viewmodel)
    {
        viewmodel_data.weapon = viewmodel->m_hWeapon().Get();

        viewmodel_data.viewmodel_index = viewmodel->m_nViewModelIndex();
        viewmodel_data.sequence = viewmodel->m_nSequence();
        viewmodel_data.animation_parity = viewmodel->m_nAnimationParity();

        viewmodel_data.cycle = viewmodel->m_flCycle();
        viewmodel_data.animation_time = viewmodel->m_flAnimTime();
    }

    prediction_data.prediction_stage = FINISH;
}

void engineprediction::finish()
{
    if (prediction_data.prediction_stage != FINISH)
        return;

    m_gamemovement()->FinishTrackPredictionErrors(g_ctx.local());
    m_movehelper()->set_host(nullptr);

    *prediction_data.prediction_random_seed = -1;
    *prediction_data.prediction_player = 0;

    *prediction_data.prediction_random_seed = -1;
    *prediction_data.prediction_player = 0;

    m_globals()->m_curtime = prediction_data.old_curtime;
    m_globals()->m_frametime = prediction_data.old_frametime;
}