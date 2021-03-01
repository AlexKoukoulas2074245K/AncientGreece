///------------------------------------------------------------------------------------------------
///  ShaderResource.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef ShaderResource_h
#define ShaderResource_h

///------------------------------------------------------------------------------------------------

#include "IResource.h"
#include "../common/utils/MathUtils.h"
#include "../common/utils/StringUtils.h"
#include "../rendering/systems/RenderingSystem.h"

#include <string>
#include <tsl/robin_map.h>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace resources
{

///------------------------------------------------------------------------------------------------

using GLuint = unsigned int;

///------------------------------------------------------------------------------------------------

class ShaderResource final: public IResource
{
    friend class rendering::RenderingSystem;
    
public:
    ShaderResource() = default;
    ShaderResource
    (
        const tsl::robin_map<StringId, GLuint, StringIdHasher> uniformNamesToLocations,
        const GLuint programId
    );
    ShaderResource& operator = (const ShaderResource&);
    ShaderResource(const ShaderResource&);
    
private:
    bool SetMatrix4fv(const StringId& uniformName, const glm::mat4& matrix, const GLuint count = 1, const bool transpose = false) const;
    bool SetFloatVec4Array(const StringId& uniformName, const std::vector<glm::vec4>& values) const;
    bool SetFloatVec3Array(const StringId& uniformName, const std::vector<glm::vec3>& values) const;
    bool SetFloatVec4(const StringId& uniformName, const glm::vec4& vec) const;
    bool SetFloatVec3(const StringId& uniformName, const glm::vec3& vec) const;
    bool SetFloat(const StringId& uniformName, const float value) const;
    bool SetFloatArray(const StringId& uniformName, const std::vector<float>& values) const;
    bool SetInt(const StringId& uniformName, const int value) const;

    GLuint GetProgramId() const;    

    const tsl::robin_map<StringId, GLuint, StringIdHasher>& GetUniformNamesToLocations() const;

    void CopyConstruction(const ShaderResource&);
    
private:
    tsl::robin_map<StringId, GLuint, StringIdHasher> mShaderUniformNamesToLocations;
    GLuint mProgramId;    
};

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* ShaderResource_h */
