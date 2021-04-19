///------------------------------------------------------------------------------------------------
///  OverworldUtils.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 01/04/2021.
///------------------------------------------------------------------------------------------------

#include "OverworldUtils.h"
#include "../components/OverworldDayTimeSingletonComponent.h"
#include "../components/OverworldHighlightableComponent.h"
#include "../../components/CollidableComponent.h"
#include "../../components/CityStateInfoSingletonComponent.h"
#include "../../components/UnitStatsComponent.h"
#include "../../utils/UnitFactoryUtils.h"
#include "../../utils/UnitInfoUtils.h"
#include "../../../engine/ECS.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/common/utils/ColorUtils.h"
#include "../../../engine/rendering/components/CameraSingletonComponent.h"
#include "../../../engine/rendering/components/HeightMapComponent.h"
#include "../../../engine/rendering/components/RenderableComponent.h"
#include "../../../engine/rendering/utils/FontUtils.h"
#include "../../../engine/rendering/utils/MeshUtils.h"
#include "../../../engine/rendering/utils/HeightMapUtils.h"
#include "../../../engine/resources/MeshResource.h"
#include "../../../engine/resources/TextureLoader.h"
#include "../../../engine/resources/ResourceLoadingService.h"

#include <fstream>
#include <json.hpp>

///------------------------------------------------------------------------------------------------

namespace overworld
{

///------------------------------------------------------------------------------------------------

namespace
{
    static const StringId GAME_FONT_NAME                     = StringId("ancient_greek_font");
    static const StringId UNIT_PREVIEW_POPUP_NAME            = StringId("unit_preview_popup");
    static const StringId MAP_ENTITY_NAME                    = StringId("map");
    static const StringId MAP_EDGE_1_ENTITY_NAME             = StringId("map_edge_1");
    static const StringId MAP_EDGE_2_ENTITY_NAME             = StringId("map_edge_2");
    static const StringId MAP_EDGE_3_ENTITY_NAME             = StringId("map_edge_3");
    static const StringId MAP_EDGE_4_ENTITY_NAME             = StringId("map_edge_4");
    static const StringId GENERIC_OVERWORLD_UNIT_ENTITY_NAME = StringId("overworld_unit");
    static const StringId PLAYER_ENTITY_NAME                 = StringId("player");

    static const std::string OVERWORLD_HEIGHTMAP_NAME       = "overworld";
    static const std::string NAME_PLATE_MODEL_NAME          = "name_plate";
    static const std::string MAP_EDGE_MODEL_NAME            = "map_edge";
    static const std::string CITY_STATE_BUILDING_MODEL_NAME = "building";
    static const std::string SAVE_FILE_PATH = "save.json";

    static const float ENTITY_SPHERE_COLLISION_MULTIPLIER     = 0.25f * 0.3333f;

    static const glm::vec3 CITY_STATE_BASE_SCALE              = glm::vec3(0.01f);
    static const glm::vec3 CITY_STATE_SCALE_RENOWN_MULTIPLIER = glm::vec3(0.0003f);
}

///------------------------------------------------------------------------------------------------

glm::vec3 GetCityStateOverworldScale(const StringId& cityStateName);
void AddCollidableDataToCityState(const genesis::ecs::EntityId cityStateEntity);
void LoadAndCreateOverworldMapComponents();
void PopulateOverworldCityStates();
void RemoveOverworldCityStates();
void RemoveOverworldMapComponents();

///------------------------------------------------------------------------------------------------

genesis::ecs::EntityId GetMapEntity()
{
    return genesis::ecs::World::GetInstance().FindEntityWithName(MAP_ENTITY_NAME);
}

///------------------------------------------------------------------------------------------------

genesis::ecs::EntityId GetPlayerEntity()
{
    return genesis::ecs::World::GetInstance().FindEntityWithName(PLAYER_ENTITY_NAME);
}

///------------------------------------------------------------------------------------------------

StringId GetPlayerUnitName()
{
    const auto& world = genesis::ecs::World::GetInstance();
    return world.GetComponent<UnitStatsComponent>(world.FindEntityWithName(PLAYER_ENTITY_NAME)).mStats.mUnitName;
}

///------------------------------------------------------------------------------------------------

StringId GetPlayerEntityName()
{
    return PLAYER_ENTITY_NAME;
}

///------------------------------------------------------------------------------------------------

StringId GetGenericOverworldUnitEntityName()
{
    return GENERIC_OVERWORLD_UNIT_ENTITY_NAME;
}

///------------------------------------------------------------------------------------------------

void PopulateOverworldEntities()
{
    LoadAndCreateOverworldMapComponents();
    overworld::PopulateOverworldCityStates();
}

///------------------------------------------------------------------------------------------------

void DestroyOverworldEntities()
{
    auto& world = genesis::ecs::World::GetInstance();
    
    SaveOverworldStateToFile();
    
    RemoveOverworldCityStates();
    RemoveOverworldMapComponents();
    
    world.DestroyEntities(world.FindAllEntitiesWithName(UNIT_PREVIEW_POPUP_NAME));
    world.DestroyEntities(world.FindAllEntitiesWithName(GENERIC_OVERWORLD_UNIT_ENTITY_NAME));
    world.DestroyEntities(world.FindAllEntitiesWithName(PLAYER_ENTITY_NAME));
}

///-----------------------------------------------------------------------------------------------

void SaveOverworldStateToFile()
{
    nlohmann::json saveFileRoot;
    
    auto& world = genesis::ecs::World::GetInstance();
    
    // Save player data
    const auto playerEntity = GetPlayerEntity();
    const auto& playerUnitStatsComponent = world.GetComponent<UnitStatsComponent>(playerEntity);
    const auto& playerTransformComponent = world.GetComponent<genesis::TransformComponent>(playerEntity);
    const auto& dayTimeComponent = world.GetSingletonComponent<OverworldDayTimeSingletonComponent>();
    
    nlohmann::json overworldStateJsonObject;
    overworldStateJsonObject["time_accumulator"] = dayTimeComponent.mTimeDtAccum;
    overworldStateJsonObject["current_day"] = dayTimeComponent.mCurrentDay;
    overworldStateJsonObject["current_year"] = dayTimeComponent.mCurrentYearBc;
    
    nlohmann::json playerJsonObject;
    playerJsonObject["player_unit_name"] = playerUnitStatsComponent.mStats.mUnitName.GetString();
    playerJsonObject["player_unit_type"] = playerUnitStatsComponent.mStats.mUnitType.GetString();
    playerJsonObject["player_x"] = playerTransformComponent.mPosition.x;
    playerJsonObject["player_y"] = playerTransformComponent.mPosition.y;
    playerJsonObject["player_z"] = playerTransformComponent.mPosition.z;
    playerJsonObject["player_rx"] = playerTransformComponent.mRotation.x;
    playerJsonObject["player_ry"] = playerTransformComponent.mRotation.y;
    playerJsonObject["player_rz"] = playerTransformComponent.mRotation.z;
    
    nlohmann::json playerPartyJsonObject;
    for (auto i = 1U; i < playerUnitStatsComponent.mParty.size(); ++i)
    {
        const auto& partyUnitStats = playerUnitStatsComponent.mParty[i];
        playerPartyJsonObject.push_back(partyUnitStats.mUnitType.GetString());
    }
    
    playerJsonObject["player_party"] = playerPartyJsonObject;
    
    // Save overworld unit data
    nlohmann::json overworldUnitsJsonObject;
    const auto& overworldUnitEntities = world.FindAllEntitiesWithName(GENERIC_OVERWORLD_UNIT_ENTITY_NAME);
    
    for (const auto overworldUnitEntity: overworldUnitEntities)
    {
        nlohmann::json unitJsonObject;
        const auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(overworldUnitEntity);
        const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(overworldUnitEntity);
        
        unitJsonObject["name"] = unitStatsComponent.mStats.mUnitName.GetString();
        unitJsonObject["unit_type"] = unitStatsComponent.mStats.mUnitType.GetString();
        unitJsonObject["x"] = transformComponent.mPosition.x;
        unitJsonObject["y"] = transformComponent.mPosition.y;
        unitJsonObject["z"] = transformComponent.mPosition.z;
        unitJsonObject["rx"] = transformComponent.mRotation.x;
        unitJsonObject["ry"] = transformComponent.mRotation.y;
        unitJsonObject["rz"] = transformComponent.mRotation.z;
        
        nlohmann::json partyJsonObject;
        for (auto i = 1U; i < unitStatsComponent.mParty.size(); ++i)
        {
            const auto& partyUnitStats = unitStatsComponent.mParty[i];
            partyJsonObject.push_back(partyUnitStats.mUnitType.GetString());
        }
        
        unitJsonObject["party"] = partyJsonObject;
        overworldUnitsJsonObject.push_back(unitJsonObject);
    }
    
    saveFileRoot["overworld_state"] = overworldStateJsonObject;
    saveFileRoot["player"] = playerJsonObject;
    saveFileRoot["overworld_units"] = overworldUnitsJsonObject;
    
    std::ofstream saveFile(SAVE_FILE_PATH);
    saveFile << saveFileRoot.dump(4);
}

///-----------------------------------------------------------------------------------------------

bool TryLoadOverworldStateFromFile()
{
    std::ifstream saveFile(SAVE_FILE_PATH);
    if (!saveFile.good())
    {
        return false;
    }
    
    // Read save file as string
    std::string fileData;
    
    saveFile.seekg(0, std::ios::end);
    fileData.reserve(static_cast<size_t>(saveFile.tellg()));
    saveFile.seekg(0, std::ios::beg);
    
    fileData.assign((std::istreambuf_iterator<char>(saveFile)),
               std::istreambuf_iterator<char>());
    
    
    // Parse save file
    const auto saveFileJsonRoot = nlohmann::json::parse(fileData);
    auto& world = genesis::ecs::World::GetInstance();
    auto& dayTimeComponent = world.GetSingletonComponent<OverworldDayTimeSingletonComponent>();
    
    // Parse overworld state
    const auto& overworldStateJsonObject = saveFileJsonRoot["overworld_state"];
    dayTimeComponent.mTimeDtAccum = overworldStateJsonObject["time_accumulator"].get<float>();
    dayTimeComponent.mCurrentDay = overworldStateJsonObject["current_day"].get<int>();
    dayTimeComponent.mCurrentYearBc = overworldStateJsonObject["current_year"].get<int>();
    
    // Parse player data
    const auto& playerJsonObject = saveFileJsonRoot["player"];
    const auto playerUnitName = StringId(playerJsonObject["player_unit_name"].get<std::string>());
    const auto playerUnitType = StringId(playerJsonObject["player_unit_type"].get<std::string>());
    const auto playerPosition = glm::vec3
    (
        playerJsonObject["player_x"].get<float>(),
        playerJsonObject["player_y"].get<float>(),
        playerJsonObject["player_z"].get<float>()
    );
    const auto playerRotation = glm::vec3
    (
        playerJsonObject["player_rx"].get<float>(),
        playerJsonObject["player_ry"].get<float>(),
        playerJsonObject["player_rz"].get<float>()
    );
    
    auto playerEntity = CreateUnit(playerUnitType, playerUnitName, PLAYER_ENTITY_NAME, playerPosition, playerRotation);
    
    auto& playerUnitStatsComponent = world.GetComponent<UnitStatsComponent>(playerEntity);
    for (const auto& partyEntry: playerJsonObject["player_party"])
    {
        playerUnitStatsComponent.mParty.push_back(GetUnitBaseStats(StringId(partyEntry.get<std::string>())));
    }
    
    // Parse overworld units
    for (const auto& overworldUnitJsonObject: saveFileJsonRoot["overworld_units"])
    {
        const auto unitName = StringId(overworldUnitJsonObject["name"].get<std::string>());
        const auto unitType = StringId(overworldUnitJsonObject["unit_type"].get<std::string>());
        const auto unitPosition = glm::vec3
        (
            overworldUnitJsonObject["x"].get<float>(),
            overworldUnitJsonObject["y"].get<float>(),
            overworldUnitJsonObject["z"].get<float>()
        );
        const auto unitRotation = glm::vec3
        (
            overworldUnitJsonObject["rx"].get<float>(),
            overworldUnitJsonObject["ry"].get<float>(),
            overworldUnitJsonObject["rz"].get<float>()
        );
        
        auto unitEntity = CreateUnit(unitType, unitName, GENERIC_OVERWORLD_UNIT_ENTITY_NAME, unitPosition, unitRotation);
        
        auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(unitEntity);
        for (const auto& partyEntry: overworldUnitJsonObject["party"])
        {
            unitStatsComponent.mParty.push_back(GetUnitBaseStats(StringId(partyEntry.get<std::string>())));
        }
    }
    
    return true;
}

///-----------------------------------------------------------------------------------------------

float GetTerrainSpeedMultiplierAtPosition(const glm::vec3& position)
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& mapEntity = world.FindEntityWithName(MAP_ENTITY_NAME);
    const auto& heightMapComponent = world.GetComponent<genesis::rendering::HeightMapComponent>(mapEntity);
    
    const auto heightAtPosition = -genesis::rendering::GetTerrainHeightAtPosition(mapEntity, position);
    return 1.4f - (heightAtPosition/heightMapComponent.mHeightMapScale);
}

///------------------------------------------------------------------------------------------------

void AddCollidableDataToCityState(const genesis::ecs::EntityId cityStateEntity)
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& entityTransformComponent =
    world.GetComponent<genesis::TransformComponent>(cityStateEntity);
    const auto& entityRenderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(cityStateEntity);
    const auto& entityMeshResource = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::MeshResource>( entityRenderableComponent.mMeshResourceIds[entityRenderableComponent.mCurrentMeshResourceIndex]);
    const auto entityScaledDimensions = entityMeshResource.GetDimensions() * entityTransformComponent.mScale;
    const auto entitySphereRadius = (entityScaledDimensions.x + entityScaledDimensions.y + entityScaledDimensions.z) * ENTITY_SPHERE_COLLISION_MULTIPLIER;
    
    auto collidableComponent = std::make_unique<CollidableComponent>();
    collidableComponent->mCollidableDimensions.x = collidableComponent->mCollidableDimensions.y = collidableComponent->mCollidableDimensions.z = entitySphereRadius;
    
    world.AddComponent<CollidableComponent>(cityStateEntity, std::move(collidableComponent));
}

///------------------------------------------------------------------------------------------------

glm::vec3 GetCityStateOverworldScale(const StringId& cityStateName)
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& cityStateInfoComponent = world.GetSingletonComponent<CityStateInfoSingletonComponent>();
    return CITY_STATE_BASE_SCALE + CITY_STATE_SCALE_RENOWN_MULTIPLIER * static_cast<float>( cityStateInfoComponent.mCityStateNameToInfo.at(cityStateName).mRenown);
}

///-----------------------------------------------------------------------------------------------

void LoadAndCreateOverworldMapComponents()
{
    genesis::rendering::LoadAndCreateHeightMapByName(OVERWORLD_HEIGHTMAP_NAME, 0.07f, 1.0f, MAP_ENTITY_NAME);
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
    const auto& mapEntity = world.FindEntityWithName(MAP_ENTITY_NAME);
    
    for (const auto& cityStateInfoEntry: cityStateInfoComponent.mCityStateNameToInfo)
    {
        const auto cityStateZ = genesis::rendering::GetTerrainHeightAtPosition(mapEntity, cityStateInfoEntry.second.mPosition);
        const auto finalPosition = glm::vec3(cityStateInfoEntry.second.mPosition.x, cityStateInfoEntry.second.mPosition.y, cityStateZ);
        auto cityStateEntity = genesis::rendering::LoadAndCreateStaticModelByName(CITY_STATE_BUILDING_MODEL_NAME,    finalPosition, cityStateInfoEntry.second.mRotation, GetCityStateOverworldScale(cityStateInfoEntry.first), cityStateInfoEntry.first);
        
        auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(cityStateEntity);
        renderableComponent.mMaterial.mAmbient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        renderableComponent.mMaterial.mDiffuse = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
        renderableComponent.mMaterial.mSpecular = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
        renderableComponent.mMaterial.mShininess = 1.0f;
        renderableComponent.mIsAffectedByLight = true;
        renderableComponent.mIsCastingShadows = true;
        
        world.AddComponent<overworld::OverworldHighlightableComponent>(cityStateEntity, std::make_unique<overworld::OverworldHighlightableComponent>());
        
        AddCollidableDataToCityState(cityStateEntity);
    }
}

///-----------------------------------------------------------------------------------------------

void PrepareOverworldCamera()
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& playerPosition = world.GetComponent<genesis::TransformComponent>(overworld::GetPlayerEntity()).mPosition;
    world.RemoveSingletonComponent<genesis::rendering::CameraSingletonComponent>();
    world.SetSingletonComponent<genesis::rendering::CameraSingletonComponent>(std::make_unique<genesis::rendering::CameraSingletonComponent>());
    auto& cameraComponent = world.GetSingletonComponent<genesis::rendering::CameraSingletonComponent>();
    cameraComponent.mPosition.x = playerPosition.x;
    cameraComponent.mPosition.y = playerPosition.y;
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
    const auto mapEntity = GetMapEntity();
    
    auto& resourceLoadingService = genesis::resources::ResourceLoadingService::GetInstance();
    auto& mapRenderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(mapEntity);
    resourceLoadingService.UnloadResource(mapRenderableComponent.mTextureResourceId);
    
    world.DestroyEntity(world.FindEntityWithName(MAP_ENTITY_NAME));
    world.DestroyEntity(world.FindEntityWithName(MAP_EDGE_1_ENTITY_NAME));
    world.DestroyEntity(world.FindEntityWithName(MAP_EDGE_2_ENTITY_NAME));
    world.DestroyEntity(world.FindEntityWithName(MAP_EDGE_3_ENTITY_NAME));
    world.DestroyEntity(world.FindEntityWithName(MAP_EDGE_4_ENTITY_NAME));
}

///-----------------------------------------------------------------------------------------------

}



