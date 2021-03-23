///------------------------------------------------------------------------------------------------
///  OverworldMapPickingInfoSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 23/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldMapPickingInfoSystem_h
#define OverworldMapPickingInfoSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

class OverworldMapPickingInfoSystem final : public genesis::ecs::BaseSystem<genesis::ecs::NullComponent>
{
public:
    OverworldMapPickingInfoSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const override;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldMapPickingInfoSystem_h */
