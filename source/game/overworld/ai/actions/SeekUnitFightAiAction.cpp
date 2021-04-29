///------------------------------------------------------------------------------------------------
///  SeekUnitFightAiAction.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 29/04/2021.
///-----------------------------------------------------------------------------------------------

#include "SeekUnitFightAiAction.h"
#include "../../components/OverworldTargetComponent.h"
#include "../../utils/OverworldDayTimeUtils.h"
#include "../../utils/OverworldUtils.h"
#include "../../../components/UnitStatsComponent.h"
#include "../../../utils/UnitCollisionUtils.h"
#include "../../../../engine/common/components/NameComponent.h"
#include "../../../../engine/common/components/TransformComponent.h"
#include "../../../../engine/common/utils/Logging.h"
#include "../../../../engine/common/utils/MathUtils.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace ai
{

///-----------------------------------------------------------------------------------------------

namespace
{
}

///-----------------------------------------------------------------------------------------------

void SeekUnitFightAiAction::VStartForEntity(const genesis::ecs::EntityId entityId) const
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
    const auto partySize = unitStatsComponent.mParty.size();
    
    auto allUnits = world.FindAllEntitiesWithName(GetGenericOverworldUnitEntityName());
    allUnits.push_back(GetPlayerEntity());
    
    // Filter units for only the ones with smaller party and excluding this entity itself
    allUnits.erase(std::remove_if(allUnits.begin(), allUnits.end(), [&](const genesis::ecs::EntityId& otherEntityId)
    {
        if (entityId == otherEntityId)
        {
            return true;
        }
        
        const auto& otherUnitStatsComponent = world.GetComponent<UnitStatsComponent>(otherEntityId);
        
        if (otherUnitStatsComponent.mStats.mCurrentBattleDuration > 0.0f)
        {
            return true;
        }
        
        return otherUnitStatsComponent.mParty.size() >= partySize;
    }), allUnits.end());
    
    if (allUnits.size() > 0)
    {
        // Shuffle & Sort units by distance
        const auto& currentPosition = world.GetComponent<genesis::TransformComponent>(entityId).mPosition;
        
        std::shuffle(allUnits.begin(), allUnits.end(), genesis::math::GetRandomEngine());
        std::sort(allUnits.begin(), allUnits.end(), [&](const genesis::ecs::EntityId& lhs, const genesis::ecs::EntityId& rhs)
        {
            return glm::distance(currentPosition, world.GetComponent<genesis::TransformComponent>(lhs).mPosition) > glm::distance(currentPosition, world.GetComponent<genesis::TransformComponent>(rhs).mPosition);
        });
        
        const auto targetEntity = allUnits.front();
        auto targetComponent = std::make_unique<OverworldTargetComponent>();
        targetComponent->mEntityTargetToFollow = targetEntity;
        
        if (world.HasComponent<OverworldTargetComponent>(entityId))
        {
            world.RemoveComponent<OverworldTargetComponent>(entityId);
        }
        world.AddComponent<OverworldTargetComponent>(entityId, std::move(targetComponent));
        
        const auto currentTimeStamp = GetCurrentTimestamp();
        Log(LogType::INFO, "Unit %s started seeking unit %s at time (%d, %d, %.6f)",
            unitStatsComponent.mStats.mUnitName.GetString().c_str(),
            world.GetComponent<UnitStatsComponent>(targetEntity).mStats.mUnitName.GetString().c_str(),
            currentTimeStamp.mYearBc, currentTimeStamp.mDay, currentTimeStamp.mTimeDtAccum);
    }
    else
    {
        const auto currentTimeStamp = GetCurrentTimestamp();
        Log(LogType::INFO, "Unit %s could not find suitable unit to seek at time (%d, %d, %.6f)",
            unitStatsComponent.mStats.mUnitName.GetString().c_str(),
            currentTimeStamp.mYearBc, currentTimeStamp.mDay, currentTimeStamp.mTimeDtAccum);
    }
}

///-----------------------------------------------------------------------------------------------

ActionStatus SeekUnitFightAiAction::VUpdateForEntity(const float, const genesis::ecs::EntityId entityId) const
{
    const auto& world = genesis::ecs::World::GetInstance();
    auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
    
    if (!world.HasComponent<OverworldTargetComponent>(entityId))
    {
        const auto currentTimeStamp = GetCurrentTimestamp();
        Log(LogType::INFO, "Unit %s could not finish seeking at time (%d, %d, %.6f)",
            unitStatsComponent.mStats.mUnitName.GetString().c_str(),
            currentTimeStamp.mYearBc, currentTimeStamp.mDay, currentTimeStamp.mTimeDtAccum);
        return ActionStatus::FINISHED;
    }
    
    const auto& entityTargetComponent = world.GetComponent<OverworldTargetComponent>(entityId);
    
    if (!world.HasEntity(entityTargetComponent.mEntityTargetToFollow))
    {
        const auto currentTimeStamp = GetCurrentTimestamp();
        Log(LogType::INFO, "Unit %s could not finish seeking due to destruction at time (%d, %d, %.6f)",
            unitStatsComponent.mStats.mUnitName.GetString().c_str(),
            currentTimeStamp.mYearBc, currentTimeStamp.mDay, currentTimeStamp.mTimeDtAccum);
        return ActionStatus::FINISHED;
    }
    
    auto& targetUnitStatsComponent = world.GetComponent<UnitStatsComponent>(entityTargetComponent.mEntityTargetToFollow);
    
    if (targetUnitStatsComponent.mStats.mCurrentBattleDuration > 0.0f)
    {
        const auto currentTimeStamp = GetCurrentTimestamp();
        Log(LogType::INFO, "Unit %s could not finish seeking due to ongoing battle at time (%d, %d, %.6f)",
            unitStatsComponent.mStats.mUnitName.GetString().c_str(),
            currentTimeStamp.mYearBc, currentTimeStamp.mDay, currentTimeStamp.mTimeDtAccum);
        return ActionStatus::FINISHED;
    }
    
    // If we have arrived at target position
    if (AreEntitiesColliding(entityId, entityTargetComponent.mEntityTargetToFollow))
    {
        const auto currentTimeStamp = GetCurrentTimestamp();
        Log(LogType::INFO, "Unit %s finished seeking target %s at time (%d, %d, %.6f)", unitStatsComponent.mStats.mUnitName.GetString().c_str(), targetUnitStatsComponent.mStats.mUnitName.GetString().c_str(),
            currentTimeStamp.mYearBc, currentTimeStamp.mDay, currentTimeStamp.mTimeDtAccum);
        
        CreateOverworldBattle(entityId, entityTargetComponent.mEntityTargetToFollow);
        
        return ActionStatus::FINISHED;
    }
    
    return ActionStatus::ONGOING;
}

///-----------------------------------------------------------------------------------------------

Applicability SeekUnitFightAiAction::VGetApplicabilityForEntity(const genesis::ecs::EntityId) const
{
    return Applicability::LOW_APPLICABILITY;
}

///-----------------------------------------------------------------------------------------------

BehaviourState SeekUnitFightAiAction::VGetActionBehaviourState() const
{
    return BehaviourState::SEEKING;
}

///-----------------------------------------------------------------------------------------------

}

}
