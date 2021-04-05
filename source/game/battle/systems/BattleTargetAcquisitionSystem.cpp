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
        PickOptimalTargetForEntity(entityId, entitiesToProcess);
    }
}

///-----------------------------------------------------------------------------------------------

void BattleTargetAcquisitionSystem::ValidateCurrentTargetComponent(const genesis::ecs::EntityId entityId) const
{
    auto& world = genesis::ecs::World::GetInstance();
    if (world.HasComponent<BattleTargetComponent>(entityId))
    {
        auto& battleTargetComponent = world.GetComponent<BattleTargetComponent>(entityId);
        
        // If current unit has a target and target is still alive continue
        if (!world.HasEntity(battleTargetComponent.mTargetEntity))
        {
            // Else erase this units target and recalculate
            world.RemoveComponent<BattleTargetComponent>(entityId);
        }
    }
}

///-----------------------------------------------------------------------------------------------

void BattleTargetAcquisitionSystem::PickOptimalTargetForEntity(const genesis::ecs::EntityId entityId, const std::vector<genesis::ecs::EntityId> entities) const
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entityId);
    const auto& battleSideComponent = world.GetComponent<BattleSideComponent>(entityId);
    
    const auto targetUnitEntity = FindClosestTargetUnit(battleSideComponent.mBattleSideLeaderName, transformComponent, entities);
    
    if (!world.HasComponent<BattleTargetComponent>(entityId))
    {
        auto battleTargetComponent = std::make_unique<BattleTargetComponent>();
        battleTargetComponent->mTargetEntity = targetUnitEntity;
        assert(battleTargetComponent->mTargetEntity != genesis::ecs::NULL_ENTITY_ID && "Can't find target for unit");
        world.AddComponent<BattleTargetComponent>(entityId, std::move(battleTargetComponent));
    }
    else
    {
        auto& battleTargetComponent = world.GetComponent<BattleTargetComponent>(entityId);
        battleTargetComponent.mTargetEntity = targetUnitEntity;
        assert(battleTargetComponent.mTargetEntity != genesis::ecs::NULL_ENTITY_ID && "Can't find target for unit");
    }
}

///-----------------------------------------------------------------------------------------------

genesis::ecs::EntityId BattleTargetAcquisitionSystem::FindClosestTargetUnit(const StringId currentEntityBattleLeader, const genesis::TransformComponent& currentEntityTransformComponent, const std::vector<genesis::ecs::EntityId> entities) const
{
    const auto& world = genesis::ecs::World::GetInstance();
    auto closestTargetUnitEntity = genesis::ecs::NULL_ENTITY_ID;
    auto minDistanceFound = 100.0f;
    
    for (const auto& otherEntityId: entities)
    {
        const auto& otherBattleSideComponent = world.GetComponent<BattleSideComponent>(otherEntityId);
        
        // Don't process units in the same army
        if (currentEntityBattleLeader == otherBattleSideComponent.mBattleSideLeaderName)
        {
            continue;
        }
        
        const auto& otherTransformComponent = world.GetComponent<genesis::TransformComponent>(otherEntityId);
        
        // Compare square distances with current target
        const auto dist2 = genesis::math::Distance2(currentEntityTransformComponent.mPosition, otherTransformComponent.mPosition);
        if (dist2 < minDistanceFound)
        {
            minDistanceFound = dist2;
            closestTargetUnitEntity = otherEntityId;
        }
    }
    
    return closestTargetUnitEntity;
}

///-----------------------------------------------------------------------------------------------

}
