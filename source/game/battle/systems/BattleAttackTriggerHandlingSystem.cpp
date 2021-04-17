///------------------------------------------------------------------------------------------------
///  BattleAttackTriggerHandlingSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 05/04/2021.
///-----------------------------------------------------------------------------------------------

#include "BattleAttackTriggerHandlingSystem.h"
#include "../components/BattleAttackCooldownComponent.h"
#include "../components/BattleProjectileComponent.h"
#include "../components/BattleSideComponent.h"
#include "../components/BattleTargetComponent.h"
#include "../utils/BattleUtils.h"
#include "../../components/CollidableComponent.h"
#include "../../components/UnitStatsComponent.h"
#include "../../../engine/animation/utils/AnimationUtils.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/common/utils/Logging.h"
#include "../../../engine/rendering/components/RenderableComponent.h"
#include "../../../engine/rendering/utils/MeshUtils.h"
#include "../../../engine/resources/ResourceLoadingService.h"
#include "../../../engine/resources/MeshResource.h"

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const StringId DAMAGED_EFFECT_UNIFORM_NAME = StringId("damaged_effect");
    static const std::string PROJECTILE_MODEL_NAME = "arrow";
    static const glm::vec3 PROJECTILE_SCALE = glm::vec3(0.002f, 0.002f, 0.002f);
}

///-----------------------------------------------------------------------------------------------

BattleAttackTriggerHandlingSystem::BattleAttackTriggerHandlingSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void BattleAttackTriggerHandlingSystem::VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    if (IsBattleFinished()) return;
    
    auto& world = genesis::ecs::World::GetInstance();
    for (const auto& entityId: entitiesToProcess)
    {
        auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(entityId);
        renderableComponent.mShaderUniforms.mShaderBoolUniforms[DAMAGED_EFFECT_UNIFORM_NAME] = false;
        
        if (genesis::animation::GetCurrentAnimationName(entityId) != StringId("attacking"))
        {
            continue;
        }
        
        const auto& currentMesh = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::MeshResource>(renderableComponent.mMeshResourceIds[renderableComponent.mCurrentMeshResourceIndex]);
        const auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
        
        
        if (world.HasComponent<BattleAttackCooldownComponent>(entityId))
        {
            auto& attackCooldownComponent = world.GetComponent<BattleAttackCooldownComponent>(entityId);
            attackCooldownComponent.mAttackCooldown -= dt;
            if (attackCooldownComponent.mAttackCooldown < 0.0f)
            {
                world.RemoveComponent<BattleAttackCooldownComponent>(entityId);
            }
        }
        else
        {
            const auto attackTriggerPoint = unitStatsComponent.mStats.mAttackAnimationDamageTrigger;
            const auto currentAnimationPercentage = std::fmod(renderableComponent.mAnimationTimeAccum, currentMesh.GetAnimationInfo().mDuration)/currentMesh.GetAnimationInfo().mDuration;
            
            if (currentAnimationPercentage > attackTriggerPoint)
            {
                auto attackCooldownComponent = std::make_unique<BattleAttackCooldownComponent>();
                attackCooldownComponent->mAttackCooldown = renderableComponent.mAnimationSpeed;
                world.AddComponent<BattleAttackCooldownComponent>(entityId, std::move(attackCooldownComponent));
                
                if (unitStatsComponent.mStats.mIsRangedUnit)
                {
                    CreateProjectile(entityId);
                }
                else
                {
                    DamageTarget(entityId);
                }
            }
        }
    }
}

///-----------------------------------------------------------------------------------------------

void BattleAttackTriggerHandlingSystem::CreateProjectile(const genesis::ecs::EntityId sourceEntityId) const
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(sourceEntityId);
    const auto& battleSideComponent = world.GetComponent<BattleSideComponent>(sourceEntityId);
    const auto& battleTargetComponent = world.GetComponent<BattleTargetComponent>(sourceEntityId);
    const auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(sourceEntityId);
    const auto& currentMesh = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::MeshResource>(renderableComponent.mMeshResourceIds[renderableComponent.mCurrentMeshResourceIndex]);
    const auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(sourceEntityId);
    
    const auto& targetTransformComponent = world.GetComponent<genesis::TransformComponent>(battleTargetComponent.mTargetEntity);
    const auto& targetRenderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(battleTargetComponent.mTargetEntity);
    const auto& targetCurrentMesh = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::MeshResource>(targetRenderableComponent.mMeshResourceIds[targetRenderableComponent.mCurrentMeshResourceIndex]);
    const auto scaledTargetDimensions = targetCurrentMesh.GetDimensions() * targetTransformComponent.mScale;
    
    const auto offsetTargetPosition = CalculateProjectileTargetOffset(scaledTargetDimensions);
    const auto offsetOriginPosition = CalculateProjectileOriginOffset(currentMesh.GetDimensions() * transformComponent.mScale, targetTransformComponent.mPosition + offsetTargetPosition);
    const auto originPosition = glm::vec3(transformComponent.mPosition.x + offsetOriginPosition.x, transformComponent.mPosition.y + offsetOriginPosition.y, offsetOriginPosition.z);
    const auto vecToTarget = (targetTransformComponent.mPosition + offsetTargetPosition) - transformComponent.mPosition;
    
    const auto projectileRotation = CalculateProjectileRotation(vecToTarget);
    
    auto arrowEntity = genesis::rendering::LoadAndCreateStaticModelByName(PROJECTILE_MODEL_NAME, originPosition, projectileRotation, PROJECTILE_SCALE, GetProjectileEntityName());
    
    auto arrowBattleSideComponent = std::make_unique<BattleSideComponent>();
    arrowBattleSideComponent->mBattleSideLeaderUnitName = battleSideComponent.mBattleSideLeaderUnitName;
    world.AddComponent<BattleSideComponent>(arrowEntity, std::move(arrowBattleSideComponent));
    
    auto arrowProjectileComponent = std::make_unique<BattleProjectileComponent>();
    arrowProjectileComponent->mDamage = unitStatsComponent.mStats.mDamage;
    arrowProjectileComponent->mState = ProjectileState::SEEKING_TARGET;
    arrowProjectileComponent->mOffsetTargetPosition = offsetTargetPosition;
    
    world.AddComponent<BattleProjectileComponent>(arrowEntity, std::move(arrowProjectileComponent));
    
    auto arrowBattleTargetComponent = std::make_unique<BattleTargetComponent>();
    arrowBattleTargetComponent->mTargetEntity = battleTargetComponent.mTargetEntity;
    world.AddComponent<BattleTargetComponent>(arrowEntity, std::move(arrowBattleTargetComponent));
    
    AddCollidableDataToArrow(arrowEntity);
}

///-----------------------------------------------------------------------------------------------

void BattleAttackTriggerHandlingSystem::DamageTarget(const genesis::ecs::EntityId sourceEntityId) const
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& battleTargetComponent = world.GetComponent<BattleTargetComponent>(sourceEntityId);
    const auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(sourceEntityId);
    
    DamageUnit(battleTargetComponent.mTargetEntity, unitStatsComponent.mStats.mDamage);
}

///-----------------------------------------------------------------------------------------------

glm::vec3 BattleAttackTriggerHandlingSystem::CalculateProjectileTargetOffset(const glm::vec3& targetEntityScaledDimensions) const
{
    return glm::vec3( genesis::math::RandomFloat(-targetEntityScaledDimensions.x/4.0f, targetEntityScaledDimensions.x/4.0f), 0.0f, -genesis::math::RandomFloat(targetEntityScaledDimensions.y * 0.15f, targetEntityScaledDimensions.y * 0.55f));
}

///-----------------------------------------------------------------------------------------------

glm::vec3 BattleAttackTriggerHandlingSystem::CalculateProjectileOriginOffset(const glm::vec3& sourceEntityScaledDimensions, const glm::vec3& targetPosition) const
{
    return glm::vec3(0.0f, sourceEntityScaledDimensions.y / 2.0f, targetPosition.z);
}

///-----------------------------------------------------------------------------------------------

glm::vec3 BattleAttackTriggerHandlingSystem::CalculateProjectileRotation(const glm::vec3& vecToTarget) const
{
    auto pitch = genesis::math::Arctan2(vecToTarget.z, vecToTarget.y);
    if (vecToTarget.y < 0) pitch += genesis::math::PI;
    return glm::vec3(pitch, 0.0f, -genesis::math::Arctan2(vecToTarget.x, vecToTarget.y));
}

///-----------------------------------------------------------------------------------------------

}
