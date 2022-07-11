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
#include "../common/utils/Logging.h"
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

    static constexpr int MAX_NUM_BONES_AFFECTING_EACH_VERTEX = 4;
}

///------------------------------------------------------------------------------------------------

struct VertexBoneData
{
    VertexBoneData()
    {
    }
    
    unsigned int mBoneIds[MAX_NUM_BONES_AFFECTING_EACH_VERTEX] = {0};
    float mBoneWeights[MAX_NUM_BONES_AFFECTING_EACH_VERTEX] = {0.0f};
};

///------------------------------------------------------------------------------------------------

void DAEMeshLoader::VInitialize()
{
}

///------------------------------------------------------------------------------------------------

std::unique_ptr<IResource> DAEMeshLoader::VCreateAndLoadResource(const std::string& path) const
{
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
    
    auto globalInverseSceneTransform = scene->mRootNode->mTransformation;
    auto sceneTransform = math::AssimpMat4ToGlmMat4(globalInverseSceneTransform);
    
    std::vector<GLuint> indexCountPerMesh;
    std::vector<GLuint> baseIndexPerMesh;
    std::vector<GLuint> baseVertexPerMesh;
    
    auto totalVertexCount = 0U;
    auto totalIndexCount = 0U;
    
    for (auto m = 0U; m < scene->mNumMeshes; ++m)
    {
        baseIndexPerMesh.push_back(totalIndexCount);
        baseVertexPerMesh.push_back(totalVertexCount);
        indexCountPerMesh.push_back(scene->mMeshes[m]->mNumFaces * 3);
        totalIndexCount += scene->mMeshes[m]->mNumFaces * 3;
        totalVertexCount += scene->mMeshes[m]->mNumVertices;
    }
    
    std::vector<glm::vec3> vertices; vertices.reserve(totalVertexCount);
    std::vector<glm::vec2> uvs; uvs.reserve(totalVertexCount);
    std::vector<glm::vec3> normals; normals.reserve(totalVertexCount);
    std::vector<VertexBoneData> bones; bones.reserve(totalVertexCount);
    std::vector<unsigned short> indices; indices.reserve(totalIndexCount);
    std::vector<glm::mat4> boneOffsetMatrices;
    tsl::robin_map<StringId, unsigned int, StringIdHasher> boneNameToIdMap;
    AnimationInfo animationInfo;
    
    float minX = 100.0f, maxX = -100.0f, minY = 100.0f, maxY = -100.0f, minZ = 100.0f, maxZ = -100.0f;
    
    // Load Face Data
    for (auto m = 0U; m < scene->mNumMeshes; ++m)
    {
        const auto mesh = scene->mMeshes[m];
        for (unsigned int i = 0 ; i < mesh->mNumFaces; i++) {
            const aiFace& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < 3; ++j)
            {
                if (j >= face.mNumIndices)
                {
                    // Invalidate mesh
                    indexCountPerMesh[m] = 0;
                    indices.push_back(0);
                }
                else
                {
                    indices.push_back(static_cast<unsigned short>(face.mIndices[j]));
                }
            }
        }
    }
    
    for (auto m = 0U; m < scene->mNumMeshes; ++m)
    {
        const auto mesh = scene->mMeshes[m];
        
        // Load Vertex Data
        for (unsigned int i = 0 ; i < mesh->mNumVertices ; i++) {
            const aiVector3D* pos = &(mesh->mVertices[i]);
            const aiVector3D* normal = &(mesh->mNormals[i]);
            const aiVector3D* uv =  &(mesh->mTextureCoords[0][i]);
            
            vertices.emplace_back(glm::vec3(pos->x, pos->y, pos->z));
            uvs.emplace_back(uv->x, uv->y);
            normals.emplace_back(normal->x, normal->y, normal->z);
            bones.emplace_back(VertexBoneData());
            
            if (indexCountPerMesh[m] != 0)
            {
                if (pos->x < minX) minX = pos->x;
                if (pos->x > maxX) maxX = pos->x;
                if (pos->y < minY) minY = pos->y;
                if (pos->y > maxY) maxY = pos->y;
                if (pos->z < minZ) minZ = pos->z;
                if (pos->z > maxZ) maxZ = pos->z;
            }
        }
    }
    
    // Load Bone Data
    for (auto m = 0U; m < scene->mNumMeshes; ++m)
    {
        const auto mesh = scene->mMeshes[m];
        for (unsigned int i = 0 ; i < mesh->mNumBones ; i++) {
            unsigned int boneIndex = 0;
            auto boneName = StringId(std::string(mesh->mBones[i]->mName.data));

            if (boneNameToIdMap.find(boneName) == boneNameToIdMap.end())
            {
                boneIndex = boneOffsetMatrices.size();
                boneOffsetMatrices.push_back(glm::mat4(1.0f));
            }
            else
            {
                boneIndex = boneNameToIdMap[boneName];
            }

            boneNameToIdMap[boneName] = boneIndex;
            boneOffsetMatrices[boneIndex] = math::AssimpMat4ToGlmMat4(mesh->mBones[i]->mOffsetMatrix);

            for (unsigned int j = 0 ; j < mesh->mBones[i]->mNumWeights ; j++)
            {
                uint vertexIndex = mesh->mBones[i]->mWeights[j].mVertexId + baseVertexPerMesh[m];
                float weight = mesh->mBones[i]->mWeights[j].mWeight;
                
                bool foundBoneDataEntry = false;
                for (unsigned int k = 0; k < MAX_NUM_BONES_AFFECTING_EACH_VERTEX; ++k)
                {
                    if (math::Abs(bones[vertexIndex].mBoneWeights[k]) < 0.00001f)
                    {
                        bones[vertexIndex].mBoneIds[k] = boneIndex;
                        bones[vertexIndex].mBoneWeights[k] = weight;
                        foundBoneDataEntry = true;
                        break;
                    }
                }
                
                assert(foundBoneDataEntry && std::string("More than: " +  std::to_string(MAX_NUM_BONES_AFFECTING_EACH_VERTEX) + " bone influences found").c_str());
            }
        }
    }
    
    // Load Animation Data
    auto* currentAnimation = scene->mAnimations[0];
    animationInfo.mTicksPerSecond = static_cast<float>(currentAnimation->mTicksPerSecond);
    
    for (unsigned int i = 0; i < currentAnimation->mNumChannels; ++i)
    {
        auto* nodeAnim = currentAnimation->mChannels[i];
        auto nodeName = StringId(std::string(nodeAnim->mNodeName.C_Str()));
        
        std::vector<PositionKey> positionKeys;
        std::vector<RotationKey> rotationKeys;
        std::vector<ScalingKey> scalingKeys;
        
        for (unsigned int j = 0; j < nodeAnim->mNumPositionKeys; ++j)
        {
            PositionKey positionKey;
            positionKey.mTime = static_cast<float>(nodeAnim->mPositionKeys[j].mTime);
            positionKey.mPosition = math::AssimpVec3ToGlmVec3(nodeAnim->mPositionKeys[j].mValue);
            positionKeys.push_back(positionKey);
        }
        
        for (unsigned int j = 0; j < nodeAnim->mNumRotationKeys; ++j)
        {
            RotationKey rotationKey;
            rotationKey.mTime = static_cast<float>(nodeAnim->mRotationKeys[j].mTime);
            rotationKey.mRotation = math::AssimpQuatToGlmQuat(nodeAnim->mRotationKeys[j].mValue);
            rotationKeys.push_back(rotationKey);
        }
        
        for (unsigned int j = 0; j < nodeAnim->mNumScalingKeys; ++j)
        {
            ScalingKey scalingKey;
            scalingKey.mTime = static_cast<float>(nodeAnim->mScalingKeys[j].mTime);
            scalingKey.mScale = math::AssimpVec3ToGlmVec3(nodeAnim->mScalingKeys[j].mValue);
            scalingKeys.push_back(scalingKey);
        }
        
        animationInfo.mDuration = positionKeys.back().mTime;
        
        animationInfo.mBoneNameToAnimInfo[nodeName].mPositionKeys = std::move(positionKeys);
        animationInfo.mBoneNameToAnimInfo[nodeName].mRotationKeys = std::move(rotationKeys);
        animationInfo.mBoneNameToAnimInfo[nodeName].mScalingKeys = std::move(scalingKeys);
    }
    
    GLuint vertexArrayObject;
    GLuint vertexBufferObject;
    GLuint uvCoordsBufferObject;
    GLuint normalsBufferObject;
    GLuint bonesBufferObject;
    GLuint indexBufferObject;
    
    // Create Buffers
    GL_CHECK(glGenVertexArrays(1, &vertexArrayObject));
    GL_CHECK(glGenBuffers(1, &vertexBufferObject));
    GL_CHECK(glGenBuffers(1, &uvCoordsBufferObject));
    GL_CHECK(glGenBuffers(1, &normalsBufferObject));
    GL_CHECK(glGenBuffers(1, &bonesBufferObject));
    GL_CHECK(glGenBuffers(1, &indexBufferObject));
    
    // Prepare VAO to record buffer state
    GL_CHECK(glBindVertexArray(vertexArrayObject));
    
    // Bind and Buffer VBO
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, totalVertexCount * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW));
    
    // 1st attribute buffer : vertices
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    
    // Bind and buffer TBO
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, uvCoordsBufferObject));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, totalVertexCount * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW));
    
    // 2nd attribute buffer: tex coords
    GL_CHECK(glEnableVertexAttribArray(1));
    GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));
    
    // Bind and buffer NBO
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, normalsBufferObject));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, totalVertexCount * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW));
    
    // 3rd attribute buffer: normals
    GL_CHECK(glEnableVertexAttribArray(2));
    GL_CHECK(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    
    // Bind and buffer BBO
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, bonesBufferObject));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, totalVertexCount * sizeof(VertexBoneData), &bones[0], GL_STATIC_DRAW));
    
    // 4th attribute buffer: bone ids
    GL_CHECK(glEnableVertexAttribArray(3));
    GL_CHECK(glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), (void*)0));
    
    // 4th attribute buffer: bone weights
    GL_CHECK(glEnableVertexAttribArray(4));
    GL_CHECK(glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (void*)16));
    
    // Bind and Buffer IBO
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalIndexCount * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW));
    
    GL_CHECK(glBindVertexArray(0));
    
    // Calculate dimensions
    glm::vec3 meshDimensions(math::Abs(minX - maxX), math::Abs(minY - maxY), math::Abs(minZ - maxZ));
    
    std::unique_ptr<MeshResource> meshResource(new MeshResource(animationInfo, boneOffsetMatrices, boneNameToIdMap, sceneTransform, scene->mRootNode, vertexArrayObject, indexCountPerMesh, baseIndexPerMesh, baseVertexPerMesh, meshDimensions));
    
    importer.FreeScene();
    
    return std::move(meshResource);
}

///------------------------------------------------------------------------------------------------

}

}

