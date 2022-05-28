
#include "animation_system.h"
#include "..\ragebot\aim.h"
void resolver::initialize(player_t* e, adjust_data* record, const float& goal_feet_yaw, const float& pitch)
{
	player = e;
	player_record = record;

	original_goal_feet_yaw = math::normalize_yaw(goal_feet_yaw);
	original_pitch = math::normalize_pitch(pitch);
}

Vector player_t::get_eye_pos() {
	return m_vecOrigin() + m_vecViewOffset();
}

bool resolver::DesyncDetect()
{
	player_t* player = nullptr;
	if (!player->is_alive())
		return false;
	if (player->get_max_desync_delta() < 10)
		return false;
	if (!player->m_iTeamNum() == g_ctx.local()->m_iTeamNum())
		return false;
	if (player->get_move_type() == MOVETYPE_NOCLIP || player->get_move_type() == MOVETYPE_LADDER)
		return false;

	return true;
}

bool resolver::update_walk_data()
{
	auto e = player;


	auto anim_layers = player_record->layers;
	bool s_1 = false,
		s_2 = false,
		s_3 = false;

	for (int i = 0; i < e->animlayer_count(); i++)
	{
		anim_layers[i] = e->get_animlayers()[i];
		if (anim_layers[i].m_nSequence == 26 && anim_layers[i].m_flWeight < 0.47f)
			s_1 = true;
		if (anim_layers[i].m_nSequence == 7 && anim_layers[i].m_flWeight > 0.001f)
			s_2 = true;
		if (anim_layers[i].m_nSequence == 2 && anim_layers[i].m_flWeight == 0)
			s_3 = true;
	}
	bool  m_fakewalking;
	if (s_1 && s_2)
		if (s_3)
			m_fakewalking = true;
		else
			m_fakewalking = false;
	else
		m_fakewalking = false;

	return m_fakewalking;
}

bool resolver::is_slow_walking()
{
	auto entity = player;
	//float large = 0;
	float velocity_2D[64], old_velocity_2D[64];
	if (entity->m_vecVelocity().Length2D() != velocity_2D[entity->EntIndex()] && entity->m_vecVelocity().Length2D() != NULL) {
		old_velocity_2D[entity->EntIndex()] = velocity_2D[entity->EntIndex()];
		velocity_2D[entity->EntIndex()] = entity->m_vecVelocity().Length2D();
	}

	Vector velocity = entity->m_vecVelocity();
	Vector direction = entity->m_angEyeAngles();

	float speed = velocity.Length();
	direction.y = entity->m_angEyeAngles().y - direction.y;

	if (velocity_2D[entity->EntIndex()] > 1) {
		int tick_counter[64];
		if (velocity_2D[entity->EntIndex()] == old_velocity_2D[entity->EntIndex()])
			tick_counter[entity->EntIndex()] += 1;
		else
			tick_counter[entity->EntIndex()] = 0;

		while (tick_counter[entity->EntIndex()] > (1 / m_globals()->m_intervalpertick) * fabsf(0.1f))//should give use 100ms in ticks if their speed stays the same for that long they are definetely up to something..
			return true;
	}


	return false;
}

bool resolver::IsAdjustingBalance()
{


	for (int i = 0; i < 15; i++)
	{
		const int activity = player->sequence_activity(player_record->layers[i].m_nSequence);
		if (activity == 979)
		{
			return true;
		}
	}
	return false;
}

bool resolver::is_breaking_lby(AnimationLayer cur_layer, AnimationLayer prev_layer)
{
	if (IsAdjustingBalance())
	{
		if (IsAdjustingBalance())
		{
			if ((prev_layer.m_flCycle != cur_layer.m_flCycle) || cur_layer.m_flWeight == 1.f)
			{
				return true;
			}
			else if (cur_layer.m_flWeight == 0.f && (prev_layer.m_flCycle > 0.92f && cur_layer.m_flCycle > 0.92f))
			{
				return true;
			}
		}
		return false;
	}

	return false;
}

void resolver::reset()
{
	player = nullptr;
	player_record = nullptr;

	side = false;

	was_first_bruteforce = false;
	was_second_bruteforce = false;

	original_goal_feet_yaw = 0.0f;
	original_pitch = 0.0f;
}

void resolver::setmode()
{
	auto e = player;

	float speed = e->m_vecVelocity().Length();


	auto cur_layer = player_record->layers;
	auto prev_layer = player_record->layers;

	bool on_ground = e->m_fFlags() & FL_ONGROUND && !e->get_animation_state()->m_bInHitGroundAnimation;

	bool slow_walking1 = is_slow_walking();
	bool slow_walking2 = update_walk_data();

	bool flicked_lby = abs(player_record->layers[3].m_flWeight - player_record->layers[7].m_flWeight) >= 1.1f;
	bool breaking_lby = is_breaking_lby(cur_layer[3], prev_layer[3]);


	bool ducking = player->get_animation_state()->m_fDuckAmount && e->m_fFlags() & FL_ONGROUND && !player->get_animation_state()->m_bInHitGroundAnimation;



	bool stand_anim = false;
	if (player_record->layers[3].m_flWeight == 0.f && player_record->layers[3].m_flCycle == 0.f && player_record->layers[6].m_flWeight == 0.f)
		stand_anim = true;

	bool move_anim = false;
	if (int(player_record->layers[6].m_flWeight * 1000.f) == int(previous_layers[6].m_flWeight * 1000.f))
		move_anim = true;

	auto animstate = player->get_animation_state();
	if (!animstate)
		return;

	auto valid_move = true;
	if (animstate->m_velocity > 0.1f || fabs(animstate->flUpVelocity) > 100.f)
		valid_move = animstate->m_flTimeSinceStartedMoving < 0.22f;


	if (!on_ground)
	{
		player_record->curMode = AIR;
	}
	else if (valid_move && stand_anim || speed < 3.1f && ducking || speed < 1.2f && !ducking || breaking_lby)
	{
		player_record->curMode = STANDING;

	}
	else if (!valid_move && move_anim || speed >= 3.1f && ducking || speed >= 1.2f && !ducking)
	{
		if ((speed >= 1.2f && speed < 134.f) && !ducking && (slow_walking1 || slow_walking2))
			player_record->curMode = SLOW_WALKING;
		else
			player_record->curMode = MOVING;
	}
	else
		player_record->curMode = FREESTANDING;
}

bool resolver::low_delta()
{
	auto record = player_record;
	if (!g_ctx.local()->is_alive())
		return false;
	float angle_diff = math::angle_diff(player->m_angEyeAngles().y, player->get_animation_state()->m_flGoalFeetYaw);
	Vector first = ZERO, second = ZERO, third = ZERO;
	first = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y + min(angle_diff, 35), player->hitbox_position(HITBOX_HEAD).z);
	second = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y, player->hitbox_position(HITBOX_HEAD).z);
	third = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y - min(angle_diff, 35), player->hitbox_position(HITBOX_HEAD).z);
	Ray_t one, two, three;
	trace_t tone, ttwo, ttree;
	CTraceFilter fl;
	fl.pSkip = player;
	one.Init(g_ctx.local()->get_shoot_position(), first);
	two.Init(g_ctx.local()->get_shoot_position(), second);
	three.Init(g_ctx.local()->get_shoot_position(), third);
	m_trace()->TraceRay(one, MASK_PLAYERSOLID, &fl, &tone);
	m_trace()->TraceRay(two, MASK_PLAYERSOLID, &fl, &ttwo);
	m_trace()->TraceRay(three, MASK_PLAYERSOLID, &fl, &ttree);
	if (!tone.allsolid && !ttwo.allsolid && !ttree.allsolid && tone.fraction < 0.97 && ttwo.fraction < 0.97 && ttree.fraction < 0.97)
		return true;

	float lby = fabs(math::normalize_yaw(player->m_flLowerBodyYawTarget()));
	if (lby < 35 && lby > -35)
		return true;
	return false;
}

bool resolver::low_delta2()
{
	auto record = player_record;
	if (!g_ctx.local()->is_alive())
		return false;
	float angle_diff = math::angle_diff(player->m_angEyeAngles().y, player->get_animation_state()->m_flGoalFeetYaw);
	Vector first = ZERO, second = ZERO, third = ZERO;
	first = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y + min(angle_diff, 20), player->hitbox_position(HITBOX_HEAD).z);
	second = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y, player->hitbox_position(HITBOX_HEAD).z);
	third = Vector(player->hitbox_position(HITBOX_HEAD).x, player->hitbox_position(HITBOX_HEAD).y - min(angle_diff, 20), player->hitbox_position(HITBOX_HEAD).z);
	Ray_t one, two, three;
	trace_t tone, ttwo, ttree;
	CTraceFilter fl;
	fl.pSkip = player;
	one.Init(g_ctx.local()->get_shoot_position(), first);
	two.Init(g_ctx.local()->get_shoot_position(), second);
	three.Init(g_ctx.local()->get_shoot_position(), third);
	m_trace()->TraceRay(one, MASK_PLAYERSOLID, &fl, &tone);
	m_trace()->TraceRay(two, MASK_PLAYERSOLID, &fl, &ttwo);
	m_trace()->TraceRay(three, MASK_PLAYERSOLID, &fl, &ttree);
	if (!tone.allsolid && !ttwo.allsolid && !ttree.allsolid && tone.fraction < 0.97 && ttwo.fraction < 0.97 && ttree.fraction < 0.97)
		return true;

	float lby = fabs(math::normalize_yaw(player->m_flLowerBodyYawTarget()));
	if (lby < 20 && lby > -20)
		return true;
	return false;
}

float resolver::GetBackwardYaw(player_t* ent) {
	return math::calculate_angle(g_ctx.local()->GetAbsOrigin(), ent->GetAbsOrigin()).y;
}

float resolver::GetForwardYaw(player_t* ent) {
	return math::normalize_yaw(GetBackwardYaw(ent) - 180.f);
}

void resolver::resolve_yaw()
{

	if (!DesyncDetect())
	{
		player_record->side = RESOLVER_ORIGINAL;
		player_record->curMode = NO_MODE;
		player_record->curSide = NO_SIDE;
		return;
	}

	player_info_t player_info;

	if (!m_engine()->GetPlayerInfo(player->EntIndex(), &player_info))
		return;

	static int side[63];
	auto animstate = player->get_animation_state();
	auto speed = g_ctx.local()->m_vecVelocity().Length2D();
	int m_Side;
	if (speed <= 0.1f)
	{
		if (player_record->layers[3].m_flWeight == 0.0 && player_record->layers[3].m_flCycle == 0.0)
		{
			side[player->EntIndex()] = 2 * (math::normalize_diff(player->m_angEyeAngles().y, player_record->abs_angles.y) <= 0.0) - 1;
		}
	}
	else
	{
		const float f_delta = abs(player_record->layers[6].m_flPlaybackRate - player_record->left_layers[6].m_flPlaybackRate);
		const float s_delta = abs(player_record->layers[6].m_flPlaybackRate - player_record->center_layers[6].m_flPlaybackRate);
		const float t_delta = abs(player_record->layers[6].m_flPlaybackRate - player_record->right_layers[6].m_flPlaybackRate);

		if (f_delta < s_delta || t_delta <= s_delta || (s_delta * 1000.0))
		{
			if (f_delta >= t_delta && s_delta > t_delta && !(t_delta * 1000.0))
			{
				side[player->EntIndex()] = 1;
			}
		}
		else
		{
			side[player->EntIndex()] = -1;
		}
	}

	setmode();
	float m_flLastShotTime;
	auto simtime = player->m_flSimulationTime();
	bool m_shot;

	if (player_record->curMode == AIR)
	{
		player_record->side = RESOLVER_ORIGINAL;
		player_record->curMode = AIR;
		player_record->curSide = NO_SIDE;
		return;
	}

	if (player->GetAbsOrigin().y == GetForwardYaw(player))
		side[player->EntIndex()] *= -1;

	if (g_ctx.globals.missed_shots[player->EntIndex()] == 0) {
		if (side[player->EntIndex()] >= 1) {
			//right
			player_record->side = low_delta() ? RESOLVER_LOW_FIRST : RESOLVER_FIRST;
		}
		else if (side[player->EntIndex()] <= -1) {
			//left
			player_record->side = low_delta() ? RESOLVER_LOW_SECOND : RESOLVER_SECOND;
		}
	}
	else if (g_ctx.globals.missed_shots[player->EntIndex()] == 1) {
		if (side[player->EntIndex()] >= 1) {
			//right
			player_record->side = low_delta2() ? RESOLVER_LOW_FIRST1 : RESOLVER_FIRST;
		}
		else if (side[player->EntIndex()] <= -1) {
			//left
			player_record->side = low_delta2() ? RESOLVER_LOW_SECOND1 : RESOLVER_SECOND;
		}
	}
	else if (g_ctx.globals.missed_shots[player->EntIndex()] == 2) {
		player_record->side = RESOLVER_SECOND;
	}
	else if (g_ctx.globals.missed_shots[player->EntIndex()] == 3) {
		player_record->side = RESOLVER_FIRST;
	}
	else if (g_ctx.globals.missed_shots[player->EntIndex()] == 4) {
		player_record->side = RESOLVER_LOW_FIRST;
	}
	else if (g_ctx.globals.missed_shots[player->EntIndex()] == 5) {
		player_record->side = RESOLVER_LOW_SECOND;
	}
	else if (g_ctx.globals.missed_shots[player->EntIndex()] >= 6) {
		m_cvar()->ConsoleColorPrintf(Color(255, 0, 0), u8"");
		g_ctx.globals.missed_shots[player->EntIndex()] = 0;
	}

	if (g_ctx.globals.missed_shots[player->EntIndex()] > 0) {

		switch (math::is_near_equal(animstate->m_flGoalFeetYaw, player->m_angEyeAngles().y, 60.f)) {
		case 1:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 60);
			animstate->m_flGoalFeetYaw += 60.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 63);
			animstate->m_flGoalFeetYaw += 63.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 65);
			animstate->m_flGoalFeetYaw += 65.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 67);
			animstate->m_flGoalFeetYaw += 67.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 69);
			animstate->m_flGoalFeetYaw += 69.f;
			break;
		case 2:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(60.0f, 69.0f));
			break;
		}

		switch (math::is_near_equal(animstate->m_flGoalFeetYaw, player->m_angEyeAngles().y, -60.f)) {
		case 1:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 60);
			animstate->m_flGoalFeetYaw -= 60.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 63);
			animstate->m_flGoalFeetYaw -= 63.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 65);
			animstate->m_flGoalFeetYaw -= 65.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 67);
			animstate->m_flGoalFeetYaw -= 67.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 69);
			animstate->m_flGoalFeetYaw -= 69.f;
			break;
		case 2:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(-69.0f, -60.0f));
			break;
		}

		switch (math::is_near_equal(animstate->m_flGoalFeetYaw, player->m_angEyeAngles().y, 90.f)) {
		case 1:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 90);
			animstate->m_flGoalFeetYaw += 90.f;
			break;
		case 2:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(0.0f, 90.0f));
			break;
		}

		switch (math::is_near_equal(animstate->m_flGoalFeetYaw, player->m_angEyeAngles().y, -90.f)) {
		case 1:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 90);
			animstate->m_flGoalFeetYaw -= 90.f;
			break;
		case 2:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(-90.0f, 0.0f));
			break;
		}

		switch (math::is_near_equal(animstate->m_flGoalFeetYaw, player->m_angEyeAngles().y, 50.f)) {
		case 1:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 50);
			animstate->m_flGoalFeetYaw += 50.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 53);
			animstate->m_flGoalFeetYaw += 53.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 57);
			animstate->m_flGoalFeetYaw += 57.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 58);
			animstate->m_flGoalFeetYaw += 58.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + 59);
			animstate->m_flGoalFeetYaw += 59.f;
			break;
		case 2:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(50.0f, 59.0f));
			break;
		}

		switch (math::is_near_equal(animstate->m_flGoalFeetYaw, player->m_angEyeAngles().y, -50.f)) {
		case 1:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 50);
			animstate->m_flGoalFeetYaw -= 50.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 53);
			animstate->m_flGoalFeetYaw -= 53.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 57);
			animstate->m_flGoalFeetYaw -= 57.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 58);
			animstate->m_flGoalFeetYaw -= 58.f;
			break;
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y - 59);
			animstate->m_flGoalFeetYaw -= 59.f;
			break;

		case 2:
			animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(-59.0f, -50.0f));
			break;
		}

		if (animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y < 50.f)) {
			switch (player->is_alive()) {
			case 1:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(40.f, 49.f));
				break;
			case 2:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(-40.f, -49.f));
				break;
			case 3:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(30.f, 39.f));
				break;
			case 4:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(-20.f, -29.f));
				break;
			case 5:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(20.f, 29.f));
				break;
			case 6:
				animstate->m_flGoalFeetYaw = math::normalize_yaw(player->m_angEyeAngles().y + math::random_float(-10.f, -19.f));
				break;
			}
		}
	}

	float flRawYawIdeal = (math::angle_diff(-player->m_vecAbsVelocity().y, -player->m_vecAbsVelocity().x) * 180 / M_PI);
	if (flRawYawIdeal < 0)
		flRawYawIdeal += 360;
}

float resolver::resolve_pitch()
{
	float liohsdafg = 0.f;
	if (liohsdafg < -179.f)
		liohsdafg += 360.f;
	else if (liohsdafg > 90.0 || liohsdafg < -90.0) liohsdafg = 89.f;
	else if (liohsdafg > 89.0 && liohsdafg < 91.0) liohsdafg -= 90.f;
	else if (liohsdafg > 179.0 && liohsdafg < 181.0) liohsdafg -= 180;
	else if (liohsdafg > -179.0 && liohsdafg < -181.0) liohsdafg += 180;

	else if (fabs(liohsdafg) == 0) liohsdafg = copysign(89.0f, liohsdafg);
	else liohsdafg = original_pitch;

	return liohsdafg;
}
