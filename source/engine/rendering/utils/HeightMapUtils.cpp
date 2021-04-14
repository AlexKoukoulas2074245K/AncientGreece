///------------------------------------------------------------------------------------------------
///  HeightMapUtils.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 08/04/2021.
///------------------------------------------------------------------------------------------------

#include "HeightMapUtils.h"
#include "DiamondSquareHeightMapGenerator.h"
#include "../components/HeightMapComponent.h"
#include "../components/RenderableComponent.h"
#include "../opengl/Context.h"
#include "../../common/components/NameComponent.h"
#include "../../common/components/TransformComponent.h"
#include "../../common/utils/FileUtils.h"
#include "../../common/utils/MathUtils.h"
#include "../../resources/ResourceLoadingService.h"
#include "../../resources/TextureResource.h"
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
    static const std::string HEIGHTMAPS_DIRECTORY = "heightMaps/";
    static const std::string HEIGHTMAP_TEXTURES_DIRECTORY = "heightMap_textures/";
    static const std::string HEIGHTMAP_IMAGE_FILE_NAME = "heightMap.png";
    static const std::string HEIGHTMAP_TEXTURE_UNIFORM_NAME = "heightMap_texture_";

    static const StringId HEIGHTMAP_SHADER_NAME = StringId("heightMap");

    static const float HEIGHTMAP_Z_OFFSET = 0.001f;
}


///------------------------------------------------------------------------------------------------

ecs::EntityId LoadAndCreateHeightMapByName
(
    const std::string& heightMapName,
    const float heightMapHeightScale,
    const float heightMapWidthScale,
    const StringId entityName /* StringId() */,
    const HeightMapGenerationType generationType /* HeightMapGenerationType::FROM_TEXTURE */
)
{
    auto& resourceLoadingService = resources::ResourceLoadingService::GetInstance();
    
    const auto& heightMapsDirectory = resources::ResourceLoadingService::RES_TEXTURES_ROOT + HEIGHTMAPS_DIRECTORY;
    
    // Load heightMap image
    auto heightMapResourceId = resourceLoadingService.LoadResource(heightMapsDirectory + heightMapName + "/" + HEIGHTMAP_IMAGE_FILE_NAME);
    auto& heightMapTextureResource = resourceLoadingService.GetResource<resources::TextureResource>(heightMapResourceId);
    
    // Generate height map on the fly if specified
    if (generationType != HeightMapGenerationType::FROM_TEXTURE)
    {
        const auto roughness = generationType == HeightMapGenerationType::RANDOM_LOW_ROUGHNESS ? 0.9f : 2.0f;
        heightMapTextureResource.ChangeTexture(DiamondSquareHeightMapGenerator::GenerateRandomHeightMap(roughness));
    }
    
    // Load heightMap textures
    std::vector<resources::ResourceId> heightMapTextures;
    const auto heightMapTextureFilenames = GetAllFilenamesInDirectory(heightMapsDirectory + heightMapName + "/" + HEIGHTMAP_TEXTURES_DIRECTORY);
    
    for (const auto& filename: heightMapTextureFilenames)
    {
        heightMapTextures.push_back(resourceLoadingService.LoadResource(heightMapsDirectory + heightMapName + "/" + HEIGHTMAP_TEXTURES_DIRECTORY + filename));
    }
    
    // Read heightMap values
    const auto& heightMapDimensions = heightMapTextureResource.GetDimensions();
    const auto heightMapRows = static_cast<int>(heightMapDimensions.y);
    const auto heightMapCols = static_cast<int>(heightMapDimensions.x);
    
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> triangleNormals[2];
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;
    std::vector<std::vector<float>> heightMapTileHeights;
    
    const auto textureU = heightMapCols * 0.1f;
    const auto textureV = heightMapRows * 0.1f;
    
    for (auto row = 0; row < heightMapRows; ++row)
    {
        heightMapTileHeights.push_back({});
        for (auto col = 0; col < heightMapCols; ++col)
        {
            const auto colFactor = static_cast<float>(col)/(heightMapCols - 1);
            const auto rowFactor = static_cast<float>(row)/(heightMapRows - 1);
            const auto vertexHeight = heightMapTextureResource.GetRgbAtPixel(col, row).mRed/255.0f;
            
            vertices.push_back(glm::vec3(-0.5f + colFactor, vertexHeight, -0.5f + rowFactor));
            uvs.push_back(glm::vec2(colFactor * textureU, rowFactor * textureV));
            heightMapTileHeights[row].push_back(vertexHeight * heightMapHeightScale);
        }
    }
    
    triangleNormals[0].resize((heightMapRows - 1) * (heightMapCols - 1));
    triangleNormals[1].resize((heightMapRows - 1) * (heightMapCols - 1));
    
    for (auto row = 0; row < heightMapRows - 1; ++row)
    {
        for (auto col = 0; col < heightMapCols - 1; ++col)
        {
            glm::vec3 triangle0[] =
            {
                vertices[row * heightMapCols + col],
                vertices[(row + 1) * heightMapCols + col],
                vertices[(row + 1) * heightMapCols + (col +1)]
            };
            triangle0[0].y *= heightMapHeightScale;
            triangle0[1].y *= heightMapHeightScale;
            triangle0[2].y *= heightMapHeightScale;
            
            glm::vec3 triangle1[] =
            {
                vertices[(row + 1) * heightMapCols + (col +1)],
                vertices[row * heightMapCols + (col + 1)],
                vertices[row * heightMapCols + col]
            };
            
            triangle1[0].y *= heightMapHeightScale;
            triangle1[1].y *= heightMapHeightScale;
            triangle1[2].y *= heightMapHeightScale;
            
            glm::vec3 triangle0Norm = glm::cross(triangle0[1]-triangle0[0], triangle0[2]-triangle0[0]);
            glm::vec3 triangle1Norm = glm::cross(triangle1[1]-triangle1[0], triangle1[2]-triangle1[0]);
            
            triangleNormals[0][row * heightMapCols + col] = glm::normalize(triangle0Norm);
            triangleNormals[1][row * heightMapCols + col] = glm::normalize(triangle1Norm);
        }
    }
    
    for (auto row = 0; row < heightMapRows; ++row)
    {
        for (auto col = 0; col < heightMapCols; ++col)
        {
            glm::vec3 finalNormal = glm::vec3(0.0f);

            // Look for upper-left triangles
            if(col != 0 && row != 0)
            {
                finalNormal += triangleNormals[0][(row - 1) * heightMapCols + (col - 1)];
                finalNormal += triangleNormals[1][(row - 1) * heightMapCols + (col - 1)];
            }

            // Look for upper-right triangles
            if(row != 0 && col != heightMapCols - 1)
            {
                finalNormal += triangleNormals[0][(row - 1) * heightMapCols + col];
            }

            // Look for bottom-right triangles
            if(row != heightMapRows - 1 && col != heightMapCols - 1)
            {
                finalNormal += triangleNormals[0][row * heightMapCols + col];
                finalNormal += triangleNormals[1][row * heightMapCols + col];
            }

            // Look for bottom-left triangles
            if(row != heightMapRows - 1 && col != 0)
            {
                finalNormal += triangleNormals[1][row * heightMapCols + (col - 1)];
            }

            normals.push_back(glm::normalize(finalNormal));
        }
    }
    
    const auto primitiveRestart = heightMapRows * heightMapCols;
    for (auto row = 0; row < heightMapRows - 1; ++row)
    {
        for (auto col = 0; col < heightMapCols; ++col)
        {
            indices.push_back((row + 1) * heightMapCols + col);
            indices.push_back(row * heightMapCols + col);
        }
        indices.push_back(primitiveRestart);
    }
    
    
    GLuint vertexArrayObject;
    GLuint vertexBufferObject;
    GLuint uvCoordsBufferObject;
    GLuint normalsBufferObject;
    GLuint indexBufferObject;
    
    // Create Buffers
    GL_CHECK(glGenVertexArrays(1, &vertexArrayObject));
    GL_CHECK(glGenBuffers(1, &vertexBufferObject));
    GL_CHECK(glGenBuffers(1, &uvCoordsBufferObject));
    GL_CHECK(glGenBuffers(1, &normalsBufferObject));
    GL_CHECK(glGenBuffers(1, &indexBufferObject));
    
    // Prepare VAO to record buffer state
    GL_CHECK(glBindVertexArray(vertexArrayObject));
    
    // Bind and Buffer VBO
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW));
    
    // 1st attribute buffer : vertices
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    
    // Bind and buffer TBO
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, uvCoordsBufferObject));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW));
    
    // 2nd attribute buffer: tex coords
    GL_CHECK(glEnableVertexAttribArray(1));
    GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));
    
    // Bind and buffer NBO
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, normalsBufferObject));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW));
    
    // 3rd attribute buffer: normals
    GL_CHECK(glEnableVertexAttribArray(2));
    GL_CHECK(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    
    // Bind and Buffer IBO
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW));
    
    GL_CHECK(glBindVertexArray(0));
    
    auto& world = ecs::World::GetInstance();
    const auto heightMapEntity = world.CreateEntity();

    auto transformComponent = std::make_unique<TransformComponent>();
    transformComponent->mPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    transformComponent->mRotation = glm::vec3(-genesis::math::PI/2.0f, 0.0f, genesis::math::PI);
    transformComponent->mScale = glm::vec3(heightMapWidthScale, heightMapHeightScale, heightMapWidthScale);

    auto renderableComponent = std::make_unique<RenderableComponent>();
    renderableComponent->mShaderNameId = HEIGHTMAP_SHADER_NAME;
    renderableComponent->mMaterial.mAmbient = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    renderableComponent->mMaterial.mDiffuse = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
    renderableComponent->mMaterial.mSpecular = glm::vec4(0.00f, 0.00f, 0.00f, 0.0f);
    renderableComponent->mMaterial.mShininess = 0.0f;
    renderableComponent->mIsAffectedByLight = true;
    
    for (auto i = 0U; i < heightMapTextures.size(); ++i)
    {
        renderableComponent->mShaderUniforms.mShaderIntUniforms[StringId(HEIGHTMAP_TEXTURE_UNIFORM_NAME + std::to_string(i))] = i;
    }
    
    auto heightMapComponent = std::make_unique<HeightMapComponent>();
    heightMapComponent->mVertexArrayObject = vertexArrayObject;
    heightMapComponent->mHeightMapTextureDimensions = glm::vec2(heightMapCols, heightMapRows);
    heightMapComponent->mHeightMapTextureResourceIds = heightMapTextures;
    heightMapComponent->mHeightMapTileHeights = heightMapTileHeights;
    heightMapComponent->mHeightMapScale = heightMapHeightScale;
    
    world.AddComponent<HeightMapComponent>(heightMapEntity, std::move(heightMapComponent));
    world.AddComponent<RenderableComponent>(heightMapEntity, std::move(renderableComponent));
    world.AddComponent<TransformComponent>(heightMapEntity, std::move(transformComponent));

    if (entityName != StringId())
    {
        world.AddComponent<NameComponent>(heightMapEntity, std::make_unique<NameComponent>(entityName));
    }
    
    return heightMapEntity;
}

///-----------------------------------------------------------------------------------------------

float GetTerrainHeightAtPosition(const genesis::ecs::EntityId heightMapEntityId, const glm::vec3& position)
{
    auto& world = genesis::ecs::World::GetInstance();
    const auto& heightMapComponent = world.GetComponent<rendering::HeightMapComponent>(heightMapEntityId);
    const auto& transformComponent = world.GetComponent<TransformComponent>(heightMapEntityId);
    
    const auto relativeXDisplacement = position.x/(transformComponent.mScale.x/2.0f);
    const auto relativeYDisplacement = position.y/(transformComponent.mScale.z/2.0f);
    const auto heightMapCols = heightMapComponent.mHeightMapTextureDimensions.x;
    const auto heightMapRows = heightMapComponent.mHeightMapTextureDimensions.y;
    
    const auto heightMapCol = heightMapCols/2.0f + relativeXDisplacement * heightMapCols/2.0f;
    const auto heightMapRow = heightMapRows/2 - relativeYDisplacement * heightMapRows/2;
    
    if (heightMapCol >= 0 && heightMapRow >= 0 && heightMapCol < heightMapCols && heightMapRow < heightMapRows)
    {
        return -(heightMapComponent.mHeightMapTileHeights[heightMapRow][heightMapCols - heightMapCol] + HEIGHTMAP_Z_OFFSET);
    }
    return 0.0f;
}

///-----------------------------------------------------------------------------------------------

}

}

