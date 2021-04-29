///------------------------------------------------------------------------------------------------
///  OverworldHighlightingSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 21/03/2021.
///-----------------------------------------------------------------------------------------------

#include "OverworldHighlightingSystem.h"
#include "../components/OverworldHighlightableComponent.h"
#include "../components/OverworldMapPickingInfoSingletonComponent.h"
#include "../ai/components/OverworldUnitAiComponent.h"
#include "../../components/UnitStatsComponent.h"
#include "../../utils/CityStateInfoUtils.h"
#include "../../utils/UnitInfoUtils.h"
#include "../../../engine/common/components/NameComponent.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/common/utils/ColorUtils.h"
#include "../../../engine/common/utils/MathUtils.h"
#include "../../../engine/rendering/components/CameraSingletonComponent.h"
#include "../../../engine/rendering/components/RenderableComponent.h"
#include "../../../engine/rendering/components/TextStringComponent.h"
#include "../../../engine/rendering/components/WindowSingletonComponent.h"
#include "../../../engine/rendering/utils/FontUtils.h"
#include "../../../engine/rendering/utils/MeshUtils.h"
#include "../../../engine/resources/MeshResource.h"
#include "../../../engine/resources/ResourceLoadingService.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const std::string NAME_PLATE_MODEL_NAME = "name_plate";

    static const StringId DEFAULT_SKELETAL_MODEL_SHADER        = StringId("default_skeletal_3d");
    static const StringId HIGHLIGHTED_SKELETAL_MODEL_SHADER    = StringId("highlighted_skeletal_3d");
    static const StringId DEFAULT_STATIC_MODEL_SHADER          = StringId("default_3d");
    static const StringId HIGHLIGHTED_STATIC_MODEL_SHADER      = StringId("highlighted_3d");
    static const StringId GUI_SHADER_CUSTOM_COLOR_UNIFORM_NAME = StringId("custom_color");
    static const StringId PREVIEW_POPUP_NAME                   = StringId("preview_popup");
    
    static const StringId GAME_FONT_NAME                       = StringId("game_font");
    
    static const float NAME_PLATE_Z                    = -0.032f;
    static const float NAME_PLATE_Y_OFFSET             = -0.001f;
    static const float NAME_PLATE_X_OFFSET_MULTIPLIER  = 1.0/20.0f;
    static const float NAME_PLATE_HEIGHT_MULTIPLIER    = 1.5f;
    static const float NAME_PLATE_WIDTH_MULTIPLIER     = 1.2f;
    static const float UNIT_NAME_SIZE                  = 0.005f;
    static const float PARTY_X_OFFSET                  = UNIT_NAME_SIZE * 4.8;
    static const float UNIT_NAME_Z                     = -0.033f;
    static const float UNIT_DETAILS_Y_OFFSET           = 0.005f;
    static const float CITY_STATE_NAME_Z               = -0.035f;
    static const float CITY_STATE_DETAILS_Y_OFFSET     = 0.0005f;

}

///-----------------------------------------------------------------------------------------------

OverworldHighlightingSystem::OverworldHighlightingSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void OverworldHighlightingSystem::VUpdate(const float, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& mapPickingInfoComponent = world.GetSingletonComponent<OverworldMapPickingInfoSingletonComponent>();
   const auto& cameraComponent = world.GetSingletonComponent<genesis::rendering::CameraSingletonComponent>();
    
    DestroyUnitPreviewPopup();
    for (const auto entityId: entitiesToProcess)
    {
        auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(entityId);
        world.GetComponent<OverworldHighlightableComponent>(entityId).mHighlighted = false;
        if (world.HasComponent<UnitStatsComponent>(entityId))
        {
            renderableComponent.mShaderNameId = DEFAULT_SKELETAL_MODEL_SHADER;
        }
        else
        {
            renderableComponent.mShaderNameId = DEFAULT_STATIC_MODEL_SHADER;
        }
    }
    
    std::vector<genesis::ecs::EntityId> highlightedCandidates;
    
    for (const auto entityId: entitiesToProcess)
    {
        auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(entityId);
        const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entityId);
        const auto& meshResource = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::MeshResource>( renderableComponent.mMeshResourceIds.at(renderableComponent.mCurrentMeshResourceIndex));
            const auto& scaledMeshDimensions = meshResource.GetDimensions() * transformComponent.mScale;
        const auto averageHalfDimension = (scaledMeshDimensions.x + scaledMeshDimensions.y + scaledMeshDimensions.z) * 0.333f * 0.75f;

        float t;
        if (genesis::math::RayToSphereIntersection(cameraComponent.mPosition, mapPickingInfoComponent.mMouseRayDirection, transformComponent.mPosition, averageHalfDimension, t))
        {
            highlightedCandidates.push_back(entityId);
        }
    }
    
    // Sort highlighted candidates by closest to mouse position
    std::sort(highlightedCandidates.begin(), highlightedCandidates.end(), [&](const genesis::ecs::EntityId& lhs, const genesis::ecs::EntityId& rhs)
    {
        return glm::distance(mapPickingInfoComponent.mMapIntersectionPoint, world.GetComponent<genesis::TransformComponent>(lhs).mPosition) < glm::distance(mapPickingInfoComponent.mMapIntersectionPoint, world.GetComponent<genesis::TransformComponent>(rhs).mPosition);
    });
    
    for (const auto& entityId: highlightedCandidates)
    {
        const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entityId);
        auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(entityId);
        auto& highlightableComponent = world.GetComponent<OverworldHighlightableComponent>(entityId);
        highlightableComponent.mHighlighted = true;
        
        // Unit highlighted
        if (world.HasComponent<UnitStatsComponent>(entityId))
        {
            renderableComponent.mShaderNameId = HIGHLIGHTED_SKELETAL_MODEL_SHADER;
            const auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
            
            auto behaviourDescription = StringId();
            if (world.HasComponent<ai::OverworldUnitAiComponent>(entityId))
            {
                behaviourDescription = ai::BEHAVIOUR_STATE_TO_STRING.at( world.GetComponent<ai::OverworldUnitAiComponent>(entityId).mBehaviourState);
            }
            CreateUnitPreviewPopup(transformComponent.mPosition, behaviourDescription, unitStatsComponent);
        }
        // City state highlighted
        else
        {
            renderableComponent.mShaderNameId = HIGHLIGHTED_STATIC_MODEL_SHADER;
            const auto& cityStateName = world.GetComponent<genesis::NameComponent>(entityId);
            CreateCityStatePreviewPopup(transformComponent.mPosition, cityStateName.mName);
        }

        break;
    }
}

///-----------------------------------------------------------------------------------------------

void OverworldHighlightingSystem::CreateUnitPreviewPopup(const glm::vec3& unitPosition, const StringId& behaviourDescription, const UnitStatsComponent& unitStatsComponent) const
{
    auto& world = genesis::ecs::World::GetInstance();
    
    std::vector<genesis::ecs::EntityId> renderedTextEntities;
    auto textEntryYOffset = 3.0f * UNIT_NAME_SIZE;
    
    renderedTextEntities.push_back(genesis::rendering::RenderText(unitStatsComponent.mStats.mUnitName.GetString(), GAME_FONT_NAME, UNIT_NAME_SIZE, unitPosition + glm::vec3(0.0f, UNIT_DETAILS_Y_OFFSET + textEntryYOffset, UNIT_NAME_Z), genesis::colors::BLACK, true, PREVIEW_POPUP_NAME));
    textEntryYOffset -= UNIT_NAME_SIZE;
    
    if (behaviourDescription != StringId())
    {
        renderedTextEntities.push_back(genesis::rendering::RenderText(behaviourDescription.GetString(), GAME_FONT_NAME, UNIT_NAME_SIZE, unitPosition + glm::vec3(0.0f, UNIT_DETAILS_Y_OFFSET + textEntryYOffset, UNIT_NAME_Z), genesis::colors::BLACK, true, PREVIEW_POPUP_NAME));
        
        textEntryYOffset -= UNIT_NAME_SIZE;
    }
    
    renderedTextEntities.push_back(genesis::rendering::RenderText("Party:", GAME_FONT_NAME, UNIT_NAME_SIZE, unitPosition + glm::vec3(0.0f, UNIT_DETAILS_Y_OFFSET + textEntryYOffset, UNIT_NAME_Z), genesis::colors::BLACK, true, PREVIEW_POPUP_NAME));
    renderedTextEntities.push_back(genesis::rendering::RenderText(std::to_string(GetUnitPartySize(unitStatsComponent)), GAME_FONT_NAME, UNIT_NAME_SIZE, unitPosition + glm::vec3(PARTY_X_OFFSET, UNIT_DETAILS_Y_OFFSET + textEntryYOffset, UNIT_NAME_Z), genesis::colors::RgbTripletToVec4( GetUnitPartyColor(unitStatsComponent)), true, PREVIEW_POPUP_NAME));
    
    glm::vec2 minBotLeftCoords(1.0f, 1.0f);
    glm::vec2 maxTopRightCoords(-1.0f, -1.0f);
    
    for (auto entity: renderedTextEntities)
    {
        const auto textRect = genesis::rendering::CalculateTextBoundingRect(entity);
        if (minBotLeftCoords.x > textRect.bottomLeft.x) minBotLeftCoords.x = textRect.bottomLeft.x;
        if (minBotLeftCoords.y > textRect.bottomLeft.y) minBotLeftCoords.y = textRect.bottomLeft.y;
        if (maxTopRightCoords.x < textRect.topRight.x) maxTopRightCoords.x = textRect.topRight.x;
        if (maxTopRightCoords.y < textRect.topRight.y) maxTopRightCoords.y = textRect.topRight.y;
    }
    
    const auto maxRectWidth = maxTopRightCoords.x - minBotLeftCoords.x;
    const auto maxRectHeight = maxTopRightCoords.y - minBotLeftCoords.y;
    
    for (auto entity: renderedTextEntities)
    {
        auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entity);
        transformComponent.mPosition.x -= maxRectWidth * 0.5f;
    }
    
    maxTopRightCoords.x -= maxRectWidth * 0.5f;
    minBotLeftCoords.x  -= maxRectWidth * 0.5f;
    
    genesis::rendering::LoadAndCreateStaticModelByName(NAME_PLATE_MODEL_NAME, glm::vec3(minBotLeftCoords.x + maxRectWidth/2 - maxRectWidth * NAME_PLATE_X_OFFSET_MULTIPLIER, minBotLeftCoords.y + maxRectHeight/2.0 + NAME_PLATE_Y_OFFSET, unitPosition.z + NAME_PLATE_Z), glm::vec3(), glm::vec3(maxRectWidth * NAME_PLATE_WIDTH_MULTIPLIER, maxRectHeight * NAME_PLATE_HEIGHT_MULTIPLIER, 1.0f), PREVIEW_POPUP_NAME);
}

///-----------------------------------------------------------------------------------------------

void OverworldHighlightingSystem::CreateCityStatePreviewPopup(const glm::vec3& cityStatePosition, const StringId& cityStateName) const
{
    const auto cityStateNameSize = GetCityStateNameSize(cityStateName);
    auto nameTextEntity = genesis::rendering::RenderText(cityStateName.GetString(), GAME_FONT_NAME, cityStateNameSize, cityStatePosition + glm::vec3(0.0f, CITY_STATE_DETAILS_Y_OFFSET + 2 * cityStateNameSize, CITY_STATE_NAME_Z), genesis::colors::BLACK, true, PREVIEW_POPUP_NAME);
    
    const auto firstTextRect = genesis::rendering::CalculateTextBoundingRect(nameTextEntity);
    const auto firstTextRectWidth = firstTextRect.topRight.x - firstTextRect.bottomLeft.x;
    
    auto& transformComponent = genesis::ecs::World::GetInstance().GetComponent<genesis::TransformComponent>(nameTextEntity);
    transformComponent.mPosition.x -= firstTextRectWidth/2.0f;
    
    const auto textRect = genesis::rendering::CalculateTextBoundingRect(nameTextEntity);
    const auto textRectWidth = textRect.topRight.x - textRect.bottomLeft.x;
    const auto textRectHeight = textRect.topRight.y - textRect.bottomLeft.y;
    
    genesis::rendering::LoadAndCreateStaticModelByName(NAME_PLATE_MODEL_NAME, glm::vec3(textRect.bottomLeft.x + textRectWidth/2 - textRectWidth * NAME_PLATE_X_OFFSET_MULTIPLIER, textRect.bottomLeft.y + textRectHeight/2.0 + NAME_PLATE_Y_OFFSET, cityStatePosition.z + NAME_PLATE_Z), glm::vec3(), glm::vec3(textRectWidth * NAME_PLATE_WIDTH_MULTIPLIER, textRectHeight * NAME_PLATE_HEIGHT_MULTIPLIER, 1.0f), PREVIEW_POPUP_NAME);
}

///-----------------------------------------------------------------------------------------------

void OverworldHighlightingSystem::DestroyUnitPreviewPopup() const
{
    auto& world = genesis::ecs::World::GetInstance();
    world.DestroyEntities(world.FindAllEntitiesWithName(PREVIEW_POPUP_NAME));
}

///-----------------------------------------------------------------------------------------------

}
