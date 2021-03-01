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
    tsl::robin_map<StringId, std::vector<glm::vec4>, StringIdHasher> mShaderFloatVec4ArrayUniforms;
    tsl::robin_map<StringId, std::vector<glm::vec3>, StringIdHasher> mShaderFloatVec3ArrayUniforms;
    tsl::robin_map<StringId, glm::mat4, StringIdHasher> mShaderMatrixUniforms;
    tsl::robin_map<StringId, glm::vec4, StringIdHasher> mShaderFloatVec4Uniforms;
    tsl::robin_map<StringId, glm::vec3, StringIdHasher> mShaderFloatVec3Uniforms;
    tsl::robin_map<StringId, float, StringIdHasher> mShaderFloatUniforms;
    tsl::robin_map<StringId, int, StringIdHasher> mShaderIntUniforms;
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

class RenderableComponent final: public ecs::IComponent
{
public:    
    ShaderUniforms mShaderUniforms;
    MaterialProperties mMaterial;
    ResourceId mMeshResourceId    = 0;
    ResourceId mTextureResourceId = 0;            
    StringId mShaderNameId        = StringId();
    bool mIsVisible               = true;
    bool mIsGuiComponent          = false;
    bool mIsAffectedByLight       = false;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* RenderableComponent_h */
