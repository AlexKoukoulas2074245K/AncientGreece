///------------------------------------------------------------------------------------------------
///  BattleTargetAcquisitionSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///-----------------------------------------------------------------------------------------------

#include "BattleTargetAcquisitionSystem.h"
#include "../components/BattleSideComponent.h"
#include "../components/BattleTargetComponent.h"
#include "../../../engine/common/components/TransformComponent.h"

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------

BattleTargetAcquisitionSystem::BattleTargetAcquisitionSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void BattleTargetAcquisitionSystem::VUpdate(const float, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    for (const auto& entityId: entitiesToProcess)
    {
        ValidateCurrentTargetComponent(entityId);
        PickNewTargetForEntity(entityId, entitiesToProcess);
    }
}

///-----------------------------------------------------------------------------------------------

void BattleTargetAcquisitionSystem::ValidateCurrentTargetComponent(const genesis::ecs::EntityId entityId) const
{
    auto& world = genesis::ecs::World::GetInstance();
    if (world.HasComponent<BattleTargetComponent>(entityId))
    {
        const auto& battleTargetComponent = world.GetComponent<BattleTargetComponent>(entityId);
        
        // If current unit has a target and target is still alive continue
        if (!world.HasEntity(battleTargetComponent.mTargetEntity))
        {
            // Else erase this units target and recalculate
            world.RemoveComponent<BattleTargetComponent>(entityId);
        }
    }
}

///-----------------------------------------------------------------------------------------------

void BattleTargetAcquisitionSystem::PickNewTargetForEntity(const genesis::ecs::EntityId entityId, const std::vector<genesis::ecs::EntityId> entities) const
{
    auto& world = genesis::ecs::World::GetInstance();
    if (!world.HasComponent<BattleTargetComponent>(entityId))
    {
        const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entityId);
        const auto& battleSideComponent = world.GetComponent<BattleSideComponent>(entityId);
        auto battleTargetComponent = std::make_unique<BattleTargetComponent>();
        
        // Find closest enemy target
        float minDistanceFound = 100.0f;
        for (const auto& otherEntityId: entities)
        {
            const auto& otherBattleSideComponent = world.GetComponent<BattleSideComponent>(otherEntityId);
            
            // Don't process units in the same army
            if (battleSideComponent.mBattleSideLeaderName == otherBattleSideComponent.mBattleSideLeaderName)
            {
                continue;
            }
            
            const auto& otherTransformComponent = world.GetComponent<genesis::TransformComponent>(otherEntityId);
            
            // Compare square distances with current target
            const auto dist2 = genesis::math::Distance2(transformComponent.mPosition, otherTransformComponent.mPosition);
            if (dist2 < minDistanceFound)
            {
                minDistanceFound = dist2;
                battleTargetComponent->mTargetEntity = otherEntityId;
            }
        }
        
        world.AddComponent<BattleTargetComponent>(entityId, std::move(battleTargetComponent));
    }
}

///-----------------------------------------------------------------------------------------------

}
