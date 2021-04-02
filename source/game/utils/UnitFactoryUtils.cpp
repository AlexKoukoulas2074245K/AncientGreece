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

namespace
{
    static const float ANIM_SPEED_RANDOM_MIN = 0.9f;
    static const float ANIM_SPEED_RANDOM_MAX = 1.1f;
}

///------------------------------------------------------------------------------------------------

genesis::ecs::EntityId CreateUnit(const StringId unitTypeName, const StringId unitName /* StringId() */, const StringId entityName /* StringId() */, const glm::vec3& position /* glm::vec3() */, const glm::vec3& rotation /* glm::vec3() */, const glm::vec3 scale /* glm::vec3() */)
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& unitBaseStats = GetUnitBaseStats(unitTypeName);
    
    auto entity = genesis::rendering::LoadAndCreateAnimatedModelByName(unitBaseStats.mUnitModelName.GetString(), position, rotation, scale * unitBaseStats.mUnitModelScaleFactor, entityName, genesis::math::RandomFloat(ANIM_SPEED_RANDOM_MIN, ANIM_SPEED_RANDOM_MAX));
    
    auto unitStatsComponent = std::make_unique<UnitStatsComponent>();
    unitStatsComponent->mStats = unitBaseStats;
    unitStatsComponent->mStats.mSpeedMultiplier = unitBaseStats.mSpeedMultiplier;
    unitStatsComponent->mStats.mDamage = unitBaseStats.mDamage;
    unitStatsComponent->mStats.mHealth = unitBaseStats.mHealth;
    
    if (unitName != StringId())
    {
        unitStatsComponent->mStats.mUnitName = unitName;
    }
    
    unitStatsComponent->mParty.push_back(unitStatsComponent->mStats);
    
    world.AddComponent<UnitStatsComponent>(entity, std::move(unitStatsComponent));
    world.AddComponent<overworld::HighlightableComponent>(entity, std::make_unique<overworld::HighlightableComponent>());
    
    AddCollidableDataToUnit(entity);
    
    return entity;
}

///-----------------------------------------------------------------------------------------------
