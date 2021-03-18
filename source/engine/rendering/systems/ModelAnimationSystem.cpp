///------------------------------------------------------------------------------------------------
///  ModelAnimationSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 08/03/2021.
///-----------------------------------------------------------------------------------------------

#include "ModelAnimationSystem.h"
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

namespace
{
    static const float ANIMATION_TRANSITION_TIME = 0.2f;
    static const StringId BONES_UNIFORM_NAME     = StringId("bones");
}

///-----------------------------------------------------------------------------------------------

ModelAnimationSystem::ModelAnimationSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void ModelAnimationSystem::VUpdate(const float dt, const std::vector<ecs::EntityId>& entitiesToProcess) const
{
    const auto& world = ecs::World::GetInstance();
    for (const auto& entityId : entitiesToProcess)
    {
        auto& renderableComponent = world.GetComponent<RenderableComponent>(entityId);
        if (renderableComponent.mMeshResourceIds.size() <= 0)
        {
            continue;
        }
        
        const auto& currentMesh = resources::ResourceLoadingService::GetInstance().GetResource<resources::MeshResource>(renderableComponent.mMeshResourceIds[renderableComponent.mCurrentMeshResourceIndex]);
        if (!currentMesh.HasSkeleton())
        {
            continue;
        }
        
        glm::mat4 transform(1.0f);
        
        if (renderableComponent.mPreviousMeshResourceIndex != -1)
        {
            renderableComponent.mTransitionAnimationTimeAccum += dt;
            if (renderableComponent.mTransitionAnimationTimeAccum >= ANIMATION_TRANSITION_TIME)
            {
                // Transition to next anim finished
                renderableComponent.mPreviousMeshResourceIndex = -1;
                renderableComponent.mTransitionAnimationTimeAccum = 0.0f;
                renderableComponent.mAnimationTimeAccum = 0.0f;
            }
            else
            {
                // Transition to next anim ongoing
                const auto& previousMesh = resources::ResourceLoadingService::GetInstance().GetResource<resources::MeshResource>(renderableComponent.mMeshResourceIds[renderableComponent.mPreviousMeshResourceIndex]);
                
                const auto transitionAnimationTime = std::fmod(renderableComponent.mTransitionAnimationTimeAccum, ANIMATION_TRANSITION_TIME);
                const auto previousAnimationTime = std::fmod(renderableComponent.mAnimationTimeAccum, previousMesh.GetAnimationInfo().mDuration);
                CalculateTransitionalTransformsInHierarchy(previousAnimationTime, transitionAnimationTime, currentMesh.GetRootSkeletonNode(), transform, previousMesh, currentMesh, renderableComponent);
            }
        }
        else
        {
            // Current anim playing
            renderableComponent.mAnimationTimeAccum += dt;
            
            const auto& animationInfo = currentMesh.GetAnimationInfo();
            const auto animationTime = std::fmod(renderableComponent.mAnimationTimeAccum, animationInfo.mDuration);
            
            CalculateTransformsInHierarchy(animationTime, currentMesh.GetRootSkeletonNode(), transform, currentMesh, renderableComponent);
        }
        
        const auto nBones = currentMesh.GetBoneOffsetMatrices().size();
        for (auto i = 0U; i < nBones; ++i)
        {
            renderableComponent.mShaderUniforms.mShaderMatrixArrayUniforms[BONES_UNIFORM_NAME][i] = renderableComponent.mBoneTransformMatrices[i];
        }
    }
}

///-----------------------------------------------------------------------------------------------

void ModelAnimationSystem::CalculateTransitionalTransformsInHierarchy(const float previousAnimationTime, const float transitionAnimationTime, const resources::SkeletonNode* node, const glm::mat4& parentTransform, const resources::MeshResource& previousMeshResource, const resources::MeshResource& currentMeshResource, RenderableComponent& renderableComponent) const
{
    auto nodeTransform = node->mTransform;
    const auto& previousMeshAnimationInfo = previousMeshResource.GetAnimationInfo();
    
    if (previousMeshAnimationInfo.mBoneNameToAnimInfo.count(node->mNodeName) > 0)
    {
        const auto& previousNodeAnim = previousMeshAnimationInfo.mBoneNameToAnimInfo.at(node->mNodeName);
        const auto& transitionNodeAnim = currentMeshResource.GetAnimationInfo().mBoneNameToAnimInfo.at(node->mNodeName);
        
        // Find closest key frame
        auto keyFrameIndex = -1;
        for (unsigned int i = 0 ; i < previousNodeAnim.mPositionKeys.size() - 1; i++)
        {
            if (previousAnimationTime < static_cast<float>(previousNodeAnim.mPositionKeys[i + 1].mTime))
            {
                keyFrameIndex = i;
                break;
            }
        }
        assert(keyFrameIndex > -1);
        
        const auto factor = transitionAnimationTime/ANIMATION_TRANSITION_TIME;
        assert(factor >= 0.0f && factor <= 1.0f);
    
        glm::vec3 position;
        glm::quat rotation;
        glm::vec3 scaling;
       
        {
            const auto& start = previousNodeAnim.mPositionKeys[keyFrameIndex].mPosition;
            const auto& end   = transitionNodeAnim.mPositionKeys[0].mPosition;
            position = start + factor * (end - start);
        }
        
        {
            const auto& start = previousNodeAnim.mRotationKeys[keyFrameIndex].mRotation;
            const auto& end   = transitionNodeAnim.mRotationKeys[0].mRotation;
            rotation = glm::slerp(start, end, factor);
        }
        
        {
            const auto& start = previousNodeAnim.mScalingKeys[keyFrameIndex].mScale;
            const auto& end   = transitionNodeAnim.mScalingKeys[0].mScale;
            scaling = start + factor * (end - start);
        }
        
        glm::mat4 rotMatrix = glm::mat4_cast(rotation);
        nodeTransform = glm::mat4(1.0f);
        nodeTransform = glm::translate(nodeTransform, position);
        nodeTransform *= rotMatrix;
        nodeTransform = glm::scale(nodeTransform, scaling);
    }
    
    auto globalTransform = parentTransform * nodeTransform;
    const auto& boneNameToIdMap = previousMeshResource.GetBoneNameToIdMap();
    
    if (boneNameToIdMap.find(node->mNodeName) != boneNameToIdMap.end())
    {
        auto boneIndex = boneNameToIdMap.at(node->mNodeName);
        
        renderableComponent.mBoneTransformMatrices[boneIndex] = previousMeshResource.GetSceneTransform() * globalTransform * currentMeshResource.GetBoneOffsetMatrices()[boneIndex];
    }
    
    for (int i = 0; i < node->mNumChildren; ++i)
    {
        CalculateTransitionalTransformsInHierarchy(previousAnimationTime, transitionAnimationTime, node->mChildren[i], globalTransform, previousMeshResource, currentMeshResource, renderableComponent);
    }
}

///-----------------------------------------------------------------------------------------------

void ModelAnimationSystem::CalculateTransformsInHierarchy(const float animationTime, const resources::SkeletonNode* node, const glm::mat4& parentTransform, const resources::MeshResource& meshResource, RenderableComponent& renderableComponent) const
{
    auto nodeTransform = node->mTransform;
    const auto& animationInfo = meshResource.GetAnimationInfo();
    
    if (animationInfo.mBoneNameToAnimInfo.count(node->mNodeName) > 0)
    {
        const auto& nodeAnim = animationInfo.mBoneNameToAnimInfo.at(node->mNodeName);
        
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
        if (nodeAnim.mPositionKeys.size() == 1)
        {
            rotation = nodeAnim.mPositionKeys[0].mPosition;
        }
        else
        {
            uint nextPositionIndex = (keyFrameIndex + 1);
            assert(nextPositionIndex < nodeAnim.mPositionKeys.size());
            float dt = static_cast<float>(nodeAnim.mPositionKeys[nextPositionIndex].mTime - nodeAnim.mPositionKeys[keyFrameIndex].mTime);
            float factor = math::Max(0.0f, (animationTime - (float)nodeAnim.mPositionKeys[keyFrameIndex].mTime) / dt);
            assert(factor >= 0.0f && factor <= 1.0f);
            const auto& start = nodeAnim.mPositionKeys[keyFrameIndex].mPosition;
            const auto& end   = nodeAnim.mPositionKeys[nextPositionIndex].mPosition;
            position = start + factor * (end - start);
        }
        
        // Calculate interpolated rotation
        if (nodeAnim.mRotationKeys.size() == 1)
        {
            rotation = nodeAnim.mRotationKeys[0].mRotation;
        }
        else
        {
            uint nextRotationIndex = (keyFrameIndex + 1);
            assert(nextRotationIndex < nodeAnim.mRotationKeys.size());
            float dt = static_cast<float>(nodeAnim.mRotationKeys[nextRotationIndex].mTime - nodeAnim.mRotationKeys[keyFrameIndex].mTime);
            float factor = math::Max(0.0f, (animationTime - (float)nodeAnim.mRotationKeys[keyFrameIndex].mTime) / dt);
            assert(factor >= 0.0f && factor <= 1.0f);
            const auto& start = nodeAnim.mRotationKeys[keyFrameIndex].mRotation;
            const auto& end   = nodeAnim.mRotationKeys[nextRotationIndex].mRotation;
            rotation = glm::slerp(start, end, factor);
        }
        
        // Calculate interpolated scaling
        if (nodeAnim.mScalingKeys.size() == 1)
        {
            scaling = nodeAnim.mScalingKeys[0].mScale;
        }
        else
        {
            uint nextScalingIndex = (keyFrameIndex + 1);
            assert(nextScalingIndex < nodeAnim.mScalingKeys.size());
            float dt = static_cast<float>(nodeAnim.mScalingKeys[nextScalingIndex].mTime - nodeAnim.mScalingKeys[keyFrameIndex].mTime);
            float factor = math::Max(0.0f, (animationTime - (float)nodeAnim.mScalingKeys[keyFrameIndex].mTime) / dt);
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
    const auto& boneNameToIdMap = meshResource.GetBoneNameToIdMap();
    
    if (boneNameToIdMap.find(node->mNodeName) != boneNameToIdMap.end())
    {
        auto boneIndex = boneNameToIdMap.at(node->mNodeName);
        renderableComponent.mBoneTransformMatrices[boneIndex] = meshResource.GetSceneTransform() * globalTransform * meshResource.GetBoneOffsetMatrices()[boneIndex];
    }
    
    for (int i = 0; i < node->mNumChildren; ++i)
    {
        CalculateTransformsInHierarchy(animationTime, node->mChildren[i], globalTransform, meshResource, renderableComponent);
    }
}

///-----------------------------------------------------------------------------------------------

}

}

