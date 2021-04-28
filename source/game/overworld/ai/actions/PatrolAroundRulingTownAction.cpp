///------------------------------------------------------------------------------------------------
///  PatrolAroundRulingTownAction.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 26/04/2021.
///-----------------------------------------------------------------------------------------------

#include "PatrolAroundRulingTownAction.h"
#include "../../components/OverworldTargetComponent.h"
#include "../../utils/OverworldDayTimeUtils.h"
#include "../../utils/OverworldUtils.h"
#include "../../../components/UnitStatsComponent.h"
#include "../../../utils/CityStateInfoUtils.h"
#include "../../../../engine/common/components/TransformComponent.h"
#include "../../../../engine/common/utils/Logging.h"
#include "../../../../engine/rendering/utils/HeightMapUtils.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace ai
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const StringId LAST_PATROL_EVENT_TIMESTAMP = StringId("last_patrol_timestamp");
    static const int PATROLLING_POSITION_COUNT = 8;
    static const glm::vec2 PATROLLING_RADIUS_RANGE = glm::vec2(0.03, 0.07);
}

///-----------------------------------------------------------------------------------------------

PatrolAroundRulingTownAction::PatrolAroundRulingTownAction()
{
}

///-----------------------------------------------------------------------------------------------

void PatrolAroundRulingTownAction::VStartForEntity(const genesis::ecs::EntityId entityId) const
{
    auto& world = genesis::ecs::World::GetInstance();
    auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
    auto& unitAiComponent = world.GetComponent<OverworldUnitAiComponent>(entityId);
    
    unitAiComponent.mPathPositions.clear();
    
    const auto rulingTownName = GetRulingCityStateOfUnit(unitStatsComponent.mStats.mUnitName);
    if (rulingTownName != StringId())
    {
        const auto townPosition = world.GetComponent<genesis::TransformComponent>(world.FindEntityWithName(rulingTownName)).mPosition;
        const auto patrollingArchSize = (2.0f * genesis::math::PI) / PATROLLING_POSITION_COUNT;
        const auto patrollingRadius = genesis::math::RandomFloat(PATROLLING_RADIUS_RANGE.s, PATROLLING_RADIUS_RANGE.t);
        
        for (int i = 0; i < PATROLLING_POSITION_COUNT; ++i)
        {
            auto position = glm::vec3(townPosition.x + genesis::math::Sinf(i * patrollingArchSize) * patrollingRadius, townPosition.y + genesis::math::Cosf(i * patrollingArchSize) * patrollingRadius, 0.0f);
            position.z = genesis::rendering::GetTerrainHeightAtPosition(GetMapEntity(), position);
            
            unitAiComponent.mPathPositions.push_front(position);
        }
    }
    
    unitStatsComponent.mStats.mUnitEventTimestamps[LAST_PATROL_EVENT_TIMESTAMP] = GetCurrentTimestamp();
    Log(LogType::INFO, "Unit %s started patrolling at time (%d, %d, %.6f)", unitStatsComponent.mStats.mUnitName.GetString().c_str(), unitStatsComponent.mStats.mUnitEventTimestamps[LAST_PATROL_EVENT_TIMESTAMP].mYearBc, unitStatsComponent.mStats.mUnitEventTimestamps[LAST_PATROL_EVENT_TIMESTAMP].mDay, unitStatsComponent.mStats.mUnitEventTimestamps[LAST_PATROL_EVENT_TIMESTAMP].mTimeDtAccum);
}

///-----------------------------------------------------------------------------------------------

ActionStatus PatrolAroundRulingTownAction::VUpdateForEntity(const float, const genesis::ecs::EntityId entityId) const
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
    auto& unitAiComponent = world.GetComponent<OverworldUnitAiComponent>(entityId);

    // No target component and has more path positions
    if (!world.HasComponent<OverworldTargetComponent>(entityId) && !unitAiComponent.mPathPositions.empty())
    {
        auto targetComponent = std::make_unique<OverworldTargetComponent>();
        targetComponent->mTargetPosition = unitAiComponent.mPathPositions.front();
        unitAiComponent.mPathPositions.pop_front();
        world.AddComponent<OverworldTargetComponent>(entityId, std::move(targetComponent));
    }
    // No target component and no other path positions
    else if (!world.HasComponent<OverworldTargetComponent>(entityId))
    {
        const auto currentTimeStamp = GetCurrentTimestamp();
        Log(LogType::INFO, "Unit %s finished patrolling at time (%d, %d, %.6f)", unitStatsComponent.mStats.mUnitName.GetString().c_str(), currentTimeStamp.mYearBc, currentTimeStamp.mDay, currentTimeStamp.mTimeDtAccum);

        return ActionStatus::FINISHED;
    }

    return ActionStatus::ONGOING;
}

///-----------------------------------------------------------------------------------------------

Applicability PatrolAroundRulingTownAction::VGetApplicabilityForEntity(const genesis::ecs::EntityId entityId) const
{
    const auto& world = genesis::ecs::World::GetInstance();
    auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
    
    const auto currentTimeStamp = GetCurrentTimestamp();
    const auto timeSinceLastPatrol = currentTimeStamp - unitStatsComponent.mStats.mUnitEventTimestamps[LAST_PATROL_EVENT_TIMESTAMP];
    
    assert(timeSinceLastPatrol >= 0.0f);
    
    if (timeSinceLastPatrol < 3.0f * GetDayDuration())
    {
        return Applicability::LOW_APPLICABILITY;
    }
    else
    {
        return Applicability::HIGH_APPLICABILITY;
    }
}

///-----------------------------------------------------------------------------------------------

BehaviourState PatrolAroundRulingTownAction::VGetActionBehaviourState() const
{
    return BehaviourState::PATROLLING;
}

///-----------------------------------------------------------------------------------------------

}

}
