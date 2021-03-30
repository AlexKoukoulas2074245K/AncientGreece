///------------------------------------------------------------------------------------------------
///  HighlightingSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 21/03/2021.
///-----------------------------------------------------------------------------------------------

#include "HighlightingSystem.h"
#include "../components/HighlightableComponent.h"
#include "../components/OverworldMapPickingInfoSingletonComponent.h"
#include "../../components/UnitStatsComponent.h"
#include "../../utils/UnitInfoUtils.h"
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
    static const StringId GUI_SHADER_CUSTOM_COLOR_UNIFORM_NAME = StringId("custom_color");
    static const StringId UNIT_PREVIEW_POPUP_NAME              = StringId("unit_preview_popup");
    static const StringId GAME_FONT_NAME                       = StringId("ancient_greek_font");
    
    static const float NAME_PLATE_Z                   = -0.03f;
    static const float NAME_PLATE_X_OFFSET_MULTIPLIER = 1.0/20.0f;
    static const float NAME_PLATE_HEIGHT_MULTIPLIER   = 1.5f;
    static const float NAME_PLATE_WIDTH_MULTIPLIER    = 1.2f;
    static const float UNIT_NAME_SIZE                 = 0.005f;
    static const float PARTY_X_OFFSET                 = UNIT_NAME_SIZE * 4.8;
    static const float UNIT_NAME_Z                    = -0.035f;
    static const float UNIT_DETAILS_Y_OFFSET          = 0.005f;

    static const glm::vec4 CITY_DEFAULT_COLOR     = genesis::colors::BLACK;
    static const glm::vec4 CITY_HIGHLIGHTED_COLOR = genesis::colors::GRAY;
}

///-----------------------------------------------------------------------------------------------

HighlightingSystem::HighlightingSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void HighlightingSystem::VUpdate(const float, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& mapPickingInfoComponent = world.GetSingletonComponent<OverworldMapPickingInfoSingletonComponent>();
    const auto& cameraComponent = world.GetSingletonComponent<genesis::rendering::CameraSingletonComponent>();
    
    DestroyUnitPreviewPopup();
    
    for (const auto entityId: entitiesToProcess)
    {
        world.GetComponent<HighlightableComponent>(entityId).mHighlighted = false;
    }
    
    for (const auto entityId: entitiesToProcess)
    {
        auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(entityId);
        auto& highlightableComponent = world.GetComponent<HighlightableComponent>(entityId);
        
        const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entityId);
        
        // Differentiate between highlightable text strings and units
        if (world.HasComponent<genesis::rendering::TextStringComponent>(entityId))
        {
            const auto textRect = genesis::rendering::CalculateTextBoundingRect(entityId);
            const auto intersectionExists =  genesis::math::IsPointInsideRectangle(textRect.bottomLeft, textRect.topRight, glm::vec2(mapPickingInfoComponent.mMapIntersectionPoint.x, mapPickingInfoComponent.mMapIntersectionPoint.y));
            
            renderableComponent.mShaderUniforms.mShaderFloatVec4Uniforms[GUI_SHADER_CUSTOM_COLOR_UNIFORM_NAME] =  intersectionExists ? CITY_HIGHLIGHTED_COLOR : CITY_DEFAULT_COLOR;
            
            highlightableComponent.mHighlighted = intersectionExists;
            if (highlightableComponent.mHighlighted)
            {
                break;
            }
        }
        else
        {
            const auto& meshResource = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::MeshResource>( renderableComponent.mMeshResourceIds.at(renderableComponent.mCurrentMeshResourceIndex));
                const auto& scaledMeshDimensions = meshResource.GetDimensions() * transformComponent.mScale;
            const auto averageHalfDimension = (scaledMeshDimensions.x + scaledMeshDimensions.y + scaledMeshDimensions.z) * 0.333f * 0.5f;
            
            float t;
            const auto intersectionExists = genesis::math::RayToSphereIntersection(cameraComponent.mPosition, mapPickingInfoComponent.mMouseRayDirection, transformComponent.mPosition, averageHalfDimension, t);
            
            renderableComponent.mShaderNameId = intersectionExists ? HIGHLIGHTED_SKELETAL_MODEL_SHADER : DEFAULT_SKELETAL_MODEL_SHADER;
            highlightableComponent.mHighlighted = intersectionExists;
            
            if (highlightableComponent.mHighlighted)
            {
                const auto& unitStatsComponent = world.GetComponent<UnitStatsComponent>(entityId);
                CreateUnitPreviewPopup(transformComponent.mPosition, unitStatsComponent, cameraComponent.mPosition);
            }
        }
    }
}


///-----------------------------------------------------------------------------------------------

void HighlightingSystem::CreateUnitPreviewPopup(const glm::vec3& unitPosition, const UnitStatsComponent& unitStatsComponent, const glm::vec3& /*cameraPosition*/) const
{
    auto& world = genesis::ecs::World::GetInstance();
    
    std::vector<genesis::ecs::EntityId> renderedTextEntities;
    renderedTextEntities.push_back(genesis::rendering::RenderText(unitStatsComponent.mStats.mUnitName.GetString(), GAME_FONT_NAME, UNIT_NAME_SIZE, unitPosition + glm::vec3(0.0f, UNIT_DETAILS_Y_OFFSET + 2 * UNIT_NAME_SIZE, UNIT_NAME_Z), genesis::colors::BLACK, true, UNIT_PREVIEW_POPUP_NAME));
    renderedTextEntities.push_back(genesis::rendering::RenderText("Party:", GAME_FONT_NAME, UNIT_NAME_SIZE, unitPosition + glm::vec3(0.0f, UNIT_DETAILS_Y_OFFSET + UNIT_NAME_SIZE, UNIT_NAME_Z), genesis::colors::BLACK, true, UNIT_PREVIEW_POPUP_NAME));
    renderedTextEntities.push_back(genesis::rendering::RenderText(std::to_string(unitStatsComponent.mParty.size()), GAME_FONT_NAME, UNIT_NAME_SIZE, unitPosition + glm::vec3(PARTY_X_OFFSET, UNIT_DETAILS_Y_OFFSET + UNIT_NAME_SIZE, UNIT_NAME_Z), genesis::colors::RgbTripletToVec4( GetUnitPartyColor(unitStatsComponent)), true, UNIT_PREVIEW_POPUP_NAME));
    
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
    
    genesis::rendering::LoadAndCreateStaticModelByName(NAME_PLATE_MODEL_NAME, glm::vec3(minBotLeftCoords.x + maxRectWidth/2 - maxRectWidth * NAME_PLATE_X_OFFSET_MULTIPLIER, minBotLeftCoords.y + maxRectHeight/2.0, NAME_PLATE_Z), glm::vec3(), glm::vec3(maxRectWidth * NAME_PLATE_WIDTH_MULTIPLIER, maxRectHeight * NAME_PLATE_HEIGHT_MULTIPLIER, 1.0f), UNIT_PREVIEW_POPUP_NAME);
}

///-----------------------------------------------------------------------------------------------

void HighlightingSystem::DestroyUnitPreviewPopup() const
{
    auto& world = genesis::ecs::World::GetInstance();
    world.DestroyEntities(world.FindAllEntitiesWithName(UNIT_PREVIEW_POPUP_NAME));
}

///-----------------------------------------------------------------------------------------------

}
