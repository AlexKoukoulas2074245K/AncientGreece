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
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/rendering/components/RenderableComponent.h"
#include "../../../engine/rendering/utils/ParticleUtils.h"
#include "../../../engine/resources/ResourceLoadingService.h"
#include "../../../engine/resources/MeshResource.h"

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
        
        AddBloodDropsToUnit(entityId);
        
        world.RemoveComponent<BattleDamageComponent>(entityId);
    }
}

///-----------------------------------------------------------------------------------------------

void  BattleDamageApplicationSystem::AddBloodDropsToUnit(const genesis::ecs::EntityId unitEntityId) const
{
    const auto& world = genesis::ecs::World::GetInstance();
    const auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(unitEntityId);
    const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(unitEntityId);
    
    const auto& currentMesh = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::MeshResource>(renderableComponent.mMeshResourceIds[renderableComponent.mCurrentMeshResourceIndex]);
    
    const auto scaledEntityHeight = currentMesh.GetDimensions().z * transformComponent.mScale.z;
    
    if (!world.HasComponent<genesis::rendering::ParticleEmitterComponent>(unitEntityId))
    {
        genesis::rendering::AddParticleEmitter(genesis::rendering::ParticleEmitterType::BLOOD_DROP, transformComponent.mPosition + glm::vec3(0.0f, 0.0f, -scaledEntityHeight/2.0f), glm::vec2(0.5f, 1.0f), glm::vec2(-0.01f, 0.01f), glm::vec2(-0.01, 0.01f), glm::vec2(0.001f, 0.001f), 100, false, unitEntityId);
    }
    else
    {
        genesis::rendering::SpawnParticlesAtFirstAvailableSlot(5, world.GetComponent<genesis::rendering::ParticleEmitterComponent>(unitEntityId));
    }
}

///-----------------------------------------------------------------------------------------------

}
