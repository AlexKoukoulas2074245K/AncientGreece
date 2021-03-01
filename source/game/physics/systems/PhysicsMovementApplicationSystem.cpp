///------------------------------------------------------------------------------------------------
///  PhysicsMovementApplicationSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 24/07/2020.
///-----------------------------------------------------------------------------------------------

#include "PhysicsMovementApplicationSystem.h"

///-----------------------------------------------------------------------------------------------

namespace physics
{

///-----------------------------------------------------------------------------------------------

PhysicsMovementApplicationSystem::PhysicsMovementApplicationSystem()
{
}

///-----------------------------------------------------------------------------------------------

void PhysicsMovementApplicationSystem::VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    for (const auto& entityId: entitiesToProcess)
    {
        auto& physicsComponent = genesis::ecs::World::GetInstance().GetComponent<PhysicsComponent>(entityId);
        auto& transformComponent = genesis::ecs::World::GetInstance().GetComponent<genesis::TransformComponent>(entityId);
        
        transformComponent.mPosition += physicsComponent.mDirection * physicsComponent.mVelocitySpeed * dt;
        transformComponent.mRotation.y += physicsComponent.mRotationalSpeed * dt;
        
        physicsComponent.mDirection = glm::normalize(physicsComponent.mDirection);
    }
}

///-----------------------------------------------------------------------------------------------

}
