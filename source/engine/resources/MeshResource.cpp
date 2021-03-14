///------------------------------------------------------------------------------------------------
///  MeshResource.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#include "MeshResource.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace resources
{

///------------------------------------------------------------------------------------------------

GLuint MeshResource::GetVertexArrayObject() const
{
    return mVertexArrayObject;
}

///------------------------------------------------------------------------------------------------

GLuint MeshResource::GetElementCount() const
{
    return mElementCount;
}

///------------------------------------------------------------------------------------------------

const glm::vec3& MeshResource::GetDimensions() const
{
    return mDimensions;
}

///------------------------------------------------------------------------------------------------

bool MeshResource::HasSkeleton() const
{
    return mRootSkeletonNode != nullptr;
}

///------------------------------------------------------------------------------------------------

const SkeletonNode* MeshResource::GetRootSkeletonNode() const
{
    return mRootSkeletonNode;
}

///------------------------------------------------------------------------------------------------

const AnimationInfo& MeshResource::GetAnimationInfo() const
{
    return mAnimationInfo;
}

///------------------------------------------------------------------------------------------------

const tsl::robin_map<StringId, unsigned int, StringIdHasher>& MeshResource::GetBoneNameToIdMap() const
{
    return mBoneNameToIdMap;
}

///------------------------------------------------------------------------------------------------

const glm::mat4& MeshResource::GetSceneTransform() const
{
    return mSceneTransform;
}

///------------------------------------------------------------------------------------------------

const std::vector<glm::mat4>& MeshResource::GetBoneOffsetMatrices() const
{
    return mBoneOffsetMatrices;
}

///------------------------------------------------------------------------------------------------

MeshResource::MeshResource(const AnimationInfo& animationInfo, const std::vector<glm::mat4>& boneOffsetMatrices, const tsl::robin_map<StringId, unsigned int, StringIdHasher>& boneMapping, const glm::mat4& sceneTransform, const aiNode* rootAssimpNode, const GLuint vertexArrayObject, const GLuint elementCount, const glm::vec3& meshDimensions)
    : mAnimationInfo(animationInfo)
    , mBoneOffsetMatrices(boneOffsetMatrices)
    , mBoneNameToIdMap(boneMapping)
    , mSceneTransform(sceneTransform)
    , mVertexArrayObject(vertexArrayObject)
    , mElementCount(elementCount)
    , mDimensions(meshDimensions)
{
    mRootSkeletonNode = new SkeletonNode;
    CreateSkeleton(rootAssimpNode, mRootSkeletonNode);
}

MeshResource::MeshResource(const GLuint vertexArrayObject, const GLuint elementCount, const glm::vec3& meshDimensions)
    : mAnimationInfo()
    , mSceneTransform()
    , mVertexArrayObject(vertexArrayObject)
    , mElementCount(elementCount)
    , mDimensions(meshDimensions)
    , mRootSkeletonNode(nullptr)
{
}

///------------------------------------------------------------------------------------------------
MeshResource::~MeshResource()
{
    DestroySkeleton(mRootSkeletonNode);
}

///------------------------------------------------------------------------------------------------

void MeshResource::CreateSkeleton(const aiNode* node, SkeletonNode* skeletonNode)
{
    if (node == nullptr) return;
    
    skeletonNode->mNodeName = StringId(std::string(node->mName.C_Str()));
    skeletonNode->mTransform = math::AssimpMat4ToGlmMat4(node->mTransformation);
    skeletonNode->mNumChildren = node->mNumChildren;
    
    if (skeletonNode->mNumChildren > 0)
    {
        skeletonNode->mChildren = new SkeletonNode*[node->mNumChildren];
    }
    
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        if (node->mChildren[i] != nullptr)
        {
            skeletonNode->mChildren[i] = new SkeletonNode;
            CreateSkeleton(node->mChildren[i], skeletonNode->mChildren[i]);
        }
        else
        {
            skeletonNode->mChildren[i] = nullptr;
        }
    }
}

///------------------------------------------------------------------------------------------------
void MeshResource::DestroySkeleton(SkeletonNode* skeletonNode)
{
    if (skeletonNode == nullptr) return;
    
    for (int i = 0; i < skeletonNode->mNumChildren; ++i)
    {
        DestroySkeleton(skeletonNode->mChildren[i]);
    }
    if (skeletonNode->mNumChildren > 0)
    {
        delete[] skeletonNode->mChildren;
    }
    delete skeletonNode;
}

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------
