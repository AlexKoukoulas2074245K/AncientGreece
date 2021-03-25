///------------------------------------------------------------------------------------------------
///  OverworldCityStateUtils.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 25/03/2021.
///------------------------------------------------------------------------------------------------

#include "OverworldCityStateUtils.h"
#include "../components/HighlightableComponent.h"
#include "../../components/CityStateInfoSingletonComponent.h"
#include "../../../engine/ECS.h"
#include "../../../engine/rendering/utils/FontUtils.h"
#include "../../../engine/rendering/utils/Colors.h"

///------------------------------------------------------------------------------------------------

namespace overworld
{

///------------------------------------------------------------------------------------------------

namespace
{
    static const StringId GAME_FONT_NAME = StringId("ancient_greek_font");
    static const float CITY_STATE_NAME_BASE_SIZE = 0.005f;
    static const float CITY_STATE_NAME_SIZE_RENOWN_MULTIPLIER = 0.0001f;
}

///------------------------------------------------------------------------------------------------

void PopulateOverworldCityStates()
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& cityStateInfoComponent = world.GetSingletonComponent<CityStateInfoSingletonComponent>();
    
    for (const auto& cityStateInfoEntry: cityStateInfoComponent.mCityStateNameToInfo)
    {
        auto cityStateEntity = genesis::rendering::RenderText(cityStateInfoEntry.first.GetString(), GAME_FONT_NAME, GetCityStateOverworldNameSize(cityStateInfoEntry.first), glm::vec3(cityStateInfoEntry.second.mPosition.x, cityStateInfoEntry.second.mPosition.y, 0.00f), genesis::rendering::colors::BLACK, true, cityStateInfoEntry.first);

        world.AddComponent<overworld::HighlightableComponent>(cityStateEntity, std::make_unique<overworld::HighlightableComponent>());
    }
}

///-----------------------------------------------------------------------------------------------

float GetCityStateOverworldNameSize(const StringId cityStateName)
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& cityStateInfoComponent = world.GetSingletonComponent<CityStateInfoSingletonComponent>();
    return CITY_STATE_NAME_BASE_SIZE + CITY_STATE_NAME_SIZE_RENOWN_MULTIPLIER * cityStateInfoComponent.mCityStateNameToInfo.at(cityStateName).mRenown;
}

///-----------------------------------------------------------------------------------------------

}

