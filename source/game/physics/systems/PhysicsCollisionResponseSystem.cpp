///------------------------------------------------------------------------------------------------
///  PhysicsCollisionResponseSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 24/07/2020.
///-----------------------------------------------------------------------------------------------

#include "PhysicsCollisionResponseSystem.h"
#include "../components/PhysicsComponent.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/common/utils/Logging.h"

///-----------------------------------------------------------------------------------------------

namespace physics
{

///-----------------------------------------------------------------------------------------------

PhysicsCollisionResponseSystem::PhysicsCollisionResponseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void PhysicsCollisionResponseSystem::VUpdate(const float, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    auto& world = genesis::ecs::World::GetInstance();
    
    for (const auto& collidedComponentEntity: entitiesToProcess)
    {
        const auto& collidedComponent = world.GetComponent<CollidedEntitiesComponent>(collidedComponentEntity);
            
        if (world.HasEntity(collidedComponent.mCollidedEntities.first) && world.HasEntity(collidedComponent.mCollidedEntities.second))
        {
            auto& entity1TransformComponent = world.GetComponent<genesis::TransformComponent>(collidedComponent.mCollidedEntities.first);
            auto& entity1PhysicsComponent   = world.GetComponent<PhysicsComponent>(collidedComponent.mCollidedEntities.first);
                
            auto& entity2TransformComponent = world.GetComponent<genesis::TransformComponent>(collidedComponent.mCollidedEntities.second);
                
            entity1PhysicsComponent.mDirection = glm::normalize(entity1TransformComponent.mPosition - entity2TransformComponent.mPosition);
            
        }
        
        world.DestroyEntity(collidedComponentEntity);
    }
}

///-----------------------------------------------------------------------------------------------

}
