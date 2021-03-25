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
#include "../../components/CityStateInfoSingletonComponent.h"
#include "../../utils/CityStateInfoUtils.h"
#include "../../utils/KeyValueUtils.h"
#include "../../view/utils/ViewUtils.h"
#include "../../../engine/common/components/NameComponent.h"
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
    static const StringId CITY_STATE_NAME_DS_KEY = StringId("city_state_name");
    static const StringId CITY_STATE_RENOWN_DS_KEY = StringId("city_state_renown");
    static const StringId CITY_STATE_GARISSON_DS_KEY = StringId("city_state_garisson");
    static const StringId CITY_STATE_DESCRIPTION_DS_KEY = StringId("city_state_description");

    static const std::string CITY_STATE_PREVIEW_NAME = "city_state_preview";
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
                const auto cityName = world.GetComponent<genesis::NameComponent>(entityId).mName;
                const auto& cityStateInfo = GetCityStateInfo(cityName);
                WriteValue(CITY_STATE_NAME_DS_KEY, cityName.GetString());
                WriteValue(CITY_STATE_RENOWN_DS_KEY, std::to_string(cityStateInfo.mRenown));
                WriteValue(CITY_STATE_GARISSON_DS_KEY, std::to_string(cityStateInfo.mGarisson));
                WriteValue(CITY_STATE_DESCRIPTION_DS_KEY, cityStateInfo.mDescription);
                view::QueueView(CITY_STATE_PREVIEW_NAME);
            }
        }
    }
}

///-----------------------------------------------------------------------------------------------

}
