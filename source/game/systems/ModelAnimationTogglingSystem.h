///------------------------------------------------------------------------------------------------
///  ModelAnimationTogglingSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 30/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef ModelAnimationTogglingSystem_h
#define ModelAnimationTogglingSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{
    namespace rendering
    {
        class RenderableComponent;
    }
}

///-----------------------------------------------------------------------------------------------
class ModelAnimationTogglingSystem final : public genesis::ecs::BaseSystem<genesis::rendering::RenderableComponent>
{
public:
    ModelAnimationTogglingSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;
};

///-----------------------------------------------------------------------------------------------

#endif /* ModelAnimationTogglingSystem_h */
