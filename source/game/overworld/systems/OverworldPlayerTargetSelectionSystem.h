///------------------------------------------------------------------------------------------------
///  OverworldPlayerTargetSelectionSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 05/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldPlayerTargetSelectionSystem_h
#define OverworldPlayerTargetSelectionSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include "../../engine/common/utils/MathUtils.h"
#include "../../engine/common/utils/ColorUtils.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------
class OverworldPlayerTargetSelectionSystem final : public genesis::ecs::BaseSystem<genesis::ecs::NullComponent>
{
public:
    OverworldPlayerTargetSelectionSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldPlayerTargetSelectionSystem_h */
