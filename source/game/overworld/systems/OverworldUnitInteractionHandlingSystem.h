///------------------------------------------------------------------------------------------------
///  OverworldUnitInteractionHandlingSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 30/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldUnitInteractionHandlingSystem_h
#define OverworldUnitInteractionHandlingSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include "../../../engine/common/utils/MathUtils.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

class OverworldUnitInteractionComponent;

///-----------------------------------------------------------------------------------------------
class OverworldUnitInteractionHandlingSystem final : public genesis::ecs::BaseSystem<OverworldUnitInteractionComponent>
{
public:
    OverworldUnitInteractionHandlingSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;

};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldUnitInteractionHandlingSystem_h */
