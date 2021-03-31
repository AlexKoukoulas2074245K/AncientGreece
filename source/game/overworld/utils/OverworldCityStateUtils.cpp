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
#include "../../../engine/common/utils/ColorUtils.h"
#include "../../../engine/rendering/utils/FontUtils.h"
#include "../../../engine/rendering/utils/MeshUtils.h"

///------------------------------------------------------------------------------------------------

namespace overworld
{

///------------------------------------------------------------------------------------------------

namespace
{
    static const StringId GAME_FONT_NAME                      = StringId("ancient_greek_font");
    static const std::string NAME_PLATE_MODEL_NAME            = "name_plate";
    static const float NAME_PLATE_Z                           = -0.001f;
    static const float NAME_PLATE_X_OFFSET_MULTIPLIER         = 1.0/20.0f;
    static const float NAME_PLATE_HEIGHT_MULTIPLIER           = 1.2f;
    static const float NAME_PLATE_WIDTH_MULTIPLIER            = 1.1f;
    static const float CITY_STATE_NAME_BASE_SIZE              = 0.001f;
    static const float CITY_STATE_NAME_Z                      = -0.002f;
    static const float CITY_STATE_NAME_SIZE_RENOWN_MULTIPLIER = 0.0001f;
}

///------------------------------------------------------------------------------------------------

void PopulateOverworldCityStates()
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& cityStateInfoComponent = world.GetSingletonComponent<CityStateInfoSingletonComponent>();
    
    for (const auto& cityStateInfoEntry: cityStateInfoComponent.mCityStateNameToInfo)
    {
        auto cityStateEntity = genesis::rendering::RenderText(cityStateInfoEntry.first.GetString(), GAME_FONT_NAME, GetCityStateOverworldNameSize(cityStateInfoEntry.first), glm::vec3(cityStateInfoEntry.second.mPosition.x, cityStateInfoEntry.second.mPosition.y, CITY_STATE_NAME_Z), genesis::colors::BLACK, true, cityStateInfoEntry.first);
        
        const auto textRect = genesis::rendering::CalculateTextBoundingRect(cityStateEntity);
        const auto textWidth = textRect.topRight.x - textRect.bottomLeft.x;
        const auto textHeight = textRect.topRight.y - textRect.bottomLeft.y;
        
        genesis::rendering::LoadAndCreateStaticModelByName(NAME_PLATE_MODEL_NAME, glm::vec3(textRect.bottomLeft.x + textWidth/2 - textWidth * NAME_PLATE_X_OFFSET_MULTIPLIER, textRect.bottomLeft.y + textHeight/2.0, NAME_PLATE_Z), glm::vec3(), glm::vec3(textWidth * NAME_PLATE_WIDTH_MULTIPLIER, textHeight * NAME_PLATE_HEIGHT_MULTIPLIER, 1.0f), cityStateInfoEntry.first);
        
        world.AddComponent<overworld::HighlightableComponent>(cityStateEntity, std::make_unique<overworld::HighlightableComponent>());
    }
}

///-----------------------------------------------------------------------------------------------

void RemoveOverworldCityStates()
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& cityStateInfoComponent = world.GetSingletonComponent<CityStateInfoSingletonComponent>();
    
    for (const auto& cityStateInfoEntry: cityStateInfoComponent.mCityStateNameToInfo)
    {
        world.DestroyEntities(world.FindAllEntitiesWithName(cityStateInfoEntry.first));
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

