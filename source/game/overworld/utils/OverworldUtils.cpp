///------------------------------------------------------------------------------------------------
///  OverworldUtils.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 01/04/2021.
///------------------------------------------------------------------------------------------------

#include "OverworldUtils.h"
#include "../components/HighlightableComponent.h"
#include "../../components/CityStateInfoSingletonComponent.h"
#include "../../../engine/ECS.h"
#include "../../../engine/common/utils/ColorUtils.h"
#include "../../../engine/rendering/utils/FontUtils.h"
#include "../../../engine/rendering/utils/MeshUtils.h"
#include "../../../engine/rendering/utils/HeightMapUtils.h"

///------------------------------------------------------------------------------------------------

namespace overworld
{

///------------------------------------------------------------------------------------------------

namespace
{
    static const StringId GAME_FONT_NAME             = StringId("ancient_greek_font");
    static const StringId UNIT_PREVIEW_POPUP_NAME    = StringId("unit_preview_popup");
    static const StringId MAP_ENTITY_NAME            = StringId("map");
    static const StringId MAP_EDGE_1_ENTITY_NAME     = StringId("map_edge_1");
    static const StringId MAP_EDGE_2_ENTITY_NAME     = StringId("map_edge_2");
    static const StringId MAP_EDGE_3_ENTITY_NAME     = StringId("map_edge_3");
    static const StringId MAP_EDGE_4_ENTITY_NAME     = StringId("map_edge_4");
    static const StringId OVERWORLD_UNIT_ENTITY_NAME = StringId("overworld_unit");
    static const StringId PLAYER_UNIT_ENTITY_NAME    = StringId("player");

    static const std::string OVERWORLD_HEIGHTMAP_NAME = "overworld";
    static const std::string NAME_PLATE_MODEL_NAME    = "name_plate";
    static const std::string MAP_EDGE_MODEL_NAME      = "map_edge";

    static const float NAME_PLATE_Z                           = -0.001f;
    static const float NAME_PLATE_X_OFFSET_MULTIPLIER         = 1.0/20.0f;
    static const float NAME_PLATE_HEIGHT_MULTIPLIER           = 1.2f;
    static const float NAME_PLATE_WIDTH_MULTIPLIER            = 1.1f;
    static const float CITY_STATE_NAME_BASE_SIZE              = 0.001f;
    static const float CITY_STATE_NAME_Z                      = -0.002f;
    static const float CITY_STATE_NAME_SIZE_RENOWN_MULTIPLIER = 0.0001f;
}

///------------------------------------------------------------------------------------------------

float GetCityStateOverworldNameSize(const StringId cityStateName);
void LoadAndCreateOverworldMapComponents();
void PopulateOverworldCityStates();
void RemoveOverworldCityStates();
void RemoveOverworldMapComponents();

///------------------------------------------------------------------------------------------------

void PopulateOverworldEntities()
{
    overworld::PopulateOverworldCityStates();
    LoadAndCreateOverworldMapComponents();
}

///------------------------------------------------------------------------------------------------

void DestroyOverworldEntities()
{
    auto& world = genesis::ecs::World::GetInstance();
    
    RemoveOverworldCityStates();
    RemoveOverworldMapComponents();
    
    world.DestroyEntities(world.FindAllEntitiesWithName(UNIT_PREVIEW_POPUP_NAME));
    world.DestroyEntities(world.FindAllEntitiesWithName(OVERWORLD_UNIT_ENTITY_NAME));
    world.DestroyEntities(world.FindAllEntitiesWithName(PLAYER_UNIT_ENTITY_NAME));
}

///-----------------------------------------------------------------------------------------------

float GetCityStateOverworldNameSize(const StringId cityStateName)
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& cityStateInfoComponent = world.GetSingletonComponent<CityStateInfoSingletonComponent>();
    return CITY_STATE_NAME_BASE_SIZE + CITY_STATE_NAME_SIZE_RENOWN_MULTIPLIER * cityStateInfoComponent.mCityStateNameToInfo.at(cityStateName).mRenown;
}

///-----------------------------------------------------------------------------------------------

void LoadAndCreateOverworldMapComponents()
{
    genesis::rendering::LoadAndCreateHeightMapByName(OVERWORLD_HEIGHTMAP_NAME, MAP_ENTITY_NAME);
    genesis::rendering::LoadAndCreateStaticModelByName(MAP_EDGE_MODEL_NAME, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), MAP_EDGE_1_ENTITY_NAME);
    genesis::rendering::LoadAndCreateStaticModelByName(MAP_EDGE_MODEL_NAME, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), MAP_EDGE_2_ENTITY_NAME);
    genesis::rendering::LoadAndCreateStaticModelByName(MAP_EDGE_MODEL_NAME, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), MAP_EDGE_3_ENTITY_NAME);
    genesis::rendering::LoadAndCreateStaticModelByName(MAP_EDGE_MODEL_NAME, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), MAP_EDGE_4_ENTITY_NAME);
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

void RemoveOverworldMapComponents()
{
    auto& world = genesis::ecs::World::GetInstance();
    world.DestroyEntity(world.FindEntityWithName(MAP_ENTITY_NAME));
    world.DestroyEntity(world.FindEntityWithName(MAP_EDGE_1_ENTITY_NAME));
    world.DestroyEntity(world.FindEntityWithName(MAP_EDGE_2_ENTITY_NAME));
    world.DestroyEntity(world.FindEntityWithName(MAP_EDGE_3_ENTITY_NAME));
    world.DestroyEntity(world.FindEntityWithName(MAP_EDGE_4_ENTITY_NAME));
}

///-----------------------------------------------------------------------------------------------

}



