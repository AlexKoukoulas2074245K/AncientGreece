///------------------------------------------------------------------------------------------------
///  BattleMovementControllerSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 02/04/2021.
///-----------------------------------------------------------------------------------------------

#include "BattleMovementControllerSystem.h"
#include "../components/BattleTargetComponent.h"
#include "../utils/BattleUtils.h"
#include "../../components/UnitStatsComponent.h"
#include "../../../engine/animation/utils/AnimationUtils.h"
#include "../../../engine/common/components/NameComponent.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/common/utils/Logging.h"
#include "../../../engine/resources/MeshResource.h"

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const float ROTATION_SPEED  = 5.0f;
    static const float BASE_UNIT_SPEED = 0.004f;
}

///-----------------------------------------------------------------------------------------------

BattleMovementControllerSystem::BattleMovementControllerSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void BattleMovementControllerSystem::VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    auto& world = genesis::ecs::World::GetInstance();
    
    for (const auto entityId: entitiesToProcess)
    {
        const auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
        auto& battleTargetComponent = world.GetComponent<BattleTargetComponent>(entityId);
        auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entityId);
        const auto& targetTransformComponent = world.GetComponent<genesis::TransformComponent>(battleTargetComponent.mTargetEntity);
        
        const auto& vecToTarget = targetTransformComponent.mPosition - transformComponent.mPosition;
        
        // If we have arrived at target position
        if (AreUnitsInMeleeDistance(entityId, battleTargetComponent.mTargetEntity))
        {
            // Start Attack animation
            genesis::animation::ChangeAnimation(entityId, StringId("attacking"));
        }
        // Else move and rotate towards target
        else
        {
            const auto unitSpeed = BASE_UNIT_SPEED * unitStatsComponent.mStats.mSpeedMultiplier;
            UpdatePosition(dt, unitSpeed, targetTransformComponent.mPosition, transformComponent.mPosition);
            UpdateRotation(dt, -genesis::math::Arctan2(vecToTarget.x, vecToTarget.y), transformComponent.mRotation);
            
            // Start walking animation
            genesis::animation::ChangeAnimation(entityId, StringId("walking"));
        }
    }
}

///-----------------------------------------------------------------------------------------------

void BattleMovementControllerSystem::UpdatePosition(const float dt, const float speed, const glm::vec3& targetPosition, glm::vec3& entityPosition) const
{
    const auto& movementDirection = glm::normalize(targetPosition - entityPosition);
    entityPosition += movementDirection * speed * dt;
}

///-----------------------------------------------------------------------------------------------

void BattleMovementControllerSystem::UpdateRotation(const float dt, const float target, glm::vec3& entityRotation) const
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
