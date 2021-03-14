///------------------------------------------------------------------------------------------------
///  MeshResource.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef MeshResource_h
#define MeshResource_h

///------------------------------------------------------------------------------------------------

#include "IResource.h"
#include "../common/utils/MathUtils.h"
#include "../common/utils/StringUtils.h"
#include <assimp/scene.h>
#include <tsl/robin_map.h>
#include <list>
#include <vector>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace resources
{

///------------------------------------------------------------------------------------------------

using GLuint = unsigned int;

///------------------------------------------------------------------------------------------------

struct SkeletonNode
{
    SkeletonNode** mChildren;
    glm::mat4 mTransform;
    StringId mNodeName;
    int mNumChildren;
};

///------------------------------------------------------------------------------------------------

struct PositionKey
{
    float mTime;
    glm::vec3 mPosition;
};

///------------------------------------------------------------------------------------------------

struct RotationKey
{
    float mTime;
    glm::quat mRotation;
};

///------------------------------------------------------------------------------------------------

struct ScalingKey
{
    float mTime;
    glm::vec3 mScale;
};

///------------------------------------------------------------------------------------------------

struct BoneAnimationInfo
{
    std::vector<PositionKey> mPositionKeys;
    std::vector<RotationKey> mRotationKeys;
    std::vector<ScalingKey> mScalingKeys;
};

///------------------------------------------------------------------------------------------------

struct AnimationInfo
{
    float mTicksPerSecond;
    float mDuration;
    tsl::robin_map<StringId, BoneAnimationInfo, StringIdHasher> mBoneNameToAnimInfo;
};

///------------------------------------------------------------------------------------------------

class MeshResource final: public IResource
{
    friend class OBJMeshLoader;
    friend class DAEMeshLoader;
    
public:
    ~MeshResource();
    
    GLuint GetVertexArrayObject() const;
    GLuint GetElementCount() const;
    const glm::vec3& GetDimensions() const;
    bool HasSkeleton() const;
    const SkeletonNode* GetRootSkeletonNode() const;
    const AnimationInfo& GetAnimationInfo() const;
    const tsl::robin_map<StringId, unsigned int, StringIdHasher>& GetBoneNameToIdMap() const;
    const glm::mat4& GetSceneTransform() const;
    const std::vector<glm::mat4>& GetBoneOffsetMatrices() const;
    
private:
    // Animated model (DAE) constructor
    MeshResource(const AnimationInfo& animationInfo, const std::vector<glm::mat4>& boneOffsetMatrices, const tsl::robin_map<StringId, unsigned int, StringIdHasher>& boneMapping, const glm::mat4& sceneTransform, const aiNode* rootAssimpNode, const GLuint vertexArrayObject, const GLuint elementCount, const glm::vec3& meshDimensions);
    
    // Static model (OBJ) constructor
    MeshResource(const GLuint vertexArrayObject, const GLuint elementCount, const glm::vec3& meshDimensions);
    
private:
    void CreateSkeleton(const aiNode* rootAssimpNode, SkeletonNode* skeletonNode);
    void DestroySkeleton(SkeletonNode* skeletonNode);
    
private:
    const AnimationInfo mAnimationInfo;
    const std::vector<glm::mat4> mBoneOffsetMatrices;
    const tsl::robin_map<StringId, unsigned int, StringIdHasher> mBoneNameToIdMap;
    const glm::mat4 mSceneTransform;
    const GLuint mVertexArrayObject;
    const GLuint mElementCount;
    const glm::vec3 mDimensions;
    SkeletonNode* mRootSkeletonNode;
    
};

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* MeshResource_h */
