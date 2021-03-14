///------------------------------------------------------------------------------------------------
///  OverworldMovementControllerSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 12/03/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldMovementControllerSystem.h"
#include "../components/OverworldWaypointTargetComponent.h"
#include "../../../engine/animation/utils/AnimationUtils.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/common/utils/Logging.h"
#include "../../../engine/rendering/components/RenderableComponent.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const float SUFFICIENTLY_CLOSE_THRESHOLD = 0.001f;
    static const float ROTATION_SPEED = 5.0f;
    static const float speed = 0.005f;
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
    
    for (const auto& entityId: entitiesToProcess)
    {
        const auto& waypointComponent = world.GetComponent<OverworldWaypointTargetComponent>(entityId);
        auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entityId);
        
        const auto& vecToWaypoint = waypointComponent.mTargetPosition - transformComponent.mPosition;
        
        // If we have arrived at target position
        if (glm::length(vecToWaypoint) < SUFFICIENTLY_CLOSE_THRESHOLD)
        {
            // Start Idle animation
            genesis::animation::ChangeAnimation(entityId, StringId("idle"));
            
            world.RemoveComponent<OverworldWaypointTargetComponent>(entityId);
        }
        // Else move and rotate towards target
        else
        {
            UpdatePosition(dt, speed, waypointComponent.mTargetPosition, transformComponent.mPosition);
            UpdateRotation(dt, -genesis::math::Arctan2(vecToWaypoint.x, vecToWaypoint.y), transformComponent.mRotation);
            
            // Start walking animation
            genesis::animation::ChangeAnimation(entityId, StringId("walking"));
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
    
    if (genesis::math::Abs(targetRotation - entityRotation.z) > SUFFICIENTLY_CLOSE_THRESHOLD)
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
