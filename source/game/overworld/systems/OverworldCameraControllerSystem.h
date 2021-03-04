///------------------------------------------------------------------------------------------------
///  OverworldCameraControllerSystem.h
///  Genesis
///
///  Created by Alex Koukoulas on 04/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldCameraControllerSystem_h
#define OverworldCameraControllerSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------
class OverworldCameraControllerSystem final : public genesis::ecs::BaseSystem<genesis::ecs::NullComponent>
{
public:
    OverworldCameraControllerSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;
    
private:
    bool IsCameraOutOfBounds() const;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldCameraControllerSystem_h */
