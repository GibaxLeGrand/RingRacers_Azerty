// DR. ROBOTNIK'S RING RACERS
//-----------------------------------------------------------------------------
// Copyright (C) 2022 by Sally "TehRealSalt" Cochenour
// Copyright (C) 2022 by Kart Krew
//
// This program is free software distributed under the
// terms of the GNU General Public License, version 2.
// See the 'LICENSE' file for more details.
//-----------------------------------------------------------------------------
/// \file  cloud.c
/// \brief Launcher clouds and tulips used for Aerial Highlands, Avant Garden, and Sky Sanctuary.

#include "../p_local.h"
#include "../k_objects.h"
#include "../g_game.h"
#include "../info.h"
#include "../s_sound.h"
#include "../r_main.h"
#include "../m_random.h"


#define BULB_ZTHRUST 96*FRACUNIT
#define CLOUD_ZTHRUST 32*FRACUNIT
#define CLOUDB_ZTHRUST 16*FRACUNIT

void Obj_CloudSpawn(mobj_t *mobj)
{
	mobj->destscale = mapobjectscale * 4;
	P_SetScale(mobj, mobj->destscale);
}

void Obj_CloudClusterThink(mobj_t *mobj, mobjtype_t cloudtype)
{
	if (mobj->extravalue1)
		return;

	mobj_t *cloud = P_SpawnMobj(mobj->x, mobj->y, mobj->z, cloudtype);
	angle_t ang = mobj->angle;
	UINT8 dist = 128;

	if (cloudtype == MT_AGZ_CLOUD)
		cloud->scale *= 2;

	for (UINT8 i = 0; i < 4; i++)
	{
		fixed_t x = mobj->x + FixedMul(mapobjectscale, dist * FINECOSINE(ang >> ANGLETOFINESHIFT));
		fixed_t y = mobj->y + FixedMul(mapobjectscale, dist * FINESINE(ang >> ANGLETOFINESHIFT));

		cloud = P_SpawnMobj(x, y, mobj->z, cloudtype);

		if (cloudtype == MT_AGZ_CLOUD)
		{
			cloud->scale *= 2;
			cloud->frame = P_RandomRange(PR_DECORATION, 0, 3);
		}

		ang += ANGLE_90;
	}

	mobj->extravalue1 = 1;
}

void Obj_TulipSpawnerThink(mobj_t *mobj)
{
	if (!mobj->tracer)
	{
		mobj->hnext = P_SpawnMobj(0, 0, 0, MT_AGZ_BULB_PART);
		mobj->hnext->hnext = P_SpawnMobj(0, 0, 0, MT_AGZ_BULB_PART);

		P_SetMobjState(mobj->hnext, S_AGZBULB_BASE);
		P_SetMobjState(mobj->hnext->hnext, S_AGZBULB_BASE);

		mobj->tracer = P_SpawnMobj(0, 0, 0, MT_AGZ_BULB_PART);
		P_SetMobjState(mobj->tracer, S_AGZBULB_NEUTRAL);
	}

	angle_t a = mobj->angle + ANG1*45;
	mobj_t *part = mobj->hnext;

	while (part)
	{
		P_MoveOrigin(part, mobj->x, mobj->y, mobj->z);
		part->angle = a;
		part->scale = mobj->scale;
		part->flags2 = mobj->flags2;
		part->eflags = mobj->eflags;
		a += ANG1*90;
		part = part->hnext;
	}

	mobj_t *b = mobj->tracer;

	P_MoveOrigin(b, mobj->x, mobj->y, mobj->z);
	b->scale = mobj->scale;
	b->flags2 = mobj->flags2;
	b->eflags = mobj->eflags;
	b->color = SKINCOLOR_MAGENTA;

	if (b->state == S_AGZBULB_ANIM2)
	{
		if (leveltime & 1)
			b->colorized = true;
		else
			b->colorized = false;
	}
	else
		b->colorized = false;
}

void Obj_PlayerCloudThink(player_t *player)
{
	mobj_t *mo = player->mo;

	if (player->cloudbuf)
		player->cloudbuf--;

	if (player->cloudlaunch)
	{
		player->cloudlaunch--;

		if (leveltime % 6 == 0)
			P_SpawnMobj(mo->x + P_RandomRange(PR_DECORATION, -8, 8)*mapobjectscale, mo->y + P_RandomRange(PR_DECORATION, -8, 8)*mapobjectscale, mo->z, MT_DRIFTDUST);
	}

	if (player->cloud)
	{
		player->cloud--;
		P_InstaThrust(mo, 0, 0);
		mo->momz = 0;

		if (!player->cloud)
		{
			if (!mo->tracer)
				return;

			switch(mo->tracer->type)
			{
				case MT_AHZ_CLOUD:
					P_SetObjectMomZ(mo, CLOUD_ZTHRUST, false);
					break;
				case MT_AGZ_CLOUD:
					mo->momz = FixedMul(mapobjectscale, CLOUD_ZTHRUST * P_MobjFlip(mo->tracer));
					break;
				case MT_SSZ_CLOUD:
					P_SetObjectMomZ(mo, CLOUDB_ZTHRUST, false);
					break;
				default:
					break;
			}
			P_SetObjectMomZ(mo, CLOUD_ZTHRUST, false);
			player->cloudlaunch = TICRATE;

			P_InstaThrust(mo, mo->cusval, mo->cvmem);
		}
	}
}

void Obj_PlayerBulbThink(player_t *player)
{
	mobj_t *mo = player->mo;

	if (player->tulipbuf)
		player->tulipbuf--;

	if (player->tuliplaunch)
	{
		player->tuliplaunch--;

		if (leveltime % 2 == 0)
			P_SpawnMobj(mo->x + P_RandomRange(PR_DECORATION, -8, 8)*mapobjectscale, mo->y + P_RandomRange(PR_DECORATION, -8, 8)*mapobjectscale, mo->z, MT_DRIFTDUST);
	}

	if (player->tulip)
	{
		player->tulip -= 1;
		P_MoveOrigin(mo, mo->tracer->x, mo->tracer->y, mo->tracer->z);
		mo->flags &= ~MF_SHOOTABLE;
		mo->renderflags |= RF_DONTDRAW;
	}

	if (player->tulip == 1)	// expired
	{

		S_StartSound(mo, sfx_s3k81);

		for (UINT8 i = 1; i < 16; i++)
		{
			mobj_t *d = P_SpawnMobj(mo->x, mo->y, mo->z, MT_DRIFTDUST);
			d->angle = (ANG1*360)/16 * i;
			P_InstaThrust(d, d->angle, mapobjectscale*23);
			d->momz = mapobjectscale*8*P_MobjFlip(mo->tracer);
		}

		mo->renderflags &= ~RF_DONTDRAW;
		mo->player->nocontrol = 0;
		P_InstaThrust(mo, mo->tracer->extravalue2, mo->tracer->extravalue1);
		mo->momz = FixedMul(mapobjectscale, BULB_ZTHRUST)*P_MobjFlip(mo->tracer);
		
		mo->flags |= MF_SHOOTABLE;
		player->tuliplaunch = TICRATE;
		player->tulipbuf = 8;
		player->tulip = 0;
		mo->tracer->target = NULL;
		mo->tracer = NULL;
	}
}

void Obj_CloudTouched(mobj_t *special, mobj_t *toucher)
{
	player_t *player = toucher->player;

	if (player->cloudbuf || player->cloud)
		return;

	player->cloud = TICRATE/8;
	player->cloudbuf = TICRATE/3;

	for (UINT8 i = 1; i < 6; i++)
	{
		mobj_t *spawn = P_SpawnMobj(toucher->x + P_RandomRange(PR_DECORATION, -32, 32)*mapobjectscale, toucher->y + P_RandomRange(PR_DECORATION, -32, 32)*mapobjectscale, toucher->z, MT_DRIFTDUST);
		spawn->angle = R_PointToAngle2(toucher->x, toucher->y, spawn->x, spawn->y);
		P_InstaThrust(spawn, spawn->angle, P_RandomRange(PR_DECORATION, 1, 8)*mapobjectscale);
		P_SetObjectMomZ(spawn, P_RandomRange(PR_DECORATION, 4, 10)<<FRACBITS, false);
		spawn->destscale = mapobjectscale * 3;
	}

	toucher->cvmem = FixedHypot(toucher->momx, toucher->momy);

	if (toucher->cvmem)
		toucher->cusval = R_PointToAngle2(0, 0, toucher->momx, toucher->momy);

	if (toucher->cvmem < mapobjectscale*8)
		toucher->cvmem = mapobjectscale*8;

	toucher->tracer = special;
	S_StartSound(toucher, sfx_s3k8a);

}

void Obj_BulbTouched(mobj_t *special, mobj_t *toucher)
{
	if (toucher->player->tulip || toucher->player->tulipbuf)
		return;

	if (special && special->target) 
		return; // player already using it

	if (toucher->player->respawn.timer)
		return;

	toucher->player->tulip = 8*2 +1;

	fixed_t spd = FixedHypot(toucher->momx, toucher->momy);
	angle_t ang = R_PointToAngle2(0, 0, toucher->momx, toucher->momy);

	P_InstaThrust(toucher, 0, 0);
	P_MoveOrigin(toucher, special->x, special->y, special->z);
	toucher->player->nocontrol = 1;
	toucher->tracer = special;
	toucher->flags &= ~MF_SHOOTABLE;
	toucher->renderflags |= RF_DONTDRAW;
	special->target = toucher;
	special->extravalue1 = spd;
	special->extravalue2 = ang;

	S_StartSound(special, sfx_s254);

	// set bulb state:
	P_SetMobjState(special->tracer, S_AGZBULB_ANIM1);
}