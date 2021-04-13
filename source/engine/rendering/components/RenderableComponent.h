///------------------------------------------------------------------------------------------------
///  RenderableComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef RenderableComponent_h
#define RenderableComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../common/utils/MathUtils.h"
#include "../../common/utils/StringUtils.h"

#include <tsl/robin_map.h>
#include <vector>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------

using ResourceId = unsigned int;

///-----------------------------------------------------------------------------------------------

struct ShaderUniforms final
{
    tsl::robin_map<StringId, std::vector<glm::mat4>, StringIdHasher> mShaderMatrixArrayUniforms;
    tsl::robin_map<StringId, std::vector<glm::vec4>, StringIdHasher> mShaderFloatVec4ArrayUniforms;
    tsl::robin_map<StringId, std::vector<glm::vec3>, StringIdHasher> mShaderFloatVec3ArrayUniforms;
    tsl::robin_map<StringId, glm::mat4, StringIdHasher> mShaderMatrixUniforms;
    tsl::robin_map<StringId, glm::vec4, StringIdHasher> mShaderFloatVec4Uniforms;
    tsl::robin_map<StringId, glm::vec3, StringIdHasher> mShaderFloatVec3Uniforms;
    tsl::robin_map<StringId, float, StringIdHasher> mShaderFloatUniforms;
    tsl::robin_map<StringId, int, StringIdHasher> mShaderIntUniforms;
    tsl::robin_map<StringId, bool, StringIdHasher> mShaderBoolUniforms;
};

///-----------------------------------------------------------------------------------------------

struct MaterialProperties final
{
    glm::vec4 mAmbient;
    glm::vec4 mDiffuse;
    glm::vec4 mSpecular;
    float mShininess;
};

///-----------------------------------------------------------------------------------------------

enum class RenderableType
{
    NORMAL_MODEL,
    GUI_SPRITE,
    GUI_3D_MODEL,
    TEXT_3D_MODEL
};

///-----------------------------------------------------------------------------------------------

class RenderableComponent final: public ecs::IComponent
{
public:
    std::vector<glm::mat4> mBoneTransformMatrices;
    std::vector<ResourceId> mMeshResourceIds;
    tsl::robin_map<StringId, int, StringIdHasher> mAnimNameToMeshIndex;
    ShaderUniforms mShaderUniforms;
    MaterialProperties mMaterial;
    ResourceId mTextureResourceId       = 0;
    StringId mShaderNameId              = StringId();
    int mCurrentMeshResourceIndex       = 0;
    int mPreviousMeshResourceIndex      = -1;
    float mAnimationTimeAccum           = 0.0f;
    float mTransitionAnimationTimeAccum = 0.0f;
    float mAnimationSpeed               = 1.5f;
    RenderableType mRenderableType      = RenderableType::NORMAL_MODEL;
    bool mIsVisible                     = true;
    bool mIsAffectedByLight             = false;
    bool mIsCastingShadows              = false;
    bool mIsLoopingAnimation            = false;
    bool mShouldAnimateSkeleton         = true;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* RenderableComponent_h */
