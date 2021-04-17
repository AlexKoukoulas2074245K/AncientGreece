///------------------------------------------------------------------------------------------------
///  BattleDamageApplicationSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 07/04/2021.
///-----------------------------------------------------------------------------------------------

#include "BattleDamageApplicationSystem.h"
#include "../components/BattleDamageComponent.h"
#include "../components/BattleDestructionTimerComponent.h"
#include "../components/BattleSideComponent.h"
#include "../utils/BattleUtils.h"
#include "../../components/CollidableComponent.h"
#include "../../components/UnitStatsComponent.h"
#include "../../utils/UnitInfoUtils.h"
#include "../../../engine/animation/utils/AnimationUtils.h"
#include "../../../engine/rendering/components/RenderableComponent.h"

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const StringId DAMAGED_EFFECT_UNIFORM_NAME = StringId("damaged_effect");
    static const float DEAD_UNIT_TTL = 5.0f;
}

///-----------------------------------------------------------------------------------------------

BattleDamageApplicationSystem::BattleDamageApplicationSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void BattleDamageApplicationSystem::VUpdate(const float, const std::vector<genesis::ecs::EntityId>& entities) const
{
    auto& world = genesis::ecs::World::GetInstance();
    
    const auto entitiesToProcess = entities;
    for (const auto& entityId: entitiesToProcess)
    {
        const auto& damageComponent = world.GetComponent<BattleDamageComponent>(entityId);
        const auto& battleSideComponent = world.GetComponent<BattleSideComponent>(entityId);
        
        auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(entityId);
        auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
        
        unitStatsComponent.mStats.mHealth -= damageComponent.mDamage;
        if (unitStatsComponent.mStats.mHealth <= 0)
        {
            // Do not add leader as casualty
            if (unitStatsComponent.mStats.mUnitName != battleSideComponent.mBattleSideLeaderUnitName)
            {
                AddBattleCasualty(unitStatsComponent.mStats.mUnitType, battleSideComponent.mBattleSideLeaderUnitName);
            }
            
            genesis::animation::ChangeAnimation(entityId, StringId("dying"), false);
            auto destructionTimerComponent = std::make_unique<BattleDestructionTimerComponent>();
            destructionTimerComponent->mDestructionTimer = DEAD_UNIT_TTL;
            world.AddComponent<BattleDestructionTimerComponent>(entityId, std::move(destructionTimerComponent));
            world.RemoveComponent<CollidableComponent>(entityId);
        }
        
        renderableComponent.mShaderUniforms.mShaderBoolUniforms[DAMAGED_EFFECT_UNIFORM_NAME] = !IsUnitDead(entityId);
        
        world.RemoveComponent<BattleDamageComponent>(entityId);
    }
}

///-----------------------------------------------------------------------------------------------

}
