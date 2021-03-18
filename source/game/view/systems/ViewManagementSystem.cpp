///------------------------------------------------------------------------------------------------
///  ViewManagementSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 16/03/2021.
///-----------------------------------------------------------------------------------------------

#include "ViewManagementSystem.h"
#include "../components/ClickableComponent.h"
#include "../components/ViewStateComponent.h"
#include "../components/ViewQueueSingletonComponent.h"
#include "../utils/ViewUtils.h"
#include "../../engine/common/components/TransformComponent.h"
#include "../../engine/common/utils/Logging.h"
#include "../../engine/input/utils/InputUtils.h"
#include "../../engine/rendering/components/RenderableComponent.h"
#include "../../engine/rendering/components/TextStringComponent.h"
#include "../../engine/rendering/components/WindowSingletonComponent.h"

///-----------------------------------------------------------------------------------------------

namespace view
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const StringId GUI_SHADER_CUSTOM_COLOR_UNIFORM_NAME = StringId("custom_color");
    static const StringId CLOSE_EVENT_NAME = StringId("close");
}

///-----------------------------------------------------------------------------------------------

ViewManagementSystem::ViewManagementSystem()
    : BaseSystem()
{
    genesis::ecs::World::GetInstance().SetSingletonComponent<ViewQueueSingletonComponent>(std::make_unique<ViewQueueSingletonComponent>());
}

///-----------------------------------------------------------------------------------------------

void ViewManagementSystem::VUpdate(const float, const std::vector<genesis::ecs::EntityId>&  entitiesToProcess) const
{
    auto entities = entitiesToProcess;
    auto& world = genesis::ecs::World::GetInstance();
    auto& viewQueueComponent = world.GetSingletonComponent<ViewQueueSingletonComponent>();
    
    // No active views
    if (entities.size() == 0)
    {
        // Queued view exists
        if (viewQueueComponent.mQueuedViews.size() > 0)
        {
            auto& viewEntry = viewQueueComponent.mQueuedViews.front();
            entities.push_back(LoadAndShowView(viewEntry.first, viewEntry.second));
            viewQueueComponent.mQueuedViews.pop();
        }
    }
    
    const auto& windowComponent = world.GetSingletonComponent<genesis::rendering::WindowSingletonComponent>();
    const auto& mousePosNdc = genesis::math::ComputeMouseCoordsInNDC(windowComponent.mRenderableWidth, windowComponent.mRenderableHeight);
    
    for (auto viewEntity: entities)
    {
        const auto& viewStateComponent = world.GetComponent<ViewStateComponent>(viewEntity);
        for (auto childEntity: viewStateComponent.mViewEntities)
        {
            if (world.HasComponent<ClickableComponent>(childEntity))
            {
                ProcessClickableEntity(childEntity, viewEntity, mousePosNdc);
            }
        }
    }
}

///-----------------------------------------------------------------------------------------------

void ViewManagementSystem::ProcessClickableEntity(const genesis::ecs::EntityId entity, const genesis::ecs::EntityId parentViewEntity, const glm::vec2& mousePosNdc) const
{
    auto& world = genesis::ecs::World::GetInstance();
    auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(entity);
    const auto& clickableComponent = world.GetComponent<ClickableComponent>(entity);
    const auto& textComponent = world.GetComponent<genesis::rendering::TextStringComponent>(entity);
    const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entity);
    
    const auto boundingRect = CalculateTextBoundingRect(transformComponent, textComponent);
    
    if (genesis::math::IsPointInsideRectangle(boundingRect.bottomLeft, boundingRect.topRight, mousePosNdc))
    {
        renderableComponent.mShaderUniforms.mShaderFloatVec4Uniforms[GUI_SHADER_CUSTOM_COLOR_UNIFORM_NAME] = clickableComponent.mInteractionColor;
        
        if (genesis::input::GetButtonState(genesis::input::Button::LEFT_BUTTON) == genesis::input::InputState::TAPPED)
        {
            HandleEvent(parentViewEntity, clickableComponent.mInteractionEvent);
            genesis::input::ConsumeButtonInput(genesis::input::Button::LEFT_BUTTON);
        }
    }
    else
    {
        renderableComponent.mShaderUniforms.mShaderFloatVec4Uniforms[GUI_SHADER_CUSTOM_COLOR_UNIFORM_NAME] = clickableComponent.mTextColor;
    }
}

///-----------------------------------------------------------------------------------------------

void ViewManagementSystem::HandleEvent(const genesis::ecs::EntityId sourceEntityId, const StringId eventName) const
{
    if (eventName == CLOSE_EVENT_NAME)
    {
        DestroyView(sourceEntityId);
    }
}

///-----------------------------------------------------------------------------------------------

genesis::math::Rectangle ViewManagementSystem::CalculateTextBoundingRect(const genesis::TransformComponent& transformComponent, const genesis::rendering::TextStringComponent& textStringComponent) const
{
    genesis::math::Rectangle result;
    const auto renderedTextWidth = ((textStringComponent.mPaddingProportionalToSize * textStringComponent.mCharacterSize) * textStringComponent.mText.size());
    const auto renderedTextHeight = textStringComponent.mCharacterSize;
    
    result.bottomLeft = glm::vec2(transformComponent.mPosition.x - textStringComponent.mCharacterSize/8.0f, transformComponent.mPosition.y - renderedTextHeight/4.0f);
    result.topRight = glm::vec2(transformComponent.mPosition.x + renderedTextWidth - textStringComponent.mCharacterSize/8.0f, transformComponent.mPosition.y + renderedTextHeight/2.0f);
    
    return result;
}


///-----------------------------------------------------------------------------------------------


}
