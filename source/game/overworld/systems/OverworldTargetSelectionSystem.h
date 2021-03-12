///------------------------------------------------------------------------------------------------
///  OverworldTargetSelectionSystem.h
///  Genesis
///
///  Created by Alex Koukoulas on 05/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldTargetSelectionSystem_h
#define OverworldTargetSelectionSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include "../../engine/common/utils/MathUtils.h"
#include "../../engine/common/utils/ColorUtils.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------
class OverworldTargetSelectionSystem final : public genesis::ecs::BaseSystem<genesis::ecs::NullComponent>
{
public:
    OverworldTargetSelectionSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldTargetSelectionSystem_h */
