///------------------------------------------------------------------------------------------------
///  PhysicsMovementApplicationSystem.h
///  Genesis
///
///  Created by Alex Koukoulas on 24/07/2020.
///-----------------------------------------------------------------------------------------------

#ifndef PhysicsMovementApplicationSystem_h
#define PhysicsMovementApplicationSystem_h

///-----------------------------------------------------------------------------------------------

#include "../components/PhysicsComponent.h"
#include "../../../engine/ECS.h"
#include "../../../engine/common/components/TransformComponent.h"

///-----------------------------------------------------------------------------------------------

namespace physics
{

///-----------------------------------------------------------------------------------------------

class PhysicsMovementApplicationSystem final : public genesis::ecs::BaseSystem<PhysicsComponent, genesis::TransformComponent>
{
public:
    PhysicsMovementApplicationSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const override;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* ScriptingSystem_h */
