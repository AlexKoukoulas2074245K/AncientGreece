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
#include "../../utils/UnitInfoUtils.h"
#include "../../view/components/ViewQueueSingletonComponent.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const StringId ATTACK_EVENT_NAME          = StringId("attack");
    static const StringId ASSIST_ATTACKER_EVENT_NAME = StringId("assist_attacker");
    static const StringId ASSIST_DEFENDER_EVENT_NAME = StringId("assist_defender");
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
    
    if (viewQueueComponent.mLastViewDestructionEvent == ATTACK_EVENT_NAME || viewQueueComponent.mLastViewDestructionEvent == ASSIST_ATTACKER_EVENT_NAME || viewQueueComponent.mLastViewDestructionEvent == ASSIST_DEFENDER_EVENT_NAME)
    {
        const auto assistingAttacker = viewQueueComponent.mLastViewDestructionEvent == ASSIST_ATTACKER_EVENT_NAME;
        const auto assistingDefender = viewQueueComponent.mLastViewDestructionEvent == ASSIST_DEFENDER_EVENT_NAME;
        
        viewQueueComponent.mLastViewDestructionEvent = StringId();
        
        const auto& lastInteraction = GetLastInteraction();
        auto attackerEntityId = lastInteraction.mInstigatorEntityId;
        auto defenderEntityId = lastInteraction.mOtherEntityId;
        
        auto liveBattleEntity = FindBattleStateEntityThatInvolvesUnit(world.GetComponent<UnitStatsComponent>(defenderEntityId).mStats.mUnitName);
        if (liveBattleEntity != genesis::ecs::NULL_ENTITY_ID)
        {
            auto& liveBattleState = world.GetComponent<OverworldBattleStateComponent>(liveBattleEntity);
            attackerEntityId = GetOverworldUnitEntityByName(liveBattleState.mAttackingUnitName);
            defenderEntityId = GetOverworldUnitEntityByName(liveBattleState.mDefendingUnitName);
        }
        
        const auto attackingLeaderUnitName = world.GetComponent<UnitStatsComponent>(attackerEntityId).mStats.mUnitName;
        const auto defendingLeaderUnitName = world.GetComponent<UnitStatsComponent>(defenderEntityId).mStats.mUnitName;
        
        auto attackingSideParty = PrepareBattleParty(attackerEntityId);
        auto defendingSideParty = PrepareBattleParty(defenderEntityId);

        std::vector<UnitStats> assistingAttackerParty;
        std::vector<UnitStats> assistingDefenderParty;
        
        if (assistingAttacker)
        {
            assistingAttackerParty = PrepareBattleParty(GetPlayerEntity());
        }
        else if (assistingDefender)
        {
            assistingDefenderParty = PrepareBattleParty(GetPlayerEntity());
        }
        
        battle::PrepareBattleCamera(lastInteraction.mOtherEntityId == GetPlayerEntity() || assistingDefender);
        battle::PopulateBattleEntities(attackingSideParty, defendingSideParty, assistingAttackerParty, assistingDefenderParty, attackerEntityId, defenderEntityId, assistingAttacker ? GetPlayerEntity() : genesis::ecs::NULL_ENTITY_ID, assistingDefender ? GetPlayerEntity() : genesis::ecs::NULL_ENTITY_ID);
        
        DestroyOverworldEntities();
        world.ChangeContext(BATTLE_CONTEXT);
        battle::SetBattleState(battle::BattleState::ONGOING);
        battle::SetBattleLeaderNames(attackingLeaderUnitName, defendingLeaderUnitName, assistingAttacker ? GetPlayerUnitName() : StringId(), assistingDefender ? GetPlayerUnitName() : StringId(), GetPlayerUnitName());
        battle::InitCasualties(attackingLeaderUnitName, defendingLeaderUnitName, assistingAttacker ? GetPlayerUnitName() : StringId(), assistingDefender ? GetPlayerUnitName() : StringId());
    }
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
