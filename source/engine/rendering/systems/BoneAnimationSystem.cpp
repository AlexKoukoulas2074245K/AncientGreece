///------------------------------------------------------------------------------------------------
///  BoneAnimationSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 08/03/2021.
///-----------------------------------------------------------------------------------------------

#include "BoneAnimationSystem.h"
#include "../components/RenderableComponent.h"
#include "../../common/components/TransformComponent.h"
#include "../../common/utils/Logging.h"
#include "../../common/utils/MathUtils.h"
#include "../../resources/ResourceLoadingService.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------

BoneAnimationSystem::BoneAnimationSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

float timeAccum = 0.0f;
void BoneAnimationSystem::VUpdate(const float dt, const std::vector<ecs::EntityId>& entitiesToProcess) const
{
    auto& world = ecs::World::GetInstance();
    for (const auto& entityId : entitiesToProcess)
    {
        auto& renderableComponent = world.GetComponent<RenderableComponent>(entityId);
        if (renderableComponent.mMeshResourceIds.size() <= 0)
        {
            continue;
        }
        
        auto& currentMesh = resources::ResourceLoadingService::GetInstance().GetResource<resources::MeshResource>(renderableComponent.mMeshResourceIds[renderableComponent.mCurrentMeshResourceIndex]);
        if (!currentMesh.HasSkeleton())
        {
            continue;
        }
        
        timeAccum += dt;
        glm::mat4 transform(1.0f);
        
        const auto& animationInfo = currentMesh.GetAnimationInfo();
        auto animationTime = std::fmod(timeAccum, animationInfo.mDuration);
        
        CalculateTransformsInHierarchy(animationTime, currentMesh.GetRootSkeletonNode(), transform, currentMesh);
        
        const auto nBones = currentMesh.GetBoneInfo().size();
        renderableComponent.mShaderUniforms.mShaderMatrixArrayUniforms[StringId("bones")].resize(nBones);
        for (auto i = 0U; i < nBones; ++i)
        {
            renderableComponent.mShaderUniforms.mShaderMatrixArrayUniforms[StringId("bones")][i] = currentMesh.GetBoneInfo()[i].mBoneFinalTransformMatrix;
        }
    }
}

///-----------------------------------------------------------------------------------------------

void BoneAnimationSystem::CalculateTransformsInHierarchy(const float animationTime, const resources::SkeletonNode* node, const glm::mat4& parentTransform, resources::MeshResource& meshResource) const
{
    auto nodeTransform = node->mTransform;
    const auto& animationInfo = meshResource.GetAnimationInfo();
    
    if (animationInfo.mBoneNameToAnimInfo.count(node->mNodeName) > 0)
    {
        auto nodeAnim = animationInfo.mBoneNameToAnimInfo.at(node->mNodeName);
        
        // Find closest key frame
        auto keyFrameIndex = -1;
        for (unsigned int i = 0 ; i < nodeAnim.mPositionKeys.size() - 1; i++)
        {
            if (animationTime < static_cast<float>(nodeAnim.mPositionKeys[i + 1].mTime))
            {
                keyFrameIndex = i;
                break;
            }
        }
        assert(keyFrameIndex > -1);
        
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scaling;
        
        // Calculate interpolated position
        if (nodeAnim.mPositionKeys.size() == 1) {
            rotation = nodeAnim.mPositionKeys[0].mPosition;
        }
        else
        {
            uint nextPositionIndex = (keyFrameIndex + 1);
            assert(nextPositionIndex < nodeAnim.mPositionKeys.size());
            float dt = static_cast<float>(nodeAnim.mPositionKeys[nextPositionIndex].mTime - nodeAnim.mPositionKeys[keyFrameIndex].mTime);
            float factor = (animationTime - (float)nodeAnim.mPositionKeys[keyFrameIndex].mTime) / dt;
            assert(factor >= 0.0f && factor <= 1.0f);
            const auto& start = nodeAnim.mPositionKeys[keyFrameIndex].mPosition;
            const auto& end   = nodeAnim.mPositionKeys[nextPositionIndex].mPosition;
            position = start + factor * (end - start);
        }
        
        // Calculate interpolated rotation
        if (nodeAnim.mRotationKeys.size() == 1) {
            rotation = nodeAnim.mRotationKeys[0].mRotation;
        }
        else
        {
            uint nextRotationIndex = (keyFrameIndex + 1);
            assert(nextRotationIndex < nodeAnim.mRotationKeys.size());
            float dt = static_cast<float>(nodeAnim.mRotationKeys[nextRotationIndex].mTime - nodeAnim.mRotationKeys[keyFrameIndex].mTime);
            float factor = (animationTime - (float)nodeAnim.mRotationKeys[keyFrameIndex].mTime) / dt;
            assert(factor >= 0.0f && factor <= 1.0f);
            const auto& start = nodeAnim.mRotationKeys[keyFrameIndex].mRotation;
            const auto& end   = nodeAnim.mRotationKeys[nextRotationIndex].mRotation;
            aiQuaternion aiQuatStart(start.w, start.x, start.y, start.z);
            aiQuaternion aiQuatEnd(end.w, end.x, end.y, end.z);
            aiQuaternion lerp;
            aiQuaternion::Interpolate(lerp, aiQuatStart, aiQuatEnd, factor);
            rotation = math::AssimpQuatToGlmQuat(lerp);
        }
        
        // Calculate interpolated scaling
        if (nodeAnim.mScalingKeys.size() == 1) {
            scaling = nodeAnim.mScalingKeys[0].mScale;
        }
        else
        {
            uint nextScalingIndex = (keyFrameIndex + 1);
            assert(nextScalingIndex < nodeAnim.mScalingKeys.size());
            float dt = static_cast<float>(nodeAnim.mScalingKeys[nextScalingIndex].mTime - nodeAnim.mScalingKeys[keyFrameIndex].mTime);
            float factor = (animationTime - (float)nodeAnim.mScalingKeys[keyFrameIndex].mTime) / dt;
            assert(factor >= 0.0f && factor <= 1.0f);
            const auto& start = nodeAnim.mScalingKeys[keyFrameIndex].mScale;
            const auto& end   = nodeAnim.mScalingKeys[nextScalingIndex].mScale;
            scaling = start + factor * (end - start);
        }
        
        glm::mat4 rotMatrix = glm::mat4_cast(rotation);
        nodeTransform = glm::mat4(1.0f);
        nodeTransform = glm::translate(nodeTransform, position);
        nodeTransform *= rotMatrix;
        nodeTransform = glm::scale(nodeTransform, scaling);
    }
    
    auto globalTransform = parentTransform * nodeTransform;
    auto& boneInfo = meshResource.GetBoneInfo();
    const auto& boneNameToIdMap = meshResource.GetBoneNameToIdMap();
    
    if (boneNameToIdMap.find(node->mNodeName) != boneNameToIdMap.end())
    {
        auto boneIndex = boneNameToIdMap.at(node->mNodeName);
        boneInfo[boneIndex].mBoneFinalTransformMatrix = meshResource.GetSceneTransform() * globalTransform * boneInfo[boneIndex].mBoneOffsetMatrix;
    }
    
    for (int i = 0; i < node->mNumChildren; ++i)
    {
        CalculateTransformsInHierarchy(animationTime, node->mChildren[i], globalTransform, meshResource);
    }
}

///-----------------------------------------------------------------------------------------------

}

}

