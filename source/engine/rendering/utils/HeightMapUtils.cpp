///------------------------------------------------------------------------------------------------
///  HeightMapUtils.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 08/04/2021.
///------------------------------------------------------------------------------------------------

#include "HeightMapUtils.h"
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

    static const float HEIGHTMAP_HEIGHT_SCALE = 0.05f;
}

///------------------------------------------------------------------------------------------------

ecs::EntityId LoadAndCreateHeightMapByName
(
    const std::string& heightMapName,
    const StringId entityName /* StringId() */
)
{
    auto& resourceLoadingService = resources::ResourceLoadingService::GetInstance();
    
    const auto& heightMapsDirectory = resources::ResourceLoadingService::RES_TEXTURES_ROOT + HEIGHTMAPS_DIRECTORY;
    
    // Load heightMap image
    auto heightMapResourceId = resourceLoadingService.LoadResource(heightMapsDirectory + heightMapName + "/" + HEIGHTMAP_IMAGE_FILE_NAME);
    const auto& heightMapTextureResource = resourceLoadingService.GetResource<resources::TextureResource>(heightMapResourceId);
    
    // Load heightMap textures
    std::vector<resources::ResourceId> heightMapTextures;
    const auto heightMapTextureFilenames = GetAllFilenamesInDirectory(heightMapsDirectory + heightMapName + "/" + HEIGHTMAP_TEXTURES_DIRECTORY);
    
    for (const auto& filename: heightMapTextureFilenames)
    {
        heightMapTextures.push_back(resourceLoadingService.LoadResource(heightMapsDirectory + heightMapName + "/" + HEIGHTMAP_TEXTURES_DIRECTORY + filename));
    }
    
    // Read heightMap values
    const auto& heightMapDimensions = heightMapTextureResource.GetDimensions();
    const auto heightMapRows = heightMapDimensions.y;
    const auto heightMapCols = heightMapDimensions.x;
    
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
            heightMapTileHeights[row].push_back(vertexHeight * HEIGHTMAP_HEIGHT_SCALE);
        }
    }
    
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
            glm::vec3 triangle1[] =
            {
                vertices[(row + 1) * heightMapCols + (col +1)],
                vertices[row * heightMapCols + (col + 1)],
                vertices[row * heightMapCols + col]
            };
            
            glm::vec3 triangle0Norm = glm::cross(triangle0[0]-triangle0[1], triangle0[1]-triangle0[2]);
            glm::vec3 triangle1Norm = glm::cross(triangle1[0]-triangle1[1], triangle1[1]-triangle1[2]);
            
            triangleNormals[0].push_back(glm::normalize(triangle0Norm));
            triangleNormals[1].push_back(glm::normalize(triangle1Norm));
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
    transformComponent->mScale = glm::vec3(1.0f, HEIGHTMAP_HEIGHT_SCALE, 1.0f);

    auto renderableComponent = std::make_unique<RenderableComponent>();
    renderableComponent->mShaderNameId = HEIGHTMAP_SHADER_NAME;
    
    for (auto i = 0U; i < heightMapTextures.size(); ++i)
    {
        renderableComponent->mShaderUniforms.mShaderIntUniforms[StringId(HEIGHTMAP_TEXTURE_UNIFORM_NAME + std::to_string(i))] = i;
    }
    
    auto heightMapComponent = std::make_unique<HeightMapComponent>();
    heightMapComponent->mVertexArrayObject = vertexArrayObject;
    heightMapComponent->mHeightMapTextureDimensions = glm::vec2(heightMapCols, heightMapRows);
    heightMapComponent->mHeightMapTextureResourceIds = heightMapTextures;
    heightMapComponent->mHeightMapTileHeights = heightMapTileHeights;
    
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

}

}

