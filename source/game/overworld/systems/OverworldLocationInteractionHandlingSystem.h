///------------------------------------------------------------------------------------------------
///  OverworldLocationInteractionHandlingSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 24/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldLocationInteractionHandlingSystem_h
#define OverworldLocationInteractionHandlingSystem_h

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
class OverworldLocationInteractionHandlingSystem final : public genesis::ecs::BaseSystem<HighlightableComponent, genesis::rendering::TextStringComponent>
{
public:
    OverworldLocationInteractionHandlingSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const override;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldLocationInteractionHandlingSystem_h */
