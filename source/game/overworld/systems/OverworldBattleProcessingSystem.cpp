///------------------------------------------------------------------------------------------------
///  OverworldBattleProcessingSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldBattleProcessingSystem.h"
#include "../components/OverworldInteractionHistorySingletonComponent.h"
#include "../utils/OverworldInteractionUtils.h"
#include "../utils/OverworldUtils.h"
#include "../../battle/utils/BattleUtils.h"
#include "../../components/UnitStatsComponent.h"
#include "../../GameContexts.h"
#include "../../components/CollidableComponent.h"
#include "../../view/components/ViewQueueSingletonComponent.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const StringId ATTACK_EVENT_NAME = StringId("attack");
}

///-----------------------------------------------------------------------------------------------

OverworldBattleProcessingSystem::OverworldBattleProcessingSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void OverworldBattleProcessingSystem::VUpdate(const float, const std::vector<genesis::ecs::EntityId>&) const
{
    auto& world = genesis::ecs::World::GetInstance();
    auto& viewQueueComponent = world.GetSingletonComponent<view::ViewQueueSingletonComponent>();
    
    if (viewQueueComponent.mLastViewDestructionEvent == ATTACK_EVENT_NAME)
    {
        viewQueueComponent.mLastViewDestructionEvent = StringId();
        PrepareLiveBattle();
        DestroyOverworldEntities();
        world.ChangeContext(BATTLE_CONTEXT);
    }
}

///-----------------------------------------------------------------------------------------------

void OverworldBattleProcessingSystem::PrepareLiveBattle() const
{
    const auto& lastInteraction = GetLastInteraction();
    
    auto attackingSideParty = PrepareBattleParty(lastInteraction.mInstigatorEntityId);
    auto defendingSideParty = PrepareBattleParty(lastInteraction.mOtherEntityId);

    battle::PrepareBattleCamera();
    battle::PopulateBattleEntities(attackingSideParty, defendingSideParty, lastInteraction.mInstigatorEntityId, lastInteraction.mOtherEntityId);
}

///-----------------------------------------------------------------------------------------------

std::vector<UnitStats> OverworldBattleProcessingSystem::PrepareBattleParty(const genesis::ecs::EntityId partyLeaderEntityId) const
{
    const auto& world = genesis::ecs::World::GetInstance();
    const auto& leaderUnitStatsComponent = world.GetComponent<UnitStatsComponent>(partyLeaderEntityId);
    auto party = leaderUnitStatsComponent.mParty;
    
    // Sort by unit type (archers at the back)
    std::partition(party.begin(), party.end(), [](const UnitStats& unitStats)
    {
        return !unitStats.mIsRangedUnit;
    });
    
    return party;
}

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------
