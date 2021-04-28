///------------------------------------------------------------------------------------------------
///  OverworldUtils.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 01/04/2021.
///------------------------------------------------------------------------------------------------

#include "OverworldUtils.h"
#include "../components/OverworldDayTimeSingletonComponent.h"
#include "../components/OverworldHighlightableComponent.h"
#include "../components/OverworldTargetComponent.h"
#include "../ai/components/OverworldUnitAiComponent.h"
#include "../ai/utils/AiUtils.h"
#include "../../components/CollidableComponent.h"
#include "../../components/CityStateInfoSingletonComponent.h"
#include "../../components/UnitStatsComponent.h"
#include "../../utils/CityStateInfoUtils.h"
#include "../../utils/UnitFactoryUtils.h"
#include "../../utils/UnitInfoUtils.h"
#include "../../../engine/ECS.h"
#include "../../../engine/common/components/NameComponent.h"
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
    static const StringId GAME_FONT_NAME                     = StringId("game_font");
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
    static const std::string UNIT_SHIP_MODEL_NAME           = "ship";
    static const std::string SAVE_FILE_PATH                 = "save.json";

    static const float ENTITY_SPHERE_COLLISION_MULTIPLIER     = 0.25f * 0.3333f;

    static const glm::vec3 CITY_STATE_BASE_SCALE              = glm::vec3(0.01f);
    static const glm::vec3 CITY_STATE_SCALE_RENOWN_MULTIPLIER = glm::vec3(0.0003f);
    static const glm::vec3 UNIT_SHIP_SCALE                    = glm::vec3(0.004f);
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

genesis::ecs::EntityId GetOverworldUnitEntityByName(const StringId& unitName)
{
    const auto& world = genesis::ecs::World::GetInstance();
    const auto allUnitEntities = world.FindAllEntitiesWithName(GENERIC_OVERWORLD_UNIT_ENTITY_NAME);
    
    for (const auto& entityId: allUnitEntities)
    {
        if (world.GetComponent<UnitStatsComponent>(entityId).mStats.mUnitName == unitName)
        {
            return entityId;
        }
    }
    
    if (GetPlayerUnitName() == unitName)
    {
        return GetPlayerEntity();
    }
    
    assert(false && "Unit with given name can't be found");
    
    return genesis::ecs::NULL_ENTITY_ID;
}

///------------------------------------------------------------------------------------------------

genesis::ecs::EntityId GetCityStateEntity(const StringId& cityStateName)
{
    return genesis::ecs::World::GetInstance().FindEntityWithName(cityStateName);
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

StringId GetShipEntityNameFromUnitName(const StringId& unitName)
{
    return StringId(unitName.GetString() + "_ship");
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
    const auto& cityStateInfoComponent = world.GetSingletonComponent<CityStateInfoSingletonComponent>();
    
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
    playerJsonObject["player_resting_duration"] = playerUnitStatsComponent.mStats.mCurrentRestingDuration;
    
    nlohmann::json playerEventTimeStampsJsonObject;
    for (const auto& unitEventTimestampEntry: playerUnitStatsComponent.mStats.mUnitEventTimestamps)
    {
        nlohmann::json eventTimeStampJsonObject;
        eventTimeStampJsonObject["event_name"] = unitEventTimestampEntry.first.GetString();
        eventTimeStampJsonObject["event_year"] = unitEventTimestampEntry.second.mYearBc;
        eventTimeStampJsonObject["event_day"] = unitEventTimestampEntry.second.mDay;
        eventTimeStampJsonObject["event_time_dt_accum"] = unitEventTimestampEntry.second.mTimeDtAccum;
        
        playerEventTimeStampsJsonObject.push_back(eventTimeStampJsonObject);
    }
    playerJsonObject["player_event_timestamps"] = playerEventTimeStampsJsonObject;
    
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
        const auto& unitAiComponent = world.GetComponent<ai::OverworldUnitAiComponent>(overworldUnitEntity);
        
        // Save core unit data
        unitJsonObject["name"] = unitStatsComponent.mStats.mUnitName.GetString();
        unitJsonObject["unit_type"] = unitStatsComponent.mStats.mUnitType.GetString();
        unitJsonObject["x"] = transformComponent.mPosition.x;
        unitJsonObject["y"] = transformComponent.mPosition.y;
        unitJsonObject["z"] = transformComponent.mPosition.z;
        unitJsonObject["rx"] = transformComponent.mRotation.x;
        unitJsonObject["ry"] = transformComponent.mRotation.y;
        unitJsonObject["rz"] = transformComponent.mRotation.z;
        unitJsonObject["resting_duration"] = unitStatsComponent.mStats.mCurrentRestingDuration;
        unitJsonObject["last_action_index"] = ai::GetAiActionIndex(unitAiComponent.mCurrentAction);
        
        // Save target data
        if (world.HasComponent<OverworldTargetComponent>(overworldUnitEntity))
        {
            const auto& targetComponent = world.GetComponent<OverworldTargetComponent>(overworldUnitEntity);
            if (targetComponent.mEntityTargetToFollow != genesis::ecs::NULL_ENTITY_ID)
            {
                if (world.HasComponent<UnitStatsComponent>(targetComponent.mEntityTargetToFollow))
                {
                    unitJsonObject["target_unit_name"] = world.GetComponent<UnitStatsComponent>(targetComponent.mEntityTargetToFollow).mStats.mUnitName.GetString();
                }
                else
                {
                    unitJsonObject["target_city_state_name"] = world.GetComponent<genesis::NameComponent>(targetComponent.mEntityTargetToFollow).mName.GetString();
                }
            }
            else
            {
                nlohmann::json targetPositionJsonObject;
                targetPositionJsonObject["x"] = targetComponent.mTargetPosition.x;
                targetPositionJsonObject["y"] = targetComponent.mTargetPosition.y;
                targetPositionJsonObject["z"] = targetComponent.mTargetPosition.z;
                
                unitJsonObject["target_position"] = targetPositionJsonObject;
            }
        }
        
        // Save last unit event timestamp data
        nlohmann::json eventTimeStampsJsonObject;
        for (const auto& unitEventTimestampEntry: unitStatsComponent.mStats.mUnitEventTimestamps)
        {
            nlohmann::json eventTimeStampJsonObject;
            eventTimeStampJsonObject["event_name"] = unitEventTimestampEntry.first.GetString();
            eventTimeStampJsonObject["event_year"] = unitEventTimestampEntry.second.mYearBc;
            eventTimeStampJsonObject["event_day"] = unitEventTimestampEntry.second.mDay;
            eventTimeStampJsonObject["event_time_dt_accum"] = unitEventTimestampEntry.second.mTimeDtAccum;
            
            eventTimeStampsJsonObject.push_back(eventTimeStampJsonObject);
        }
        unitJsonObject["event_timestamps"] = eventTimeStampsJsonObject;
        
        // Save party data
        nlohmann::json partyJsonObject;
        for (auto i = 1U; i < unitStatsComponent.mParty.size(); ++i)
        {
            const auto& partyUnitStats = unitStatsComponent.mParty[i];
            partyJsonObject.push_back(partyUnitStats.mUnitType.GetString());
        }
        
        unitJsonObject["party"] = partyJsonObject;
        
        overworldUnitsJsonObject.push_back(unitJsonObject);
    }
    
    nlohmann::json cityStatesJsonObject;
    for (const auto& entry: cityStateInfoComponent.mCityStateNameToInfo)
    {
        nlohmann::json cityStateInfoJsonObject;
        cityStateInfoJsonObject["name"] = entry.first.GetString();
        cityStateInfoJsonObject["renown"] = entry.second.mRenown;
        cityStateInfoJsonObject["garisson"] = entry.second.mGarisson;
        cityStateInfoJsonObject["ruler"] = entry.second.mRuler.GetString();
        
        cityStatesJsonObject.push_back(cityStateInfoJsonObject);
    }
    
    saveFileRoot["overworld_state"] = overworldStateJsonObject;
    saveFileRoot["player"] = playerJsonObject;
    saveFileRoot["overworld_units"] = overworldUnitsJsonObject;
    saveFileRoot["city_states_info"] = cityStatesJsonObject;
    
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
    auto playerShipEntity = genesis::rendering::LoadAndCreateStaticModelByName(UNIT_SHIP_MODEL_NAME, glm::vec3(0.0f), glm::vec3(0.0f), UNIT_SHIP_SCALE, GetShipEntityNameFromUnitName(playerUnitName));
    
    auto& playerShipRenderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(playerShipEntity);
    playerShipRenderableComponent.mIsVisible           = false;
    playerShipRenderableComponent.mIsCastingShadows    = true;
    playerShipRenderableComponent.mIsAffectedByLight   = true;
    playerShipRenderableComponent.mMaterial.mAmbient   = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    playerShipRenderableComponent.mMaterial.mDiffuse   = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
    playerShipRenderableComponent.mMaterial.mSpecular  = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    playerShipRenderableComponent.mMaterial.mShininess = 1.0f;
    
    auto& playerUnitStatsComponent = world.GetComponent<UnitStatsComponent>(playerEntity);
    playerUnitStatsComponent.mStats.mSpeedMultiplier *= 3.0f;
    
    // Parse event timestamps for player
    for (auto& eventTimeStampJsonObject: playerJsonObject["player_event_timestamps"])
    {
        playerUnitStatsComponent.mStats.mUnitEventTimestamps[StringId(eventTimeStampJsonObject["event_name"])] = TimeStamp
        (
            eventTimeStampJsonObject["event_year"].get<int>(),
            eventTimeStampJsonObject["event_day"].get<int>(),
            eventTimeStampJsonObject["event_time_dt_accum"].get<float>()
        );
    }
    
    playerUnitStatsComponent.mStats.mCurrentRestingDuration = playerJsonObject["player_resting_duration"].get<float>();
    
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
        auto unitShipEntity = genesis::rendering::LoadAndCreateStaticModelByName(UNIT_SHIP_MODEL_NAME, glm::vec3(0.0f), glm::vec3(0.0f), UNIT_SHIP_SCALE, GetShipEntityNameFromUnitName(unitName));
        
        auto& shipRenderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(unitShipEntity);
        
        shipRenderableComponent.mIsVisible           = false;
        shipRenderableComponent.mIsAffectedByLight   = true;
        shipRenderableComponent.mIsCastingShadows    = true;
        shipRenderableComponent.mMaterial.mAmbient   = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        shipRenderableComponent.mMaterial.mDiffuse   = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
        shipRenderableComponent.mMaterial.mSpecular  = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        shipRenderableComponent.mMaterial.mShininess = 1.0f;
        
        auto& unitAiComponent = world.GetComponent<ai::OverworldUnitAiComponent>(unitEntity);
        unitAiComponent.mLastActionIndex = overworldUnitJsonObject["last_action_index"].get<int>();
        
        auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(unitEntity);
        
        // Parse event timestamps for unit
        for (auto& eventTimeStampJsonObject: overworldUnitJsonObject["event_timestamps"])
        {
            unitStatsComponent.mStats.mUnitEventTimestamps[StringId(eventTimeStampJsonObject["event_name"])] = TimeStamp
            (
                eventTimeStampJsonObject["event_year"].get<int>(),
                eventTimeStampJsonObject["event_day"].get<int>(),
                eventTimeStampJsonObject["event_time_dt_accum"].get<float>()
            );
        }
        
        // Parse unit's party
        for (const auto& partyEntry: overworldUnitJsonObject["party"])
        {
            unitStatsComponent.mParty.push_back(GetUnitBaseStats(StringId(partyEntry.get<std::string>())));
        }
    }
    
    // Reparse units to establish target mappings
    for (const auto& overworldUnitJsonObject: saveFileJsonRoot["overworld_units"])
    {
        const auto unitName = StringId(overworldUnitJsonObject["name"].get<std::string>());
        const auto unitEntity = GetOverworldUnitEntityByName(unitName);
        
        if (overworldUnitJsonObject.count("target_position"))
        {
            auto targetComponent = std::make_unique<OverworldTargetComponent>();
            targetComponent->mTargetPosition = glm::vec3
            (
                overworldUnitJsonObject["target_position"]["x"].get<float>(),
                overworldUnitJsonObject["target_position"]["y"].get<float>(),
                overworldUnitJsonObject["target_position"]["z"].get<float>()
            );
            
            world.AddComponent<OverworldTargetComponent>(unitEntity, std::move(targetComponent));
        }
        else if (overworldUnitJsonObject.count("target_unit_name"))
        {
            auto targetComponent = std::make_unique<OverworldTargetComponent>();
            targetComponent->mEntityTargetToFollow = GetOverworldUnitEntityByName(StringId(overworldUnitJsonObject["target_unit_name"].get<std::string>()));
            world.AddComponent<OverworldTargetComponent>(unitEntity, std::move(targetComponent));
        }
        else if (overworldUnitJsonObject.count("target_city_state_name"))
        {
            auto targetComponent = std::make_unique<OverworldTargetComponent>();
            targetComponent->mEntityTargetToFollow = GetCityStateEntity(StringId(overworldUnitJsonObject["target_city_state_name"].get<std::string>()));
            world.AddComponent<OverworldTargetComponent>(unitEntity, std::move(targetComponent));
        }
    }
    
    // Parse city state changes
    for (const auto& cityStateInfoJsonObject: saveFileJsonRoot["city_states_info"])
    {
        const auto cityStateName = StringId(cityStateInfoJsonObject["name"].get<std::string>());
        auto& cityStateInfo = GetCityStateInfo(cityStateName);
        
        cityStateInfo.mGarisson = cityStateInfoJsonObject["garisson"].get<int>();
        cityStateInfo.mRenown   = cityStateInfoJsonObject["renown"].get<int>();
        cityStateInfo.mRuler    = StringId(cityStateInfoJsonObject["ruler"].get<std::string>());
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
        
        auto cityStateScale = GetCityStateOverworldScale(cityStateInfoEntry.first);
        cityStateScale.y *= 2.0f;
        
        auto cityStateEntity = genesis::rendering::LoadAndCreateStaticModelByName(CITY_STATE_BUILDING_MODEL_NAME,    finalPosition, cityStateInfoEntry.second.mRotation, cityStateScale, cityStateInfoEntry.first);
        
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



