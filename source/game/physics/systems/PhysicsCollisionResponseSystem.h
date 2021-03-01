///------------------------------------------------------------------------------------------------
///  PhysicsCollisionResponseSystem.h
///  Genesis
///
///  Created by Alex Koukoulas on 24/07/2020.
///-----------------------------------------------------------------------------------------------

#ifndef PhysicsCollisionResponseSystem_h
#define PhysicsCollisionResponseSystem_h

///-----------------------------------------------------------------------------------------------

#include "../components/CollidedEntitiesComponent.h"
#include "../../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace physics
{

///-----------------------------------------------------------------------------------------------

class PhysicsCollisionResponseSystem final : public genesis::ecs::BaseSystem<CollidedEntitiesComponent>
{
public:
    PhysicsCollisionResponseSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const override;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* PhysicsCollisionResponseSystem_h */
