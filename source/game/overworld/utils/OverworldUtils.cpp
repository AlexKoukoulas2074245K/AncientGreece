///------------------------------------------------------------------------------------------------
///  OverworldUtils.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 01/04/2021.
///------------------------------------------------------------------------------------------------

#include "OverworldUtils.h"
#include "../components/HighlightableComponent.h"
#include "../../components/CollidableComponent.h"
#include "../../components/CityStateInfoSingletonComponent.h"
#include "../../../engine/ECS.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/common/utils/ColorUtils.h"
#include "../../../engine/rendering/components/RenderableComponent.h"
#include "../../../engine/rendering/components/HeightMapComponent.h"
#include "../../../engine/rendering/utils/FontUtils.h"
#include "../../../engine/rendering/utils/MeshUtils.h"
#include "../../../engine/rendering/utils/HeightMapUtils.h"
#include "../../../engine/resources/MeshResource.h"
#include "../../../engine/resources/ResourceLoadingService.h"


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

    static const std::string OVERWORLD_HEIGHTMAP_NAME       = "overworld";
    static const std::string NAME_PLATE_MODEL_NAME          = "name_plate";
    static const std::string MAP_EDGE_MODEL_NAME            = "map_edge";
    static const std::string CITY_STATE_BUILDING_MODEL_NAME = "building";

    static const float HEIGHTMAP_Z_OFFSET                     = 0.01f;
    static const float ENTITY_SPHERE_COLLISION_MULTIPLIER     = 0.25f * 0.3333f;

    static const glm::vec3 MAP_DIMENSIONS                     = glm::vec3(1.0f, 1.0f, 1.0f);
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

void PopulateOverworldEntities()
{
    LoadAndCreateOverworldMapComponents();
    overworld::PopulateOverworldCityStates();
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

float GetTerrainHeightAtPosition(const glm::vec3& position)
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& mapEntity = world.FindEntityWithName(MAP_ENTITY_NAME);
    const auto& heightMapComponent = world.GetComponent<genesis::rendering::HeightMapComponent>(mapEntity);
    
    const auto relativeXDisplacement = position.x/(MAP_DIMENSIONS.x/2.0f);
    const auto relativeYDisplacement = position.y/(MAP_DIMENSIONS.y/2.0f);
    const auto heightMapCols = heightMapComponent.mHeightMapTextureDimensions.x;
    const auto heightMapRows = heightMapComponent.mHeightMapTextureDimensions.y;
    
    const auto heightMapCol = heightMapCols/2.0f + relativeXDisplacement * heightMapCols/2.0f;
    const auto heightMapRow = heightMapRows/2 - relativeYDisplacement * heightMapRows/2;
    
    if (heightMapCol >= 0 && heightMapRow >= 0 && heightMapCol < heightMapCols && heightMapRow < heightMapRows)
    {
        return heightMapComponent.mHeightMapTileHeights[heightMapRow][heightMapCols - heightMapCol] + HEIGHTMAP_Z_OFFSET;
    }
    return 0.0f;
}

///-----------------------------------------------------------------------------------------------

float GetTerrainSpeedMultiplierAtPosition(const glm::vec3& position)
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& mapEntity = world.FindEntityWithName(MAP_ENTITY_NAME);
    const auto& heightMapComponent = world.GetComponent<genesis::rendering::HeightMapComponent>(mapEntity);
    
    const auto heightAtPosition = GetTerrainHeightAtPosition(position);
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
    genesis::rendering::LoadAndCreateHeightMapByName(OVERWORLD_HEIGHTMAP_NAME, 0.07f, MAP_ENTITY_NAME);
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
        const auto cityStateZ = -GetTerrainHeightAtPosition(cityStateInfoEntry.second.mPosition);
        const auto finalPosition = glm::vec3(cityStateInfoEntry.second.mPosition.x, cityStateInfoEntry.second.mPosition.y, cityStateZ);
        auto cityStateEntity = genesis::rendering::LoadAndCreateStaticModelByName(CITY_STATE_BUILDING_MODEL_NAME,    finalPosition, cityStateInfoEntry.second.mRotation, GetCityStateOverworldScale(cityStateInfoEntry.first), cityStateInfoEntry.first);
        
        auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(cityStateEntity);
        renderableComponent.mMaterial.mAmbient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        renderableComponent.mMaterial.mDiffuse = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
        renderableComponent.mMaterial.mSpecular = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
        renderableComponent.mMaterial.mShininess = 1.0f;
        renderableComponent.mIsAffectedByLight = true;
        renderableComponent.mIsCastingShadows = true;
        
        world.AddComponent<overworld::HighlightableComponent>(cityStateEntity, std::make_unique<overworld::HighlightableComponent>());
        
        AddCollidableDataToCityState(cityStateEntity);
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



