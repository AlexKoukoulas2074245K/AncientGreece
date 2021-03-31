///------------------------------------------------------------------------------------------------
///  BattleCameraControllerSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef BattleCameraControllerSystem_h
#define BattleCameraControllerSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------
class BattleCameraControllerSystem final : public genesis::ecs::BaseSystem<genesis::ecs::NullComponent>
{
public:
    BattleCameraControllerSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;
    
private:
    void NormalCameraOperation(const float dt) const;
    void FreeCameraDebugOperation(const float dt) const;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* BattleCameraControllerSystem_h */
