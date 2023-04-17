/* object-specific code */
#ifndef k_objects_H
#define k_objects_H

#include "taglist.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Hyudoro */
void Obj_InitHyudoroCenter(mobj_t *center, mobj_t *master);
void Obj_HyudoroDeploy(mobj_t *master);
void Obj_HyudoroThink(mobj_t *actor);
void Obj_HyudoroCenterThink(mobj_t *actor);
void Obj_HyudoroCollide(mobj_t *special, mobj_t *toucher);

/* Garden Top */
void Obj_GardenTopDeploy(mobj_t *rider);
mobj_t *Obj_GardenTopThrow(player_t *player);
mobj_t *Obj_GardenTopDestroy(player_t *player);
void Obj_GardenTopThink(mobj_t *top);
void Obj_GardenTopSparkThink(mobj_t *spark);
void Obj_GardenTopArrowThink(mobj_t *arrow);
boolean Obj_GardenTopPlayerIsGrinding(player_t *player);

/* Shrink */
void Obj_PohbeeThinker(mobj_t *pohbee);
void Obj_PohbeeRemoved(mobj_t *pohbee);
void Obj_ShrinkGunRemoved(mobj_t *gun);
boolean Obj_ShrinkLaserCollide(mobj_t *gun, mobj_t *victim);
void Obj_CreateShrinkPohbees(player_t *owner);

/* Item Debris */
void Obj_SpawnItemDebrisEffects(mobj_t *collectible, mobj_t *collector);
void Obj_ItemDebrisThink(mobj_t *debris);
fixed_t Obj_ItemDebrisBounce(mobj_t *debris, fixed_t momz);

/* SPB */
void Obj_SPBThink(mobj_t *spb);
void Obj_SPBExplode(mobj_t *spb);
void Obj_SPBTouch(mobj_t *spb, mobj_t *toucher);

/* SPB Juicebox Rings */
void Obj_MantaRingThink(mobj_t *manta);
mobj_t *Obj_MantaRingCreate(mobj_t *spb, mobj_t *owner, mobj_t *chase);

/* Orbinaut */
void Obj_OrbinautThink(mobj_t *th);
boolean Obj_OrbinautJawzCollide(mobj_t *t1, mobj_t *t2);
void Obj_OrbinautThrown(mobj_t *th, fixed_t finalSpeed, SINT8 dir);
void Obj_GachaBomThrown(mobj_t *th, fixed_t finalSpeed, SINT8 dir);
void Obj_OrbinautJawzMoveHeld(player_t *player);
boolean Obj_GachaBomWasTossed(mobj_t *th);

/* Jawz */
void Obj_JawzThink(mobj_t *th);
void Obj_JawzThrown(mobj_t *th, fixed_t finalSpeed, SINT8 dir);

/* Duel Bomb */
void Obj_DuelBombThink(mobj_t *bomb);
void Obj_DuelBombReverse(mobj_t *bomb);
void Obj_DuelBombTouch(mobj_t *bomb, mobj_t *toucher);
void Obj_DuelBombInit(mobj_t *bomb);

/* Broly Ki */
mobj_t *Obj_SpawnBrolyKi(mobj_t *source, tic_t duration);
boolean Obj_BrolyKiThink(mobj_t *ki);

/* Special Stage UFO */
waypoint_t *K_GetSpecialUFOWaypoint(mobj_t *ufo);
void Obj_SpecialUFOThinker(mobj_t *ufo);
boolean Obj_SpecialUFODamage(mobj_t *ufo, mobj_t *inflictor, mobj_t *source, UINT8 damageType);
void Obj_PlayerUFOCollide(mobj_t *ufo, mobj_t *other);
boolean Obj_UFOEmeraldCollect(mobj_t *ufo, mobj_t *toucher);
void Obj_UFOPieceThink(mobj_t *piece);
void Obj_UFOPieceDead(mobj_t *piece);
void Obj_UFOPieceRemoved(mobj_t *piece);
mobj_t *Obj_CreateSpecialUFO(void);
UINT32 K_GetSpecialUFODistance(void);

/* Monitors */
mobj_t *Obj_SpawnMonitor(mobj_t *origin, UINT8 numItemTypes, UINT8 emerald);
void Obj_MonitorSpawnParts(mobj_t *monitor);
void Obj_MonitorPartThink(mobj_t *part);
fixed_t Obj_MonitorGetDamage(mobj_t *monitor, mobj_t *inflictor, UINT8 damagetype);
void Obj_MonitorOnDamage(mobj_t *monitor, mobj_t *inflictor, INT32 damage);
void Obj_MonitorOnDeath(mobj_t *monitor);
void Obj_MonitorShardThink(mobj_t *shard);
UINT32 Obj_MonitorGetEmerald(const mobj_t *monitor);
void Obj_MonitorSetItemSpot(mobj_t *monitor, mobj_t *spot);

/* Item Spot */
boolean Obj_ItemSpotIsAvailable(const mobj_t *spot);
void Obj_ItemSpotAssignMonitor(mobj_t *spot, mobj_t *monitor);
void Obj_ItemSpotUpdate(mobj_t *spot);

/* Loops */
mobj_t *Obj_FindLoopCenter(const mtag_t tag);
void Obj_InitLoopEndpoint(mobj_t *end, mobj_t *anchor);
void Obj_InitLoopCenter(mobj_t *center);
void Obj_LinkLoopAnchor(mobj_t *anchor, mobj_t *center, UINT8 type);
void Obj_LoopEndpointCollide(mobj_t *special, mobj_t *toucher);

/* Drop Target */
void Obj_BeginDropTargetMorph(mobj_t *target, skincolornum_t color);
boolean Obj_DropTargetMorphThink(mobj_t *morph);

/* Ring Shooter */
boolean Obj_RingShooterThinker(mobj_t *mo);
boolean Obj_PlayerRingShooterFreeze(player_t *const player);
void Obj_RingShooterInput(player_t *player);
void Obj_PlayerUsedRingShooter(mobj_t *base, player_t *player);
void Obj_RingShooterDelete(mobj_t *mo);
void Obj_UpdateRingShooterFace(mobj_t *part);

#ifdef __cplusplus
} // extern "C"
#endif

#endif/*k_objects_H*/
