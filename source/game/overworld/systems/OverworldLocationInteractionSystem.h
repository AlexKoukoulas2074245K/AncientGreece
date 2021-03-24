///------------------------------------------------------------------------------------------------
///  OverworldLocationInteractionSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 24/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldLocationInteractionSystem_h
#define OverworldLocationInteractionSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{
    namespace rendering
    {
        class TextStringComponent;
    }
}

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

class HighlightableComponent;

///-----------------------------------------------------------------------------------------------
class OverworldLocationInteractionSystem final : public genesis::ecs::BaseSystem<HighlightableComponent, genesis::rendering::TextStringComponent>
{
public:
    OverworldLocationInteractionSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const override;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldLocationInteractionSystem_h */
