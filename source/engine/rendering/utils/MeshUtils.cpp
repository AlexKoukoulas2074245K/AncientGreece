///------------------------------------------------------------------------------------------------
///  MeshUtils.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 09/01/2020.
///------------------------------------------------------------------------------------------------

#include "MeshUtils.h"
#include "../components/RenderableComponent.h"
#include "../../common/components/NameComponent.h"
#include "../../common/components/TransformComponent.h"
#include "../../common/utils/MathUtils.h"

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
    const StringId DEFAULT_MODEL_SHADER = StringId("default_3d");    
    const StringId ATLAS_MODEL_NAME     = StringId("gui_atlas_quad");
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

ecs::EntityId LoadAndCreateModelByName
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

    renderableComponent->mMeshResourceId =     
        resources::ResourceLoadingService::GetInstance().
        LoadResource(resources::ResourceLoadingService::RES_MODELS_ROOT + modelName + ".obj");
        
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

ecs::EntityId LoadAndCreateGuiSprite
(
    const std::string& modelName,
    const std::string& textureName,
    const StringId shaderName,    
    const glm::vec3& initialPosition /* glm::vec3(0.0f, 0.0f, 0.0f) */,
    const StringId entityName /* StringId() */
)
{
    auto& world = ecs::World::GetInstance();
    const auto modelEntity = world.CreateEntity();

    auto transformComponent = std::make_unique<TransformComponent>();
    transformComponent->mPosition = initialPosition;

    auto renderableComponent = std::make_unique<RenderableComponent>();    
    renderableComponent->mShaderNameId = shaderName;
    renderableComponent->mIsGuiComponent = true;
    renderableComponent->mMeshResourceId =     
        resources::ResourceLoadingService::GetInstance().
        LoadResource(resources::ResourceLoadingService::RES_MODELS_ROOT + modelName + ".obj");

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

