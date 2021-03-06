///------------------------------------------------------------------------------------------------
///  RawInputHandlingSystem.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef RawInputHandlingSystem_h
#define RawInputHandlingSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace input
{

///-----------------------------------------------------------------------------------------------
class RawInputHandlingSystem final : public ecs::BaseSystem<ecs::NullComponent>
{
public:
    RawInputHandlingSystem();

    void VUpdate(const float dt, const std::vector<ecs::EntityId>&) const override;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* RawInputHandlingSystem_h */
