///------------------------------------------------------------------------------------------------
///  HighlightingSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 21/03/2021.
///-----------------------------------------------------------------------------------------------

#include "HighlightingSystem.h"
#include "../components/HighlightableComponent.h"
#include "../../../engine/rendering/components/CameraSingletonComponent.h"
#include "../../../engine/rendering/components/RenderableComponent.h"
#include "../../../engine/rendering/components/WindowSingletonComponent.h"
#include "../../../engine/common/components/TransformComponent.h"
#include "../../../engine/common/utils/MathUtils.h"
#include "../../../engine/resources/MeshResource.h"
#include "../../../engine/resources/ResourceLoadingService.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

namespace
{
    static const StringId DEFAULT_SKELETAL_MODEL_SHADER     = StringId("default_skeletal_3d");
    static const StringId HIGHLIGHTED_SKELETAL_MODEL_SHADER = StringId("highlighted_skeletal_3d");
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
    auto& cameraComponent = world.GetSingletonComponent<genesis::rendering::CameraSingletonComponent>();
    auto& windowComponent = world.GetSingletonComponent<genesis::rendering::WindowSingletonComponent>();
    
    // Calculate render-constant camera view matrix
    auto mViewMatrix = glm::lookAtLH(cameraComponent.mPosition, cameraComponent.mPosition + cameraComponent.mFrontVector, cameraComponent.mUpVector);

    // Calculate render-constant camera projection matrix
    auto mProjectionMatrix = glm::perspectiveFovLH
    (
        cameraComponent.mFieldOfView,
        windowComponent.mRenderableWidth,
        windowComponent.mRenderableHeight,
        cameraComponent.mZNear,
        cameraComponent.mZFar
    );
    
    // Calculate mouse world ray direction
    auto rayDirection = genesis::math::ComputeMouseRayDirection(mViewMatrix, mProjectionMatrix, windowComponent.mRenderableWidth, windowComponent.mRenderableHeight);
    
    for (const auto entityId: entitiesToProcess)
    {
        auto& renderableComponent = world.GetComponent<genesis::rendering::RenderableComponent>(entityId);
        auto& highlightableComponent = world.GetComponent<HighlightableComponent>(entityId);
        
        const auto& transformComponent = world.GetComponent<genesis::TransformComponent>(entityId);
        const auto& meshResource = genesis::resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::MeshResource>( renderableComponent.mMeshResourceIds.at(renderableComponent.mCurrentMeshResourceIndex));
            const auto& scaledMeshDimensions = meshResource.GetDimensions() * transformComponent.mScale;
        const auto averageHalfDimension = (scaledMeshDimensions.x + scaledMeshDimensions.y + scaledMeshDimensions.z) * 0.333f * 0.5f;
        
        float t;
        const auto intersectionExists = genesis::math::RayToSphereIntersection(cameraComponent.mPosition, rayDirection, transformComponent.mPosition, averageHalfDimension, t);
        
        renderableComponent.mShaderNameId = intersectionExists ? HIGHLIGHTED_SKELETAL_MODEL_SHADER : DEFAULT_SKELETAL_MODEL_SHADER;
        highlightableComponent.mHighlighted = intersectionExists;
    }
}

///-----------------------------------------------------------------------------------------------

}
