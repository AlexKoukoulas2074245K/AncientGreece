///------------------------------------------------------------------------------------------------
///  OverworldCameraControllerSystem.h
///  AncientGreece
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
    void NormalCameraOperation(const float dt) const;
    void FreeCameraDebugOperation(const float dt) const;
    bool IsCameraOutOfBounds() const;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldCameraControllerSystem_h */
