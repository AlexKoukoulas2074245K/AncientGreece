///------------------------------------------------------------------------------------------------
///  VisitRandomCityStateAiAction.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 26/04/2021.
///-----------------------------------------------------------------------------------------------

#include "VisitRandomCityStateAiAction.h"
#include "../../components/OverworldTargetComponent.h"
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
            
            const auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
            Log(LogType::INFO, "Unit %s started moving to city state %s", unitStatsComponent.mStats.mUnitName.GetString().c_str(), cityStateEntry.first.GetString().c_str());
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
        Log(LogType::INFO, "Unit %s finished revisiting last city", unitStatsComponent.mStats.mUnitName.GetString().c_str());
        return ActionStatus::FINISHED;
    }
    
    const auto& entityTargetComponent = world.GetComponent<OverworldTargetComponent>(entityId);
    const auto& targetNameComponent = world.GetComponent<genesis::NameComponent>(entityTargetComponent.mEntityTargetToFollow);
    
    // If we have arrived at target position
    if (AreEntitiesColliding(entityId, entityTargetComponent.mEntityTargetToFollow))
    {
        Log(LogType::INFO, "Unit %s finished moving to city state %s", unitStatsComponent.mStats.mUnitName.GetString().c_str(), targetNameComponent.mName.GetString().c_str());
        return ActionStatus::FINISHED;
    }
    
    return ActionStatus::ONGOING;
}

///-----------------------------------------------------------------------------------------------

Applicability VisitRandomCityStateAiAction::VGetApplicabilityForEntity(const genesis::ecs::EntityId) const
{
    return Applicability::MEDIUM_APPLICABILITY;
}

///-----------------------------------------------------------------------------------------------

}

}
