///------------------------------------------------------------------------------------------------
///  ViewManagementSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 16/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef ViewManagementSystem_h
#define ViewManagementSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include "../../engine/common/utils/MathUtils.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{
    class TransformComponent;
    namespace rendering
    {
        class TextStringComponent;
    }
}

///-----------------------------------------------------------------------------------------------

namespace view
{

///-----------------------------------------------------------------------------------------------

class ViewStateComponent;

///-----------------------------------------------------------------------------------------------
class ViewManagementSystem final : public genesis::ecs::BaseSystem<ViewStateComponent>
{
public:
    ViewManagementSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;
    
private:
    void ProcessClickableEntity(const genesis::ecs::EntityId entity, const genesis::ecs::EntityId parentViewEntity, const glm::vec2& mousePosNdc) const;
    void HandleEvent(const genesis::ecs::EntityId sourceEntityId, const StringId eventName) const;
    genesis::math::Rectangle CalculateTextBoundingRect(const genesis::TransformComponent& transformComponent, const genesis::rendering::TextStringComponent& textStringComponent) const;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* ViewManagementSystem_h */
