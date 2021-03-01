///------------------------------------------------------------------------------------------------
///  SceneUpdaterSystem.h
///  Genesis
///
///  Created by Alex Koukoulas on 24/07/2020.
///-----------------------------------------------------------------------------------------------

#ifndef SceneUpdaterSystem_h
#define SceneUpdaterSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../physics/components/PhysicsComponent.h"
#include "../../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace scene
{

///-----------------------------------------------------------------------------------------------

class SceneUpdaterSystem final : public genesis::ecs::BaseSystem<physics::PhysicsComponent>
{
public:
    SceneUpdaterSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const override;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* SceneUpdaterSystem_h */
