///------------------------------------------------------------------------------------------------
///  OverworldMovementControllerSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 12/03/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldMovementControllerSystem.h"
#include "../AreaTypes.h"
#include "../components/OverworldTargetComponent.h"
#include "../components/OverworldInteractionComponent.h"
#include "../utils/OverworldUtils.h"
#include "../../components/UnitStatsComponent.h"
#include "../../utils/UnitCollisionUtils.h"
#include "../../../engine/animation/utils/AnimationUtils.h"
#include "../../../engine/common/components/NameComponent.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/common/utils/Logging.h"
#include "../../../engine/rendering/components/RenderableComponent.h"
#include "../../../engine/rendering/utils/HeightMapUtils.h"
#include "../../../engine/rendering/utils/ParticleUtils.h"
#include "../../../engine/resources/ResourceLoadingService.h"

#include <tsl/robin_map.h>

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace
{
    const tsl::robin_map<AreaTypeMask, float> AREA_TYPE_TO_SPEED_MULTIPLIER =
    {
        { areaTypeMasks::FOREST, 0.5f },
        { areaTypeMasks::MOUNTAIN, 0.5f },
        { areaTypeMasks::HIGH_MOUNTAIN, 0.25f },
    };

    static const float SUFFICIENTLY_CLOSE_THRESHOLD           = 0.001f;
    static const float ROTATION_SPEED                         = 5.0f;
    static const float BASE_UNIT_SPEED                        = 0.006f;
    static const float UNIT_ASCENDING_DESCENDING_SPEED_FACTOR = 300.0f;
}

///-----------------------------------------------------------------------------------------------

OverworldMovementControllerSystem::OverworldMovementControllerSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void OverworldMovementControllerSystem::VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entities) const
{
    auto& world = genesis::ecs::World::GetInstance();
    auto mapEntity = GetMapEntity();
    const auto entitiesToProcess = entities;
    for (const auto entityId: entitiesToProcess)
    {
        const auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
        auto& targetComponent = world.GetComponent<OverworldTargetComponent>(entityId);
        auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entityId);
        const auto isFollowingEntity = targetComponent.mEntityTargetToFollow != genesis::ecs::NULL_ENTITY_ID && world.HasEntity(targetComponent.mEntityTargetToFollow);
        
        if (isFollowingEntity)
        {
            targetComponent.mTargetPosition = world.GetComponent<genesis::TransformComponent>(targetComponent.mEntityTargetToFollow).mPosition;
        }
        
        const auto& vecToTarget = targetComponent.mTargetPosition - transformComponent.mPosition;
        
        // If we have arrived at target position or collided with our target entity
        if (glm::length(vecToTarget) < SUFFICIENTLY_CLOSE_THRESHOLD || (isFollowingEntity && AreEntitiesColliding(entityId, targetComponent.mEntityTargetToFollow)))
        {
            // Create interaction component
            if (isFollowingEntity)
            {
                if (world.HasComponent<UnitStatsComponent>(targetComponent.mEntityTargetToFollow))
                {
                    auto& targetTransformComponent = world.GetComponent<genesis::TransformComponent>(targetComponent.mEntityTargetToFollow);
                    const auto vecToFollower = transformComponent.mPosition - targetTransformComponent.mPosition;
                    targetTransformComponent.mRotation.z = -genesis::math::Arctan2(vecToFollower.x, vecToFollower.y);
                }
                
                auto interactionComponent = std::make_unique<OverworldInteractionComponent>();
                interactionComponent->mInteraction.mInstigatorEntityId = entityId;
                interactionComponent->mInteraction.mInstigatorUnitName = world.GetComponent<UnitStatsComponent>(entityId).mStats.mUnitName;
                interactionComponent->mInteraction.mOtherEntityId = targetComponent.mEntityTargetToFollow;
                interactionComponent->mInteraction.mOtherUnitName = world.HasComponent<UnitStatsComponent>(targetComponent.mEntityTargetToFollow) ? world.GetComponent<UnitStatsComponent>(targetComponent.mEntityTargetToFollow).mStats.mUnitName : world.GetComponent<genesis::NameComponent>(targetComponent.mEntityTargetToFollow).mName;
                world.AddComponent<OverworldInteractionComponent>(world.CreateEntity(), std::move(interactionComponent));
            }
            
            // Start Idle animation
            genesis::animation::ChangeAnimation(entityId, StringId("idle"));
            world.RemoveComponent<OverworldTargetComponent>(entityId);
        }
        // Else move and rotate towards target
        else
        {
            const auto terrainSpeedMultiplier = GetTerrainSpeedMultiplierAtPosition(transformComponent.mPosition);
            const auto unitSpeed = BASE_UNIT_SPEED * unitStatsComponent.mStats.mSpeedMultiplier * terrainSpeedMultiplier;
            UpdatePosition(mapEntity, dt, unitSpeed, targetComponent.mTargetPosition, transformComponent.mPosition);
            UpdateRotation(dt, -genesis::math::Arctan2(vecToTarget.x, vecToTarget.y), transformComponent.mRotation);
            
            // Start walking animation
            genesis::animation::ChangeAnimation(entityId, StringId("walking"));
        }
    }
}

///-----------------------------------------------------------------------------------------------

void OverworldMovementControllerSystem::UpdatePosition(const genesis::ecs::EntityId mapEntity, const float dt, const float speed, const glm::vec3& targetPosition, glm::vec3& entityPosition) const
{
    const auto& movementDirection = glm::normalize(targetPosition - entityPosition);
    entityPosition.x += movementDirection.x * speed * dt;
    entityPosition.y += movementDirection.y * speed * dt;
    entityPosition.z += (genesis::rendering::GetTerrainHeightAtPosition(mapEntity, entityPosition) - entityPosition.z) * UNIT_ASCENDING_DESCENDING_SPEED_FACTOR * speed * dt;
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
