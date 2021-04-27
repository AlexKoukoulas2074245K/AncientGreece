///------------------------------------------------------------------------------------------------
///  OverworldShipTogglingSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 27/04/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldShipTogglingSystem.h"
#include "../utils/OverworldUtils.h"
#include "../../components/UnitStatsComponent.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/rendering/components/ParticleEmitterComponent.h"
#include "../../../engine/rendering/components/RenderableComponent.h"
#include "../../../engine/rendering/utils/ParticleUtils.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const float SHIP_TOGGLING_HEIGHT = -0.0014f;
    static const float SHIP_Z_OFFSET = -0.01f;
}

///-----------------------------------------------------------------------------------------------

OverworldShipTogglingSystem::OverworldShipTogglingSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void OverworldShipTogglingSystem::VUpdate(const float, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    auto& world = genesis::ecs::World::GetInstance();
    
    for (const auto entityId: entitiesToProcess)
    {
        const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entityId);
        const auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
        auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(entityId);
        
        auto shipEntity = world.FindEntityWithName(GetShipEntityNameFromUnitName(unitStatsComponent.mStats.mUnitName));
        auto& shipTransformComponent = world.GetComponent<genesis::TransformComponent>(shipEntity);
        auto& shipRenderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(shipEntity);
        
        // Toggle between ship and unit visibilities
        if (transformComponent.mPosition.z >= SHIP_TOGGLING_HEIGHT)
        {
            // Play particle effect on transition
            if (renderableComponent.mIsVisible)
            {
                if (world.HasComponent<genesis::rendering::ParticleEmitterComponent>(entityId))
                {
                    world.RemoveComponent<genesis::rendering::ParticleEmitterComponent>(entityId);
                }
                    
                CreateSmokeRevealParticleEffectForEntity(entityId);
            }
            
            renderableComponent.mIsVisible = false;
            shipRenderableComponent.mIsVisible = true;
            
            shipTransformComponent.mPosition = transformComponent.mPosition;
            shipTransformComponent.mPosition.z += SHIP_Z_OFFSET;
            
            shipTransformComponent.mRotation = transformComponent.mRotation;
            shipTransformComponent.mRotation.x -= genesis::math::PI/2.0f;
            shipTransformComponent.mRotation.z += genesis::math::PI/2.0f;
        }
        else
        {
            // Play particle effect on transition
            if (!renderableComponent.mIsVisible)
            {
                if (world.HasComponent<genesis::rendering::ParticleEmitterComponent>(entityId))
                {
                    world.RemoveComponent<genesis::rendering::ParticleEmitterComponent>(entityId);
                }
                    
                CreateSmokeRevealParticleEffectForEntity(entityId);
            }
            
            
            renderableComponent.mIsVisible = true;
            shipRenderableComponent.mIsVisible = false;
        }
    }
}

///-----------------------------------------------------------------------------------------------

void OverworldShipTogglingSystem::CreateSmokeRevealParticleEffectForEntity(const genesis::ecs::EntityId entityId) const
{
    const auto& world = genesis::ecs::World::GetInstance();
    const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entityId);
    
    genesis::rendering::AddParticleEmitter(genesis::rendering::ParticleEmitterType::SMOKE_REVEAL, transformComponent.mPosition + glm::vec3(-0.01f, -0.01f, -0.02f), glm::vec2(0.4f, 0.8f), glm::vec2(-0.01f, 0.01f), glm::vec2(-0.01f, 0.01f), glm::vec2(-0.01f, 0.01f), glm::vec2(0.02f), 100, true, entityId);
}

///-----------------------------------------------------------------------------------------------

}
