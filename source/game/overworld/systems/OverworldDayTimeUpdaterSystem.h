///------------------------------------------------------------------------------------------------
///  OverworldDayTimeUpdaterSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 18/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldDayTimeUpdaterSystem_h
#define OverworldDayTimeUpdaterSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------
class OverworldDayTimeUpdaterSystem final : public genesis::ecs::BaseSystem<genesis::ecs::NullComponent>
{
public:
    OverworldDayTimeUpdaterSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldDayTimeUpdaterSystem_h */
