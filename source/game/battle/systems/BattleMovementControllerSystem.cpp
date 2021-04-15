///------------------------------------------------------------------------------------------------
///  BattleMovementControllerSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 02/04/2021.
///-----------------------------------------------------------------------------------------------

#include "BattleMovementControllerSystem.h"
#include "../components/BattleDestructionTimerComponent.h"
#include "../components/BattleProjectileComponent.h"
#include "../components/BattleTargetComponent.h"
#include "../utils/BattleUtils.h"
#include "../../components/UnitStatsComponent.h"
#include "../../utils/UnitInfoUtils.h"
#include "../../../engine/animation/utils/AnimationUtils.h"
#include "../../../engine/common/components/NameComponent.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/common/utils/Logging.h"
#include "../../../engine/rendering/utils/HeightMapUtils.h"
#include "../../../engine/resources/MeshResource.h"

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const float ROTATION_SPEED                         = 5.0f;
    static const float BASE_UNIT_SPEED                        = 0.01f;
	static const float BASE_PROJECTILE_SPEED                  = 0.2f;
    static const float PROJECTILE_TARGET_DISTANCE_THRESHOLD   = 0.001f;
    static const float PROJECTILE_PITCH_DOWN_SPEED            = 0.2f;
    static const float PROJECTILE_TTL_ON_ATTACHMENT           = 1.0f;
    static const float UNIT_ASCENDING_DESCENDING_SPEED_FACTOR = 300.0f;
}

///-----------------------------------------------------------------------------------------------

BattleMovementControllerSystem::BattleMovementControllerSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void BattleMovementControllerSystem::VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    if (IsBattleFinished()) return;
    
    auto& world = genesis::ecs::World::GetInstance();
    auto mapEntity = GetMapEntity();
    
    for (const auto entityId: entitiesToProcess)
    {
        const auto isProjectile = world.HasComponent<BattleProjectileComponent>(entityId);
        if (isProjectile)
        {
            UpdateProjectile(dt, entityId);
        }
        else
        {
            UpdateUnit(mapEntity, dt, entityId);
        }
    }
}

///-----------------------------------------------------------------------------------------------

void BattleMovementControllerSystem::UpdateProjectile(const float dt, const genesis::ecs::EntityId entityId) const
{
    auto& world = genesis::ecs::World::GetInstance();

    const auto& battleTargetComponent = world.GetComponent<BattleTargetComponent>(entityId);
    auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entityId);

    if (!world.HasEntity(battleTargetComponent.mTargetEntity))
    {
        world.DestroyEntity(entityId);
        return;
    }
    
    const auto& targetTransformComponent = world.GetComponent<genesis::TransformComponent>(battleTargetComponent.mTargetEntity);
    auto& projectileComponent = world.GetComponent<BattleProjectileComponent>(entityId);
    
    const auto targetPoint = targetTransformComponent.mPosition + projectileComponent.mOffsetTargetPosition;
    
    switch (projectileComponent.mState)
    {
        case ProjectileState::SEEKING_TARGET:
        {
            const auto& vecToTarget3 = targetPoint - transformComponent.mPosition;
            
            if (genesis::math::Abs(targetPoint.y - transformComponent.mPosition.y) > PROJECTILE_TARGET_DISTANCE_THRESHOLD)
            {
                const auto unitSpeed = BASE_PROJECTILE_SPEED;
                UpdatePosition(genesis::ecs::NULL_ENTITY_ID, dt, unitSpeed, targetPoint, transformComponent.mPosition);
                UpdateRotation(dt, -genesis::math::Arctan2(vecToTarget3.x, vecToTarget3.y), transformComponent.mRotation);
                UpdateProjectilePitch(dt, transformComponent.mRotation);
            }
            else
            {
                projectileComponent.mState = ProjectileState::ATTACHED_TO_TARGET;
                projectileComponent.mTargetAttachmentDelta = targetPoint - transformComponent.mPosition;
                
                // Create self destruct component on projectile
                auto destructionTimerComponent = std::make_unique<BattleDestructionTimerComponent>();
                destructionTimerComponent->mDestructionTimer = PROJECTILE_TTL_ON_ATTACHMENT;
                world.AddComponent<BattleDestructionTimerComponent>(entityId, std::move(destructionTimerComponent));
                
                DamageUnit(battleTargetComponent.mTargetEntity, projectileComponent.mDamage);
            }
        } break;
            
        case ProjectileState::ATTACHED_TO_TARGET:
        {
            transformComponent.mPosition = targetPoint - projectileComponent.mTargetAttachmentDelta;
        } break;
    }
}

///-----------------------------------------------------------------------------------------------

void BattleMovementControllerSystem::UpdateUnit(const genesis::ecs::EntityId mapEntity, const float dt, const genesis::ecs::EntityId entityId) const
{
    auto& world = genesis::ecs::World::GetInstance();
    
    if (IsUnitDead(entityId)) return;
    
    const auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
    const auto& battleTargetComponent = world.GetComponent<BattleTargetComponent>(entityId);
    auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entityId);

    const auto& targetTransformComponent = world.GetComponent<genesis::TransformComponent>(battleTargetComponent.mTargetEntity);
    
    const auto& vecToTarget = targetTransformComponent.mPosition - transformComponent.mPosition;
    const auto distanceToTarget = glm::length(vecToTarget);
    
    UpdateRotation(dt, -genesis::math::Arctan2(vecToTarget.x, vecToTarget.y), transformComponent.mRotation);
    
    // If we have arrived at target position, or current unit is range and is close enough to attack
    if (AreUnitsInMeleeDistance(entityId, battleTargetComponent.mTargetEntity) || (unitStatsComponent.mStats.mIsRangedUnit && unitStatsComponent.mStats.mAttackRange >= distanceToTarget))
    {
        // Start Attack animation
        genesis::animation::ChangeAnimation(entityId, StringId("attacking"));
    }
    // Else move and rotate towards target
    else
    {
        const auto unitSpeed = BASE_UNIT_SPEED * unitStatsComponent.mStats.mSpeedMultiplier;
        UpdatePosition(mapEntity, dt, unitSpeed, targetTransformComponent.mPosition, transformComponent.mPosition);
        
        // Start walking animation
        if (!AreUnitsInDoubleMeleeDistance(entityId, battleTargetComponent.mTargetEntity) && (!unitStatsComponent.mStats.mIsRangedUnit || genesis::animation::GetCurrentAnimationName(entityId) != StringId("attacking")))
        {
            genesis::animation::ChangeAnimation(entityId, StringId("walking"));
        }
    }
}

///-----------------------------------------------------------------------------------------------

void BattleMovementControllerSystem::UpdatePosition(const genesis::ecs::EntityId mapEntity, const float dt, const float speed, const glm::vec3& targetPosition, glm::vec3& entityPosition) const
{
    const auto& movementDirection = glm::normalize(targetPosition - entityPosition);
    entityPosition.x += movementDirection.x * speed * dt;
    entityPosition.y += movementDirection.y * speed * dt;
    
    if (mapEntity != genesis::ecs::NULL_ENTITY_ID)
    {
        entityPosition.z += (genesis::rendering::GetTerrainHeightAtPosition(mapEntity, entityPosition) - entityPosition.z) * UNIT_ASCENDING_DESCENDING_SPEED_FACTOR * speed * dt;
    }
}

///-----------------------------------------------------------------------------------------------

void BattleMovementControllerSystem::UpdateProjectilePitch(const float dt, glm::vec3& entityRotation) const
{
    entityRotation.x += PROJECTILE_PITCH_DOWN_SPEED * dt;
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
