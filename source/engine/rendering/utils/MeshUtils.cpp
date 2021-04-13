///------------------------------------------------------------------------------------------------
///  MeshUtils.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 09/01/2020.
///------------------------------------------------------------------------------------------------

#include "MeshUtils.h"
#include "../components/RenderableComponent.h"
#include "../../animation/utils/AnimationUtils.h"
#include "../../common/utils/ColorUtils.h"
#include "../../common/components/NameComponent.h"
#include "../../common/components/TransformComponent.h"
#include "../../common/utils/FileUtils.h"
#include "../../common/utils/MathUtils.h"
#include "../../common/utils/StringUtils.h"
#include "../../resources/MeshResource.h"

#include <vector>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace rendering
{

///------------------------------------------------------------------------------------------------

namespace
{
    static const StringId DEFAULT_SKELETAL_MODEL_SHADER        = StringId("default_skeletal_3d");
    static const StringId GUI_ANIMATED_MODEL_3D_SHADER_NAME    = StringId("default_gui_skeletal_3d");
    static const StringId DEFAULT_MODEL_SHADER                 = StringId("default_3d");
    static const StringId ATLAS_MODEL_NAME                     = StringId("gui_atlas_quad");
    static const StringId BONES_UNIFORM_NAME                   = StringId("bones");
    static const StringId GUI_SHADER_CUSTOM_COLOR_UNIFORM_NAME = StringId("custom_color");
    static const StringId IDLE_ANIMATION_NAME                  = StringId("idle");
}

///------------------------------------------------------------------------------------------------

static std::vector<glm::vec2> CalculateTextureCoordsFromColumnAndRow
(
    const int col,
    const int row,
    const int atlasNumberOfCols,
    const int atlasNumberOfRows,
    const bool horFlipped
);

static std::string CreateTexCoordInjectedModelPath
(    
    const std::vector<glm::vec2>& texCoords
);

///------------------------------------------------------------------------------------------------

ecs::EntityId LoadAndCreateStaticModelByName
(
    const std::string& modelName,
    const glm::vec3& initialPosition /* glm::vec3(0.0f, 0.0f, 0.0f) */,
    const glm::vec3& initialRotation /* glm::vec3(0.0f, 0.0f, 0.0f) */,
    const glm::vec3& initialScale /* glm::vec3(1.0f, 1.0f, 1.0f) */,
    const StringId entityName /* StringId() */
)
{
    auto& world = ecs::World::GetInstance();
    const auto modelEntity = world.CreateEntity();

    auto transformComponent = std::make_unique<TransformComponent>();
    transformComponent->mPosition = initialPosition;
    transformComponent->mRotation = initialRotation;
    transformComponent->mScale = initialScale;

    auto renderableComponent = std::make_unique<RenderableComponent>();        
    renderableComponent->mShaderNameId = DEFAULT_MODEL_SHADER;
    renderableComponent->mMeshResourceIds.push_back(
        resources::ResourceLoadingService::GetInstance().
        LoadResource(resources::ResourceLoadingService::RES_MODELS_ROOT + modelName + ".obj"));
    renderableComponent->mTextureResourceId = resources::ResourceLoadingService::GetInstance().LoadResource
    (
        resources::ResourceLoadingService::RES_TEXTURES_ROOT + modelName + ".png"
    );
    
    world.AddComponent<RenderableComponent>(modelEntity, std::move(renderableComponent));
    world.AddComponent<TransformComponent>(modelEntity, std::move(transformComponent));

    if (entityName != StringId())
    {
        world.AddComponent<NameComponent>(modelEntity, std::make_unique<NameComponent>(entityName));
    }

    return modelEntity;
}

///------------------------------------------------------------------------------------------------

ecs::EntityId LoadAndCreateAnimatedModelByName
(
    const std::string& modelName,
    const glm::vec3& initialPosition /* glm::vec3(0.0f, 0.0f, 0.0f) */,
    const glm::vec3& initialRotation /* glm::vec3(0.0f, 0.0f, 0.0f) */,
    const glm::vec3& initialScale /* glm::vec3(1.0f, 1.0f, 1.0f) */,
    const StringId entityName /* StringId() */,
    const float randomizationAnimationFactor /* false */,
    const bool isGui /* false */
)
{
    auto& world = ecs::World::GetInstance();
    const auto modelEntity = world.CreateEntity();

    auto transformComponent = std::make_unique<TransformComponent>();
    transformComponent->mPosition = initialPosition;
    transformComponent->mRotation = initialRotation;
    transformComponent->mScale = initialScale;

    auto renderableComponent = std::make_unique<RenderableComponent>();
    renderableComponent->mRenderableType = isGui ? RenderableType::GUI_3D_MODEL : RenderableType::NORMAL_MODEL;
    renderableComponent->mShaderNameId = isGui ? GUI_ANIMATED_MODEL_3D_SHADER_NAME: DEFAULT_SKELETAL_MODEL_SHADER;
    renderableComponent->mAnimationSpeed *= randomizationAnimationFactor;
    
    if (!isGui)
    {
        renderableComponent->mMaterial.mAmbient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        renderableComponent->mMaterial.mDiffuse = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);
        renderableComponent->mMaterial.mSpecular = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        renderableComponent->mMaterial.mShininess = 1.0f;
        renderableComponent->mIsAffectedByLight = true;
        renderableComponent->mIsCastingShadows = true;
    }
    
    auto animFiles = GetAllFilenamesInDirectory(resources::ResourceLoadingService::RES_MODELS_ROOT + modelName + "/");
    for (const auto& fileName: animFiles)
    {
        auto meshResourceId = resources::ResourceLoadingService::GetInstance() .LoadResource(resources::ResourceLoadingService::RES_MODELS_ROOT + modelName + "/" +   fileName);
        renderableComponent->mMeshResourceIds.push_back(meshResourceId);
        renderableComponent->mAnimNameToMeshIndex[StringId(GetFileNameWithoutExtension(fileName))] = renderableComponent->mAnimNameToMeshIndex.size();
        
        const auto& meshResource = resources::ResourceLoadingService::GetInstance().GetResource<genesis::resources::MeshResource>(meshResourceId);
        renderableComponent->mBoneTransformMatrices.resize(meshResource.GetBoneOffsetMatrices().size());
        renderableComponent->mShaderUniforms.mShaderMatrixArrayUniforms[BONES_UNIFORM_NAME].resize(meshResource.GetBoneOffsetMatrices().size());
    }
    
    renderableComponent->mTextureResourceId = resources::ResourceLoadingService::GetInstance().LoadResource
    (
        resources::ResourceLoadingService::RES_TEXTURES_ROOT + modelName + ".png"
    );
    
    world.AddComponent<RenderableComponent>(modelEntity, std::move(renderableComponent));
    world.AddComponent<TransformComponent>(modelEntity, std::move(transformComponent));

    if (entityName != StringId())
    {
        world.AddComponent<NameComponent>(modelEntity, std::make_unique<NameComponent>(entityName));
    }
    
    animation::ChangeAnimation(modelEntity, IDLE_ANIMATION_NAME);
    
    return modelEntity;
}

///------------------------------------------------------------------------------------------------

ecs::EntityId LoadAndCreateGuiSprite
(
    const std::string& modelName,
    const std::string& textureName,
    const StringId shaderName,    
    const glm::vec3& initialPosition /* glm::vec3(0.0f, 0.0f, 0.0f) */,
    const glm::vec3& initialRotation /* glm::vec3(0.0f, 0.0f, 0.0f) */,
    const glm::vec3& initialScale /* glm::vec3(1.0f, 1.0f, 1.0f) */,
    const bool is3d /* false */,
    const StringId entityName /* StringId() */
)
{
    auto& world = ecs::World::GetInstance();
    const auto modelEntity = world.CreateEntity();

    auto transformComponent = std::make_unique<TransformComponent>();
    transformComponent->mPosition = initialPosition;
    transformComponent->mRotation = initialRotation;
    transformComponent->mScale = initialScale;

    auto renderableComponent = std::make_unique<RenderableComponent>();    
    renderableComponent->mShaderNameId = shaderName;
    renderableComponent->mRenderableType = is3d ? genesis::rendering::RenderableType::GUI_3D_MODEL: genesis::rendering::RenderableType::GUI_SPRITE;
    renderableComponent->mMeshResourceIds.push_back(
        resources::ResourceLoadingService::GetInstance().
        LoadResource(resources::ResourceLoadingService::RES_MODELS_ROOT + modelName + ".obj"));
    renderableComponent->mShaderUniforms.mShaderFloatVec4Uniforms[GUI_SHADER_CUSTOM_COLOR_UNIFORM_NAME] = genesis::colors::BLACK;
    renderableComponent->mTextureResourceId = resources::ResourceLoadingService::GetInstance().LoadResource
    (
        resources::ResourceLoadingService::RES_TEXTURES_ROOT + textureName + ".png"
    );

    world.AddComponent<RenderableComponent>(modelEntity, std::move(renderableComponent));
    world.AddComponent<TransformComponent>(modelEntity, std::move(transformComponent));

    if (entityName != StringId())
    {
        world.AddComponent<NameComponent>(modelEntity, std::make_unique<NameComponent>(entityName));
    }

    return modelEntity;
}

///-----------------------------------------------------------------------------------------------

resources::ResourceId LoadAndCreateMeshFromAtlasTexCoords
(
    const int meshAtlasCol,
    const int meshAtlasRow, 
    const int atlasColCount,
    const int atlasRowCount,
    const bool horizontalFlip /* false */
)
{
    auto correctedMeshCol = meshAtlasCol;
    auto correctedMeshRow = meshAtlasRow;

    if (correctedMeshCol >= atlasColCount)
    {
        correctedMeshCol %= atlasColCount;
        correctedMeshRow++;
    }

    const auto texCoords = CalculateTextureCoordsFromColumnAndRow(correctedMeshCol, correctedMeshRow, atlasColCount, atlasRowCount, horizontalFlip);
    const auto meshPath  = CreateTexCoordInjectedModelPath(texCoords);

    const auto loadedMeshResourceId = resources::ResourceLoadingService::GetInstance().LoadResource(meshPath);
    return loadedMeshResourceId;   
}

///-----------------------------------------------------------------------------------------------

std::vector<glm::vec2> CalculateTextureCoordsFromColumnAndRow
(
    const int col,
    const int row,
    const int atlasNumberOfCols,
    const int atlasNumberOfRows,
    const bool horFlipped
)
{
    const auto cols = static_cast<float>(atlasNumberOfCols);
    const auto rows = static_cast<float>(atlasNumberOfRows);
    const auto cellWidth = 1.0f / cols;
    const auto cellHeight = 1.0f / rows;

    if (horFlipped)
    {
        return
        {
            glm::vec2((col + 1) * cellWidth, 1.0f - row * cellHeight),
            glm::vec2(col * cellWidth,       1.0f - row * cellHeight),
            glm::vec2(col * cellWidth,       1.0f - (row + 1) * cellHeight),
            glm::vec2((col + 1) * cellWidth, 1.0f - (row + 1) * cellHeight)
        };
    }
    else
    {
        return
        {
            glm::vec2(col * cellWidth,       1.0f - row * cellHeight),
            glm::vec2((col + 1) * cellWidth, 1.0f - row * cellHeight),
            glm::vec2((col + 1) * cellWidth, 1.0f - (row + 1) * cellHeight),
            glm::vec2(col * cellWidth,       1.0f - (row + 1) * cellHeight)
        };
    }
}

///-----------------------------------------------------------------------------------------------

std::string CreateTexCoordInjectedModelPath
(    
    const std::vector<glm::vec2>& texCoords
)
{
    std::string path = resources::ResourceLoadingService::RES_MODELS_ROOT + ATLAS_MODEL_NAME.GetString() + "[";

    path += std::to_string(texCoords[0].x) + "," + std::to_string(texCoords[0].y);

    for (auto i = 1U; i < texCoords.size(); ++i)
    {
        path += "-" + std::to_string(texCoords[i].x) + "," + std::to_string(texCoords[i].y);
    }

    path += "].obj";

    return path;
}

///-----------------------------------------------------------------------------------------------

}

}

