///------------------------------------------------------------------------------------------------
///  OverworldMovementControllerSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 12/03/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldMovementControllerSystem.h"
#include "../components/OverworldTargetComponent.h"
#include "../utils/Pathfinding.h"
#include "../../components/UnitStatsComponent.h"
#include "../../../engine/animation/utils/AnimationUtils.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/common/utils/Logging.h"
#include "../../../engine/rendering/components/RenderableComponent.h"
#include "../../../engine/resources/ResourceLoadingService.h"
#include "../../../engine/resources/TextureResource.h"
#include "../../../engine/resources/MeshResource.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const float SUFFICIENTLY_CLOSE_THRESHOLD = 0.001f;
    static const float ROTATION_SPEED               = 5.0f;
    static const float BASE_UNIT_SPEED              = 0.002f;
}

///-----------------------------------------------------------------------------------------------

OverworldMovementControllerSystem::OverworldMovementControllerSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void OverworldMovementControllerSystem::VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    auto& world = genesis::ecs::World::GetInstance();

    for (const auto entityId: entitiesToProcess)
    {
        const auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
        auto& targetComponent = world.GetComponent<OverworldTargetComponent>(entityId);
        auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entityId);
        
        if (targetComponent.mOptionalEntityTarget != genesis::ecs::NULL_ENTITY_ID && world.HasEntity(targetComponent.mOptionalEntityTarget))
        {
            targetComponent.mTargetPositionPath.push_back( world.GetComponent<genesis::TransformComponent>(targetComponent.mOptionalEntityTarget).mPosition);
        }
        
        // If we have arrived at target position
        if (targetComponent.mTargetPositionPath.size() == 0)
        {
            // Start Idle animation
            genesis::animation::ChangeAnimation(entityId, StringId("idle"));
            world.RemoveComponent<OverworldTargetComponent>(entityId);
        }
        // Else move and rotate towards target
        else
        {
            auto currentTargetPos = targetComponent.mTargetPositionPath.front();
            auto vecToWaypoint = currentTargetPos - transformComponent.mPosition;
            
            const auto unitSpeed = BASE_UNIT_SPEED * unitStatsComponent.mSpeedMultiplier;
            UpdatePosition(dt, unitSpeed, currentTargetPos, transformComponent.mPosition);
            //UpdateRotation(dt, -genesis::math::Arctan2(vecToWaypoint.x, vecToWaypoint.y), transformComponent.mRotation);
            
            // Start walking animation
            genesis::animation::ChangeAnimation(entityId, StringId("walking"));
            
            // Check for moving to the next position path step
            if (glm::length(vecToWaypoint) < SUFFICIENTLY_CLOSE_THRESHOLD)
            {
                targetComponent.mTargetPositionPath.pop_front();
            }
        }
    }
}

///-----------------------------------------------------------------------------------------------

void OverworldMovementControllerSystem::UpdatePosition(const float dt, const float speed, const glm::vec3& targetPosition, glm::vec3& entityPosition) const
{
    const auto& movementDirection = glm::normalize(targetPosition - entityPosition);
    entityPosition += movementDirection * speed * dt;
}

///-----------------------------------------------------------------------------------------------

void OverworldMovementControllerSystem::UpdateRotation(const float dt, const float target, glm::vec3& entityRotation) const
{
    auto targetRotation = target;
    while (entityRotation.z - targetRotation > genesis::math::PI) targetRotation += genesis::math::PI * 2.0f;
    while (targetRotation - entityRotation.z > genesis::math::PI) entityRotation.z += genesis::math::PI * 2.0f;
    
    if (genesis::math::Abs(targetRotation - entityRotation.z) > ROTATION_SPEED * dt)
    {
        entityRotation.z = targetRotation > entityRotation.z ? entityRotation.z + ROTATION_SPEED * dt : entityRotation.z - ROTATION_SPEED * dt;
        
        if (entityRotation.z > genesis::math::PI * 2.0f)
        {
            entityRotation.z -= genesis::math::PI * 2.0f;
        }
        else if (entityRotation.z < -genesis::math::PI * 2.0f)
        {
            entityRotation.z += genesis::math::PI * 2.0f;
        }
    }
}

///-----------------------------------------------------------------------------------------------

}
