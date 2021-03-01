///------------------------------------------------------------------------------------------------
///  PhysicsCollisionDetectionSystem.h
///  Genesis
///
///  Created by Alex Koukoulas on 24/07/2020.
///-----------------------------------------------------------------------------------------------

#ifndef PhysicsCollisionDetectionSystem_h
#define PhysicsCollisionDetectionSystem_h

///-----------------------------------------------------------------------------------------------

#include "../components/PhysicsComponent.h"
#include "../../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace physics
{

///-----------------------------------------------------------------------------------------------

class PhysicsCollisionDetectionSystem final : public genesis::ecs::BaseSystem<PhysicsComponent>
{
public:
    PhysicsCollisionDetectionSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const override;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* PhysicsCollisionDetectionSystem_h */
