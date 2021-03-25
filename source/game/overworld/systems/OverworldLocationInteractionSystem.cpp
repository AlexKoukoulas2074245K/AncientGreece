///------------------------------------------------------------------------------------------------
///  OverworldLocationInteractionSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 24/03/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldLocationInteractionSystem.h"
#include "../AreaTypes.h"
#include "../components/HighlightableComponent.h"
#include "../components/OverworldMapPickingInfoSingletonComponent.h"
#include "../components/OverworldTargetComponent.h"
#include "../../view/utils/ViewUtils.h"
#include "../../../engine/common/utils/Logging.h"
#include "../../../engine/input/utils/InputUtils.h"
#include "../../../engine/rendering/components/TextStringComponent.h"

#include <map>

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const StringId PLAYER_ENTITY_NAME = StringId("player");
}

///-----------------------------------------------------------------------------------------------

OverworldLocationInteractionSystem::OverworldLocationInteractionSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void OverworldLocationInteractionSystem::VUpdate(const float, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    const auto leftMouseButtonTapped = genesis::input::GetButtonState(genesis::input::Button::LEFT_BUTTON) == genesis::input::InputState::TAPPED;
    
    const auto rightMouseButtonTapped = genesis::input::GetButtonState(genesis::input::Button::RIGHT_BUTTON) == genesis::input::InputState::TAPPED;
    
    if (!leftMouseButtonTapped && !rightMouseButtonTapped)
    {
        return;
    }
    
    auto& world = genesis::ecs::World::GetInstance();
    const auto& mapPickingInfoComponent = world.GetSingletonComponent<OverworldMapPickingInfoSingletonComponent>();
    
    auto playerEntity = world.FindEntityWithName(PLAYER_ENTITY_NAME);
    
    for (const auto entityId: entitiesToProcess)
    {
        const auto& highlightableComponent = world.GetComponent<HighlightableComponent>(entityId);
        if (highlightableComponent.mHighlighted)
        {
            if (leftMouseButtonTapped)
            {
                auto targetComponent = std::make_unique<OverworldTargetComponent>();
                targetComponent->mTargetPosition = mapPickingInfoComponent.mMapIntersectionPoint;
                targetComponent->mTargetAreaType = areaTypeMasks::NEUTRAL;
                
                if (world.HasComponent<OverworldTargetComponent>(playerEntity))
                {
                    world.RemoveComponent<OverworldTargetComponent>(playerEntity);
                }
                world.AddComponent<OverworldTargetComponent>(playerEntity, std::move(targetComponent));
            }
            else // rightButtonTapped
            {
                view::QueueView("test", StringId("test"));
            }
        }
    }
}

///-----------------------------------------------------------------------------------------------

}
