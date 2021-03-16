///------------------------------------------------------------------------------------------------
///  ViewManagementSystem.h
///  Genesis
///
///  Created by Alex Koukoulas on 16/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef ViewManagementSystem_h
#define ViewManagementSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace view
{

///-----------------------------------------------------------------------------------------------
class ViewManagementSystem final : public genesis::ecs::BaseSystem<genesis::ecs::NullComponent>
{
public:
    ViewManagementSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* ViewManagementSystem_h */
