///------------------------------------------------------------------------------------------------
///  DAEMeshLoader.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 07/03/2021.
///------------------------------------------------------------------------------------------------

// Disable CRT_SECURE warnings for fopen, fscanf etc..
#ifdef _WIN32
#pragma warning(disable: 4996)
#endif

#include "DAEMeshLoader.h"
#include "MeshResource.h"
#include "../common/utils/StringUtils.h"
#include "../common/utils/MathUtils.h"
#include "../rendering/opengl/Context.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <cassert>
#include <cstdio>
#include <vector>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace resources
{

///------------------------------------------------------------------------------------------------

namespace
{
    Assimp::Importer importer;
}

void DAEMeshLoader::VInitialize()
{
}

///------------------------------------------------------------------------------------------------

std::unique_ptr<IResource> DAEMeshLoader::VCreateAndLoadResource(const std::string& path) const
{
    std::vector<glm::vec3> final_vertices;
    std::vector<glm::vec2> final_uvs;
    std::vector<glm::vec3> final_normals;
    
    std::vector<unsigned short> final_indices;
    
    float minX = 100.0f, maxX = -100.0f, minY = 100.0f, maxY = -100.0f, minZ = 100.0f, maxZ = -100.0f;

    const aiScene* scene = importer.ReadFile(path.c_str(),
      aiProcess_CalcTangentSpace       |
      aiProcess_Triangulate            |
      aiProcess_JoinIdenticalVertices  |
      aiProcess_SortByPType);
   
    if (!scene || !scene->mMeshes[0])
    {
        auto errorMessage = "Error loading DAE scene: " + path + "\n" + importer.GetErrorString();
        assert(false && errorMessage.c_str());
    }
    
    const aiMesh* mesh = scene->mMeshes[0];
  
    for (unsigned int i = 0 ; i < mesh->mNumVertices ; i++) {
        const aiVector3D* pos = &(mesh->mVertices[i]);
        const aiVector3D* normal = &(mesh->mNormals[i]);
        const aiVector3D* uv = &(mesh->mTextureCoords[0][i]);

        final_vertices.emplace_back(glm::vec3(pos->x, pos->y, pos->z));
        final_uvs.emplace_back(uv->x, uv->y);
        final_normals.emplace_back(normal->x, normal->y, normal->z);
    }
    
    for (unsigned int i = 0 ; i < mesh->mNumFaces ; i++) {
        const aiFace& Face = mesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        final_indices.push_back(Face.mIndices[0]);
        final_indices.push_back(Face.mIndices[1]);
        final_indices.push_back(Face.mIndices[2]);
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
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, final_vertices.size() * sizeof(glm::vec3), &final_vertices[0], GL_STATIC_DRAW));
    
    // 1st attribute buffer : vertices
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    
    // Bind and buffer TBO
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, uvCoordsBufferObject));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, final_uvs.size() * sizeof(glm::vec2), &final_uvs[0], GL_STATIC_DRAW));
    
    // 2nd attribute buffer: tex coords
    GL_CHECK(glEnableVertexAttribArray(1));
    GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));
    
    // Bind and buffer NBO
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, normalsBufferObject));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, final_normals.size() * sizeof(glm::vec3), &final_normals[0], GL_STATIC_DRAW));
    
    // 3rd attribute buffer: normals
    GL_CHECK(glEnableVertexAttribArray(2));
    GL_CHECK(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    
    // Bind and Buffer IBO
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, final_indices.size() * sizeof(unsigned short), &final_indices[0], GL_STATIC_DRAW));
    
    GL_CHECK(glBindVertexArray(0));
    
    // Calculate dimensinos
    glm::vec3 meshDimensions(math::Abs(minX - maxX), math::Abs(minY - maxY), math::Abs(minZ - maxZ));
    return std::unique_ptr<MeshResource>(new MeshResource(vertexArrayObject, final_indices.size(), meshDimensions));
}

///------------------------------------------------------------------------------------------------

std::string DAEMeshLoader::ExtractAndRemoveInjectedTexCoordsIfAny(std::string& path) const
{
    std::string injectedTexCoordString;
    std::string cleanPath;
    
    auto recordingInjectedTexCoords = false;
    
    for (auto i = 0U; i < path.length(); ++i)
    {
        const auto c = path[i];
        if (c == '[' || c == ']')
        {
            recordingInjectedTexCoords = !recordingInjectedTexCoords;
        }
        else if (recordingInjectedTexCoords)
        {
            injectedTexCoordString += c;
        }
        else
        {
            cleanPath += c;
        }
    }
    
    path = cleanPath;
    return injectedTexCoordString;
}

}

///------------------------------------------------------------------------------------------------

}

