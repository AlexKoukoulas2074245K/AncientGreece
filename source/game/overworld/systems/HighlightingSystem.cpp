///------------------------------------------------------------------------------------------------
///  HighlightingSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 21/03/2021.
///-----------------------------------------------------------------------------------------------

#include "HighlightingSystem.h"
#include "../components/HighlightableComponent.h"
#include "../components/OverworldMapPickingInfoSingletonComponent.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/common/utils/MathUtils.h"
#include "../../../engine/rendering/components/CameraSingletonComponent.h"
#include "../../../engine/rendering/components/RenderableComponent.h"
#include "../../../engine/rendering/components/TextStringComponent.h"
#include "../../../engine/rendering/components/WindowSingletonComponent.h"
#include "../../../engine/rendering/utils/Colors.h"
#include "../../../engine/rendering/utils/FontUtils.h"
#include "../../../engine/resources/MeshResource.h"
#include "../../../engine/resources/ResourceLoadingService.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const StringId DEFAULT_SKELETAL_MODEL_SHADER        = StringId("default_skeletal_3d");
    static const StringId HIGHLIGHTED_SKELETAL_MODEL_SHADER    = StringId("highlighted_skeletal_3d");
    static const StringId GUI_SHADER_CUSTOM_COLOR_UNIFORM_NAME = StringId("custom_color");
    
    static const glm::vec4 CITY_DEFAULT_COLOR     = genesis::rendering::colors::BLACK;
    static const glm::vec4 CITY_HIGHLIGHTED_COLOR = genesis::rendering::colors::GRAY;
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
    
    for (const auto entityId: entitiesToProcess)
    {
        auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(entityId);
        auto& highlightableComponent = world.GetComponent<HighlightableComponent>(entityId);
        
        const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entityId);
        
        // Differentiate between highlightable text strings and units
        if (world.HasComponent<genesis::rendering::TextStringComponent>(entityId))
        {
            const auto textRect = genesis::rendering::CalculateTextBoundingRect(entityId);
            renderableComponent.mShaderUniforms.mShaderFloatVec4Uniforms[GUI_SHADER_CUSTOM_COLOR_UNIFORM_NAME] = genesis::math::IsPointInsideRectangle(textRect.bottomLeft, textRect.topRight, glm::vec2(mapPickingInfoComponent.mMapIntersectionPoint.x, mapPickingInfoComponent.mMapIntersectionPoint.y)) ? CITY_HIGHLIGHTED_COLOR : CITY_DEFAULT_COLOR;
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
        }
    }
}

///-----------------------------------------------------------------------------------------------

}
