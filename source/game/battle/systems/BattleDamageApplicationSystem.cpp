///------------------------------------------------------------------------------------------------
///  BattleDamageApplicationSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 07/04/2021.
///-----------------------------------------------------------------------------------------------

#include "BattleDamageApplicationSystem.h"
#include "../components/BattleDamageComponent.h"
#include "../components/BattleDestructionTimerComponent.h"
#include "../../components/UnitStatsComponent.h"
#include "../../utils/UnitInfoUtils.h"
#include "../../../engine/rendering/components/RenderableComponent.h"

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const StringId DAMAGED_EFFECT_UNIFORM_NAME = StringId("damaged_effect");
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
        auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(entityId);
        auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
        
        unitStatsComponent.mStats.mHealth -= damageComponent.mDamage;
        if (unitStatsComponent.mStats.mHealth <= 0)
        {
            auto destructionTimerComponent = std::make_unique<BattleDestructionTimerComponent>();
            destructionTimerComponent->mDestructionTimer = 1.0f;
            world.AddComponent<BattleDestructionTimerComponent>(entityId, std::move(destructionTimerComponent));
        }
        
        renderableComponent.mShaderUniforms.mShaderBoolUniforms[DAMAGED_EFFECT_UNIFORM_NAME] = !IsUnitDead(entityId);
        
        world.RemoveComponent<BattleDamageComponent>(entityId);
    }
}

///-----------------------------------------------------------------------------------------------

}
