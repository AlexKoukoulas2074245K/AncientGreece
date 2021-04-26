///------------------------------------------------------------------------------------------------
///  RestAiAction.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 26/04/2021.
///-----------------------------------------------------------------------------------------------

#include "RestAiAction.h"
#include "../../utils/OverworldDayTimeUtils.h"
#include "../../../components/UnitStatsComponent.h"
#include "../../../../engine/common/utils/Logging.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace ai
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const float REST_DURATION_IN_HOURS = 6.0f;
}

///-----------------------------------------------------------------------------------------------

RestAiAction::RestAiAction()
{
}

///-----------------------------------------------------------------------------------------------

void RestAiAction::VStartForEntity(const genesis::ecs::EntityId entityId) const
{
    auto& world = genesis::ecs::World::GetInstance();
    auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
    
    unitStatsComponent.mStats.mUnitLastRestTimeStamp = overworld::GetCurrentTimestamp();
    unitStatsComponent.mStats.mCurrentRestingDuration = overworld::GetHourDuration() * REST_DURATION_IN_HOURS;
    
    Log(LogType::INFO, "Unit %s started resting for %.6f at (%d, %d, %.6f)", unitStatsComponent.mStats.mUnitName.GetString().c_str(), unitStatsComponent.mStats.mCurrentRestingDuration, unitStatsComponent.mStats.mUnitLastRestTimeStamp.mYearBc, unitStatsComponent.mStats.mUnitLastRestTimeStamp.mDay, unitStatsComponent.mStats.mUnitLastRestTimeStamp.mTimeDtAccum);
}

///-----------------------------------------------------------------------------------------------

ActionStatus RestAiAction::VUpdateForEntity(const float dt, const genesis::ecs::EntityId entityId) const
{
    auto& world = genesis::ecs::World::GetInstance();
    auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
    
    unitStatsComponent.mStats.mCurrentRestingDuration -= GetDayTimeSpeed() * dt;
    if (unitStatsComponent.mStats.mCurrentRestingDuration < 0.0f)
    {
        unitStatsComponent.mStats.mCurrentRestingDuration = 0.0f;
        return ActionStatus::FINISHED;
        
        Log(LogType::INFO, "Unit %s finished resting at (%d, %d, %.6f)", unitStatsComponent.mStats.mUnitName.GetString().c_str(), unitStatsComponent.mStats.mUnitLastRestTimeStamp.mYearBc, unitStatsComponent.mStats.mUnitLastRestTimeStamp.mDay, unitStatsComponent.mStats.mUnitLastRestTimeStamp.mTimeDtAccum);
    }
    
    return ActionStatus::ONGOING;
}

///-----------------------------------------------------------------------------------------------

Applicability RestAiAction::VGetApplicabilityForEntity(const genesis::ecs::EntityId entityId) const
{
    const auto& world = genesis::ecs::World::GetInstance();
    const auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
    if (unitStatsComponent.mStats.mCurrentRestingDuration > 0.0f)
    {
        return Applicability::NOT_APPLICABLE;
    }
    else
    {
        const auto currentTimeStamp = overworld::GetCurrentTimestamp();
        const auto timeSinceLastRest = currentTimeStamp - unitStatsComponent.mStats.mUnitLastRestTimeStamp;
        
        assert(timeSinceLastRest >= 0.0f);
        
        if (timeSinceLastRest < GetDayDuration())
        {
            return Applicability::LOW_APPLICABILITY;
        }
        else if (timeSinceLastRest < GetDayDuration() * 2.0f)
        {
            return Applicability::MEDIUM_APPLICABILITY;
        }
        else
        {
            return Applicability::HIGH_APPLICABILITY;
        }
    }
}

///-----------------------------------------------------------------------------------------------

}

}
