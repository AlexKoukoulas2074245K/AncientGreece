///------------------------------------------------------------------------------------------------
///  PhysicsCollisionDetectionSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 24/07/2020.
///-----------------------------------------------------------------------------------------------

#include "PhysicsCollisionDetectionSystem.h"
#include "../components/CollidedEntitiesComponent.h"
#include "../../scene/components/SceneStateSingletonComponent.h"
#include "../../scene/scenegraphs/ISceneGraph.h"
#include "../../../engine/common/components/TransformComponent.h"

///-----------------------------------------------------------------------------------------------

namespace physics
{

///-----------------------------------------------------------------------------------------------

PhysicsCollisionDetectionSystem::PhysicsCollisionDetectionSystem()
{
}

///-----------------------------------------------------------------------------------------------

void PhysicsCollisionDetectionSystem::VUpdate(const float, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& sceneGraph = world.GetSingletonComponent<scene::SceneStateSingletonComponent>().mSceneGraph;
    
    for (const auto& entityId: entitiesToProcess)
    {
        const auto& physicsComponent = world.GetComponent<PhysicsComponent>(entityId);
        const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entityId);
        
        const auto& collisionCandidates = sceneGraph->VGetCollisionCandidates(entityId);
        for (const auto& collisionCandidateEntityId: collisionCandidates)
        {
            const auto& otherPhysicsComponent = world.GetComponent<PhysicsComponent>(collisionCandidateEntityId);
            const auto& otherTransformComponent = world.GetComponent<genesis::TransformComponent>(collisionCandidateEntityId);
            
            if (glm::distance(transformComponent.mPosition, otherTransformComponent.mPosition) < physicsComponent.mCollidableDimensions.x * 0.5f + otherPhysicsComponent.mCollidableDimensions.x * 0.5f)
            {
                auto collidedComponentEntity = world.CreateEntity();
                
                auto collidedComponent = std::make_unique<CollidedEntitiesComponent>();
                collidedComponent->mCollidedEntities = std::make_pair(entityId, collisionCandidateEntityId);
                
                world.AddComponent<CollidedEntitiesComponent>(collidedComponentEntity, std::move(collidedComponent));
            }
        }
    }
}

///-----------------------------------------------------------------------------------------------

}
