///------------------------------------------------------------------------------------------------
///  BattleUtils.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///------------------------------------------------------------------------------------------------

#ifndef BattleUtils_h
#define BattleUtils_h

///------------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///------------------------------------------------------------------------------------------------

struct UnitStats;

namespace battle
{

///------------------------------------------------------------------------------------------------

genesis::ecs::EntityId GetMapEntity();

///------------------------------------------------------------------------------------------------

void DamageUnit(const genesis::ecs::EntityId unitEntity, const int damage);

///------------------------------------------------------------------------------------------------

void AddCollidableDataToArrow(const genesis::ecs::EntityId arrowEntity);

///------------------------------------------------------------------------------------------------

void QueueBattle(const genesis::ecs::EntityId attackingEntity, const genesis::ecs::EntityId defendingEntity);

///------------------------------------------------------------------------------------------------

void PopulateBattleEntities(const std::vector<UnitStats>& attackingSideParty, const std::vector<UnitStats>& defendingSideParty, const genesis::ecs::EntityId attackingLeaderEntity, const genesis::ecs::EntityId defendingLeaderEntity);

///------------------------------------------------------------------------------------------------

bool AreUnitsInMeleeDistance(const genesis::ecs::EntityId unitEntityA, const genesis::ecs::EntityId unitEntityB);

///------------------------------------------------------------------------------------------------

bool AreUnitsInDoubleMeleeDistance(const genesis::ecs::EntityId unitEntityA, const genesis::ecs::EntityId unitEntityB);

///------------------------------------------------------------------------------------------------

void PrepareBattleCamera();

///------------------------------------------------------------------------------------------------

}

///------------------------------------------------------------------------------------------------

#endif /* BattleUtils_h */
