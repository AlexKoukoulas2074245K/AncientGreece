///------------------------------------------------------------------------------------------------
///  BattleEndHandlingSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 15/04/2021.
///-----------------------------------------------------------------------------------------------

#include "BattleEndHandlingSystem.h"
#include "../components/BattleSideComponent.h"
#include "../utils/BattleUtils.h"
#include "../../components/UnitStatsComponent.h"
#include "../../GameContexts.h"
#include "../../overworld/utils/OverworldInteractionUtils.h"
#include "../../overworld/utils/OverworldUtils.h"
#include "../../overworld/components/OverworldInteractionComponent.h"
#include "../../utils/UnitInfoUtils.h"
#include "../../../engine/animation/utils/AnimationUtils.h"

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const float CELEBRATION_TIME = 5.0f;
}

///-----------------------------------------------------------------------------------------------

BattleEndHandlingSystem::BattleEndHandlingSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void BattleEndHandlingSystem::VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    const auto& lastInteraction = overworld::GetLastInteraction();
    auto& world = genesis::ecs::World::GetInstance();
    auto& battleStateComponent = world.GetSingletonComponent<LiveBattleStateSingletonComponent>();
    
    switch (battleStateComponent.mBattleState)
    {
        case BattleState::ONGOING:
        {
            auto attackingSideUnitCount = 0;
            auto defendingSideUnitCount = 0;
            
            for (const auto& entityId: entitiesToProcess)
            {
                if (IsUnitDead(entityId))
                {
                    continue;
                }
                
                const auto& battleSideComponent = world.GetComponent<BattleSideComponent>(entityId);
                
                if (battleSideComponent.mBattleSideLeaderName == lastInteraction.mInstigatorEntityName)
                {
                    attackingSideUnitCount++;
                }
                else
                {
                    defendingSideUnitCount++;
                }
            }
            
            if (attackingSideUnitCount == 0)
            {
                battleStateComponent.mBattleResult = BattleResult::DEFENDER_VICTORIOUS;
                battleStateComponent.mBattleState = BattleState::UNITS_CELEBRATING;
                battleStateComponent.mCelebrationTimer = CELEBRATION_TIME;
            }
            else if (defendingSideUnitCount == 0)
            {
                battleStateComponent.mBattleResult = BattleResult::ATTACKER_VICTORIOUS;
                battleStateComponent.mBattleState = BattleState::UNITS_CELEBRATING;
                battleStateComponent.mCelebrationTimer = CELEBRATION_TIME;
            }
        } break;
            
            
        case BattleState::UNITS_CELEBRATING:
        {
            for (const auto& entityId: entitiesToProcess)
            {
                if (!IsUnitDead(entityId))
                {
                    genesis::animation::ChangeAnimation(entityId, StringId("celebrating"));
                }
            }
            
            battleStateComponent.mCelebrationTimer -= dt;
            if (battleStateComponent.mCelebrationTimer < 0.0f)
            {
                battleStateComponent.mBattleState = BattleState::FINISHED;
            }
        } break;
            
        case BattleState::FINISHED:
        {
            DestroyBattleEntities();
            overworld::PopulateOverworldEntities();
            overworld::TryLoadOverworldStateFromFile();
            overworld::PrepareOverworldCamera();
            world.ChangeContext(MAP_CONTEXT);
        } break;
    }
}

///-----------------------------------------------------------------------------------------------

}
