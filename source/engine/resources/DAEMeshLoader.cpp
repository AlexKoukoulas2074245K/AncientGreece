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
    
    const aiMesh* mesh = scene->mMeshes[0];
    const auto vertexCount = mesh->mNumVertices;
    
    std::vector<glm::vec3> vertices; vertices.reserve(vertexCount);
    std::vector<glm::vec2> uvs; uvs.reserve(vertexCount);
    std::vector<glm::vec3> normals; normals.reserve(vertexCount);
    std::vector<VertexBoneData> bones; bones.reserve(vertexCount);
    std::vector<unsigned short> indices;
    std::vector<BoneInfo> boneInfo;
    tsl::robin_map<std::string, unsigned int> boneNameToIdMap;
    AnimationInfo animationInfo;
    
    float minX = 100.0f, maxX = -100.0f, minY = 100.0f, maxY = -100.0f, minZ = 100.0f, maxZ = -100.0f;
    
    // Load Vertex Data
    for (unsigned int i = 0 ; i < vertexCount ; i++) {
        const aiVector3D* pos = &(mesh->mVertices[i]);
        const aiVector3D* normal = &(mesh->mNormals[i]);
        const aiVector3D* uv = &(mesh->mTextureCoords[0][i]);
        
        vertices.emplace_back(glm::vec3(pos->x, pos->y, pos->z));
        uvs.emplace_back(uv->x, uv->y);
        normals.emplace_back(normal->x, normal->y, normal->z);
        bones.emplace_back(VertexBoneData());
    }
    
    // Load Bone Data
    for (unsigned int i = 0 ; i < mesh->mNumBones ; i++) {
        unsigned int boneIndex = 0;
        auto boneName = std::string(mesh->mBones[i]->mName.data);

        if (boneNameToIdMap.find(boneName) == boneNameToIdMap.end())
        {
            boneIndex = boneInfo.size();
            BoneInfo bi;
            boneInfo.push_back(bi);
        }
        else
        {
            boneIndex = boneNameToIdMap[boneName];
        }

        boneNameToIdMap[boneName] = boneIndex;
        boneInfo[boneIndex].mBoneOffsetMatrix = math::AssimpMat4ToGlmMat4(mesh->mBones[i]->mOffsetMatrix);

        for (unsigned int j = 0 ; j < mesh->mBones[i]->mNumWeights ; j++)
        {
            uint vertexIndex = mesh->mBones[i]->mWeights[j].mVertexId;
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
    
    // Load Face Data
    for (unsigned int i = 0 ; i < mesh->mNumFaces ; i++) {
        const aiFace& Face = mesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        indices.push_back(Face.mIndices[0]);
        indices.push_back(Face.mIndices[1]);
        indices.push_back(Face.mIndices[2]);
    }
    
    // Load Animation Data
    auto* currentAnimation = scene->mAnimations[0];
    animationInfo.mDuration = static_cast<float>(currentAnimation->mDuration);
    animationInfo.mTicksPerSecond = static_cast<float>(currentAnimation->mTicksPerSecond);
    
    for (unsigned int i = 0; i < currentAnimation->mNumChannels; ++i)
    {
        auto* nodeAnim = currentAnimation->mChannels[i];
        auto nodeName = std::string(nodeAnim->mNodeName.C_Str());
        
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
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW));
    
    // 1st attribute buffer : vertices
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    
    // Bind and buffer TBO
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, uvCoordsBufferObject));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW));
    
    // 2nd attribute buffer: tex coords
    GL_CHECK(glEnableVertexAttribArray(1));
    GL_CHECK(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0));
    
    // Bind and buffer NBO
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, normalsBufferObject));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW));
    
    // 3rd attribute buffer: normals
    GL_CHECK(glEnableVertexAttribArray(2));
    GL_CHECK(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0));
    
    // Bind and buffer BBO
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, bonesBufferObject));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(VertexBoneData), &bones[0], GL_STATIC_DRAW));
    
    // 4th attribute buffer: bone ids
    GL_CHECK(glEnableVertexAttribArray(3));
    GL_CHECK(glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), (void*)0));
    
    // 4th attribute buffer: bone weights
    GL_CHECK(glEnableVertexAttribArray(4));
    GL_CHECK(glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (void*)16));
    
    // Bind and Buffer IBO
    GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferObject));
    GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW));
    
    GL_CHECK(glBindVertexArray(0));
    
    // Calculate dimensions
    glm::vec3 meshDimensions(math::Abs(minX - maxX), math::Abs(minY - maxY), math::Abs(minZ - maxZ));
    return std::unique_ptr<MeshResource>(new MeshResource(boneInfo, animationInfo, boneNameToIdMap, sceneTransform, scene->mRootNode, vertexArrayObject, indices.size(), meshDimensions));
}

///------------------------------------------------------------------------------------------------

}

}

