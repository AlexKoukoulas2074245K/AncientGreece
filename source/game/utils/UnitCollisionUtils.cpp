///------------------------------------------------------------------------------------------------
///  UnitCollisionUtils.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///------------------------------------------------------------------------------------------------

#include "UnitCollisionUtils.h"
#include "../components/CollidableComponent.h"
#include "../../engine/common/components/TransformComponent.h"
#include "../../engine/common/utils/MathUtils.h"
#include "../../engine/rendering/components/RenderableComponent.h"
#include "../../engine/resources/ResourceLoadingService.h"
#include "../../engine/resources/MeshResource.h"

///------------------------------------------------------------------------------------------------

namespace
{
    static const float ENTITY_SPHERE_COLLISION_MULTIPLIER = 0.5f * 0.3333f;
}

///------------------------------------------------------------------------------------------------

void AddCollidableDataToUnit(const genesis::ecs::EntityId unitEntity)
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& entityTransformComponent =
    world.GetComponent<genesis::TransformComponent>(unitEntity);
    const auto& entityRenderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(unitEntity);
    const auto& entityMeshResource = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::MeshResource>( entityRenderableComponent.mMeshResourceIds[entityRenderableComponent.mCurrentMeshResourceIndex]);
    const auto entityScaledDimensions = entityMeshResource.GetDimensions() * entityTransformComponent.mScale;
    const auto entitySphereRadius = (entityScaledDimensions.x + entityScaledDimensions.y + entityScaledDimensions.z) * ENTITY_SPHERE_COLLISION_MULTIPLIER;
    
    auto collidableComponent = std::make_unique<CollidableComponent>();
    collidableComponent->mCollidableDimensions.x = collidableComponent->mCollidableDimensions.y = collidableComponent->mCollidableDimensions.z = entitySphereRadius;
    
    world.AddComponent<CollidableComponent>(unitEntity, std::move(collidableComponent));
}

///------------------------------------------------------------------------------------------------

bool AreEntitiesColliding(const genesis::ecs::EntityId entityA, const genesis::ecs::EntityId entityB)
{
    const auto& world = genesis::ecs::World::GetInstance();
    
    const auto& entityATransformComponent = world.GetComponent<genesis::TransformComponent>(entityA);
    const auto& entityBTransformComponent = world.GetComponent<genesis::TransformComponent>(entityB);
    
    const auto& entityACollidableComponent = world.GetComponent<CollidableComponent>(entityA);
    const auto& entityBCollidableComponent = world.GetComponent<CollidableComponent>(entityB);
    
    return genesis::math::SphereToSphereIntersection(entityATransformComponent.mPosition, entityACollidableComponent.mCollidableDimensions.x, entityBTransformComponent.mPosition, entityBCollidableComponent.mCollidableDimensions.x);
}

///-----------------------------------------------------------------------------------------------

bool AreEntitiesColliding(const genesis::ecs::EntityId entityA, const genesis::ecs::EntityId entityB, float& penetration)
{
    const auto& world = genesis::ecs::World::GetInstance();
    
    const auto& entityATransformComponent = world.GetComponent<genesis::TransformComponent>(entityA);
    const auto& entityBTransformComponent = world.GetComponent<genesis::TransformComponent>(entityB);
    
    const auto& entityACollidableComponent = world.GetComponent<CollidableComponent>(entityA);
    const auto& entityBCollidableComponent = world.GetComponent<CollidableComponent>(entityB);
    
    return genesis::math::SphereToSphereIntersection(entityATransformComponent.mPosition, entityACollidableComponent.mCollidableDimensions.x, entityBTransformComponent.mPosition, entityBCollidableComponent.mCollidableDimensions.x, penetration);
}

///-----------------------------------------------------------------------------------------------

