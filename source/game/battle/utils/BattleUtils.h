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
#include "../components/BattleStateSingletonComponent.h"

///------------------------------------------------------------------------------------------------

struct UnitStats;

namespace battle
{

///------------------------------------------------------------------------------------------------

genesis::ecs::EntityId GetMapEntity();

///------------------------------------------------------------------------------------------------

StringId GetProjectileEntityName();

///------------------------------------------------------------------------------------------------

std::vector<genesis::ecs::EntityId> GetAllBattleUnitEntities();

///------------------------------------------------------------------------------------------------

bool IsBattleFinished();

///------------------------------------------------------------------------------------------------

void SetBattleState(const BattleState battleState);

///------------------------------------------------------------------------------------------------

void InitCasualties(const StringId attackingLeaderUnitName, const StringId defendingLeaderUnitName);

///------------------------------------------------------------------------------------------------

void DamageUnit(const genesis::ecs::EntityId unitEntity, const int damage);

///------------------------------------------------------------------------------------------------

void AddBattleCasualty(const StringId unitType, const StringId leaderName);

///------------------------------------------------------------------------------------------------

void AddCollidableDataToArrow(const genesis::ecs::EntityId arrowEntity);

///------------------------------------------------------------------------------------------------

void PopulateBattleEntities(const std::vector<UnitStats>& attackingSideParty, const std::vector<UnitStats>& defendingSideParty, const genesis::ecs::EntityId attackingLeaderEntity, const genesis::ecs::EntityId defendingLeaderEntity);

///------------------------------------------------------------------------------------------------

void DestroyBattleEntities();

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
