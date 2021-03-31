///------------------------------------------------------------------------------------------------
///  UnitFactoryUtils.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///------------------------------------------------------------------------------------------------

#include "UnitFactoryUtils.h"
#include "../components/UnitBaseStatsSingletonComponent.h"
#include "../components/UnitStatsComponent.h"
#include "../overworld/components/HighlightableComponent.h"
#include "../utils/UnitCollisionUtils.h"
#include "../utils/UnitInfoUtils.h"
#include "../../engine/rendering/utils/MeshUtils.h"

///------------------------------------------------------------------------------------------------

genesis::ecs::EntityId CreateUnit(const StringId unitTypeName, const StringId unitName /* StringId() */, const StringId entityName /* StringId() */, const float speedMultipiler /* 1.0f */, const glm::vec3& position /* glm::vec3() */, const glm::vec3& rotation /* glm::vec3() */, const glm::vec3 scale /* glm::vec3() */)
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& unitBaseStats = world.GetSingletonComponent<UnitBaseStatsSingletonComponent>().mUnitTypeNameToBaseStats.at(unitTypeName);
    
    auto entity = genesis::rendering::LoadAndCreateAnimatedModelByName(unitBaseStats.mUnitModelName.GetString(), position, rotation, scale * unitBaseStats.mUnitModelScaleFactor, entityName);
    
    auto unitStatsComponent = std::make_unique<UnitStatsComponent>();
    unitStatsComponent->mStats.mSpeedMultiplier = speedMultipiler;
    unitStatsComponent->mStats.mUnitType = unitTypeName;
    unitStatsComponent->mStats.mUnitName = unitName == StringId() ? unitTypeName : unitName;
    unitStatsComponent->mStats.mDamage = unitBaseStats.mBaseDamage;
    unitStatsComponent->mStats.mHealth = 100;
    unitStatsComponent->mParty.push_back(unitStatsComponent->mStats);
    
    world.AddComponent<UnitStatsComponent>(entity, std::move(unitStatsComponent));
    world.AddComponent<overworld::HighlightableComponent>(entity, std::make_unique<overworld::HighlightableComponent>());
    
    AddCollidableDataToUnit(entity);
    
    return entity;
}

///-----------------------------------------------------------------------------------------------
