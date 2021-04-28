///------------------------------------------------------------------------------------------------
///  VisitRandomCityStateAiAction.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 26/04/2021.
///-----------------------------------------------------------------------------------------------

#include "VisitRandomCityStateAiAction.h"
#include "../../components/OverworldTargetComponent.h"
#include "../../utils/OverworldDayTimeUtils.h"
#include "../../utils/OverworldUtils.h"
#include "../../../components/CityStateInfoSingletonComponent.h"
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
    static const StringId LAST_CITY_STATE_VISIT_EVENT_TIMESTAMP = StringId("last_city_state_visit_timestamp");
}

///-----------------------------------------------------------------------------------------------

void VisitRandomCityStateAiAction::VStartForEntity(const genesis::ecs::EntityId entityId) const
{
    auto& world = genesis::ecs::World::GetInstance();
    auto& cityStateInfoComponent = world.GetSingletonComponent<CityStateInfoSingletonComponent>();
    auto randomCityIndex = genesis::math::RandomInt(1, cityStateInfoComponent.mCityStateNameToInfo.size());
    
    for (auto& cityStateEntry: cityStateInfoComponent.mCityStateNameToInfo)
    {
        if (--randomCityIndex == 0)
        {
            auto targetComponent = std::make_unique<OverworldTargetComponent>();
            targetComponent->mEntityTargetToFollow = GetCityStateEntity(cityStateEntry.first);
            world.AddComponent<OverworldTargetComponent>(entityId, std::move(targetComponent));
            
            auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
            unitStatsComponent.mStats.mUnitEventTimestamps[LAST_CITY_STATE_VISIT_EVENT_TIMESTAMP] = GetCurrentTimestamp();
            
            Log(LogType::INFO, "Unit %s started moving to city state %s at time (%d, %d, %.6f)", unitStatsComponent.mStats.mUnitName.GetString().c_str(), cityStateEntry.first.GetString().c_str(), unitStatsComponent.mStats.mUnitEventTimestamps[LAST_CITY_STATE_VISIT_EVENT_TIMESTAMP].mYearBc, unitStatsComponent.mStats.mUnitEventTimestamps[LAST_CITY_STATE_VISIT_EVENT_TIMESTAMP].mDay, unitStatsComponent.mStats.mUnitEventTimestamps[LAST_CITY_STATE_VISIT_EVENT_TIMESTAMP].mTimeDtAccum);
            break;
        }
    }
}

///-----------------------------------------------------------------------------------------------

ActionStatus VisitRandomCityStateAiAction::VUpdateForEntity(const float, const genesis::ecs::EntityId entityId) const
{
    const auto& world = genesis::ecs::World::GetInstance();
    const auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
    
    if (!world.HasComponent<OverworldTargetComponent>(entityId))
    {
        const auto currentTimeStamp = GetCurrentTimestamp();
        Log(LogType::INFO, "Unit %s finished revisiting last city at at time (%d, %d, %.6f)",
            unitStatsComponent.mStats.mUnitName.GetString().c_str(),
            currentTimeStamp.mYearBc, currentTimeStamp.mDay, currentTimeStamp.mTimeDtAccum);
        return ActionStatus::FINISHED;
    }
    
    const auto& entityTargetComponent = world.GetComponent<OverworldTargetComponent>(entityId);
    const auto& targetNameComponent = world.GetComponent<genesis::NameComponent>(entityTargetComponent.mEntityTargetToFollow);
    
    // If we have arrived at target position
    if (AreEntitiesColliding(entityId, entityTargetComponent.mEntityTargetToFollow))
    {
        const auto currentTimeStamp = GetCurrentTimestamp();
        Log(LogType::INFO, "Unit %s finished moving to city state %s at time (%d, %d, %.6f)", unitStatsComponent.mStats.mUnitName.GetString().c_str(), targetNameComponent.mName.GetString().c_str(),
            currentTimeStamp.mYearBc, currentTimeStamp.mDay, currentTimeStamp.mTimeDtAccum);
        return ActionStatus::FINISHED;
    }
    
    return ActionStatus::ONGOING;
}

///-----------------------------------------------------------------------------------------------

Applicability VisitRandomCityStateAiAction::VGetApplicabilityForEntity(const genesis::ecs::EntityId entityId) const
{
    const auto& world = genesis::ecs::World::GetInstance();
    auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
    
    if (world.HasComponent<OverworldTargetComponent>(entityId))
    {
        return Applicability::NOT_APPLICABLE;
    }
    
    const auto currentTimeStamp = GetCurrentTimestamp();
    const auto timeSinceLastCityStateVisit = currentTimeStamp - unitStatsComponent.mStats.mUnitEventTimestamps[LAST_CITY_STATE_VISIT_EVENT_TIMESTAMP];
    
    assert(timeSinceLastCityStateVisit >= 0.0f);
    
    if (timeSinceLastCityStateVisit < GetDayDuration())
    {
        return Applicability::LOW_APPLICABILITY;
    }
    else if (timeSinceLastCityStateVisit < GetDayDuration() * 4.0f)
    {
        return Applicability::MEDIUM_APPLICABILITY;
    }
    else
    {
        return Applicability::HIGH_APPLICABILITY;
    }
}

///-----------------------------------------------------------------------------------------------

BehaviourState VisitRandomCityStateAiAction::VGetActionBehaviourState() const
{
    return BehaviourState::TRAVELLING;
}

///-----------------------------------------------------------------------------------------------

}

}
