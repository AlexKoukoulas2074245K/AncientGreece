///------------------------------------------------------------------------------------------------
///  OverworldBattleProcessingSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldBattleProcessingSystem.h"
#include "../components/OverworldBattleStateComponent.h"
#include "../components/OverworldInteractionHistorySingletonComponent.h"
#include "../utils/OverworldDayTimeUtils.h"
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

void OverworldBattleProcessingSystem::VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    UpdateOverworldBattles(dt, entitiesToProcess);
    CheckForLiveBattle();
}

///-----------------------------------------------------------------------------------------------

void OverworldBattleProcessingSystem::UpdateOverworldBattles(const float dt, const std::vector<genesis::ecs::EntityId>& entities) const
{
    auto& world = genesis::ecs::World::GetInstance();
    
    const auto entitiesToProcess = entities;
    for (const auto& entityId: entitiesToProcess)
    {
        auto& battleStateComponent = world.GetComponent<OverworldBattleStateComponent>(entityId);
        
        const auto overworldDt = dt * GetDayTimeSpeed();
        
        const auto& attackingEntityId = GetOverworldUnitEntityByName(battleStateComponent.mAttackingUnitName);
        const auto& defendingEntityId =
            GetOverworldUnitEntityByName(battleStateComponent.mDefendingUnitName);
        
        auto& attackingUnitStatsComponent = world.GetComponent<UnitStatsComponent>(attackingEntityId);
        auto& defendingUnitStatsComponent = world.GetComponent<UnitStatsComponent>(defendingEntityId);
        
        attackingUnitStatsComponent.mStats.mCurrentBattleDuration -= overworldDt;
        defendingUnitStatsComponent.mStats.mCurrentBattleDuration -= overworldDt;
        
        // Natural battle finish
        if (attackingUnitStatsComponent.mStats.mCurrentBattleDuration < 0.0f)
        {
            attackingUnitStatsComponent.mStats.mCurrentBattleDuration = 0.0f;
            defendingUnitStatsComponent.mStats.mCurrentBattleDuration = 0.0f;
            world.DestroyEntity(entityId);
            continue;
        }
        else
        {
            battleStateComponent.mKillTimer += overworldDt;
            if (battleStateComponent.mKillTimer >= GetProjectedOverworldBattleSingleDeathTime())
            {
                if (attackingUnitStatsComponent.mParty.size() == 1)
                {
                    attackingUnitStatsComponent.mParty[0] = attackingUnitStatsComponent.mStats;
                    attackingUnitStatsComponent.mStats.mCurrentBattleDuration = 0.0f;
                    defendingUnitStatsComponent.mStats.mCurrentBattleDuration = 0.0f;
                    world.DestroyEntity(entityId);
                    continue;
                }
                else if (defendingUnitStatsComponent.mParty.size() == 1)
                {
                    defendingUnitStatsComponent.mParty[0] = defendingUnitStatsComponent.mStats;
                    attackingUnitStatsComponent.mStats.mCurrentBattleDuration = 0.0f;
                    defendingUnitStatsComponent.mStats.mCurrentBattleDuration = 0.0f;
                    world.DestroyEntity(entityId);
                    continue;
                }
                
                const auto shouldAddCasualtyToAttacker = genesis::math::RandomSign() == 1;
                const auto shouldAddCasualtyToDefender = genesis::math::RandomSign() == 1;
                
                if (shouldAddCasualtyToAttacker)
                {
                    attackingUnitStatsComponent.mParty.pop_back();
                }
                if (shouldAddCasualtyToDefender)
                {
                    defendingUnitStatsComponent.mParty.pop_back();
                }
                
                battleStateComponent.mKillTimer = 0.0f;
            }
        }
        
    }
}

///-----------------------------------------------------------------------------------------------

void OverworldBattleProcessingSystem::CheckForLiveBattle() const
{
    auto& world = genesis::ecs::World::GetInstance();
    auto& viewQueueComponent = world.GetSingletonComponent<view::ViewQueueSingletonComponent>();
    
    if (viewQueueComponent.mLastViewDestructionEvent == ATTACK_EVENT_NAME)
    {
        viewQueueComponent.mLastViewDestructionEvent = StringId();
        PrepareLiveBattle();
        DestroyOverworldEntities();
        world.ChangeContext(BATTLE_CONTEXT);
        battle::SetBattleState(battle::BattleState::ONGOING);
        battle::InitCasualties(GetLastInteraction().mInstigatorUnitName, GetLastInteraction().mOtherUnitName);
    }
}

///-----------------------------------------------------------------------------------------------

void OverworldBattleProcessingSystem::PrepareLiveBattle() const
{
    const auto& lastInteraction = GetLastInteraction();
    
    auto attackingSideParty = PrepareBattleParty(lastInteraction.mInstigatorEntityId);
    auto defendingSideParty = PrepareBattleParty(lastInteraction.mOtherEntityId);

    battle::PrepareBattleCamera(lastInteraction.mOtherEntityId == GetPlayerEntity());
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
