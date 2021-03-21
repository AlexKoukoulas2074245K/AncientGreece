///------------------------------------------------------------------------------------------------
///  HighlightingSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 21/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef HighlightingSystem_h
#define HighlightingSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

class HighlightableComponent;

///-----------------------------------------------------------------------------------------------
class HighlightingSystem final : public genesis::ecs::BaseSystem<HighlightableComponent>
{
public:
    HighlightingSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* HighlightingSystem_h */
