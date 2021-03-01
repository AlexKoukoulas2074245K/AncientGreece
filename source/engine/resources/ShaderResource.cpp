///------------------------------------------------------------------------------------------------
///  ShaderResource.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#include "ShaderResource.h"
#include "../rendering/opengl/Context.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace resources
{

///------------------------------------------------------------------------------------------------

ShaderResource::ShaderResource
(
    const tsl::robin_map<StringId, GLuint, StringIdHasher> uniformNamesToLocations,
    const GLuint programId
)
    : mShaderUniformNamesToLocations(uniformNamesToLocations) 
    , mProgramId(programId)
{
    
}

///------------------------------------------------------------------------------------------------

ShaderResource& ShaderResource::operator = (const ShaderResource& rhs)
{
    CopyConstruction(rhs);
    return *this;
}

///------------------------------------------------------------------------------------------------

ShaderResource::ShaderResource(const ShaderResource& rhs)
{
    CopyConstruction(rhs);
}

///------------------------------------------------------------------------------------------------

bool ShaderResource::SetMatrix4fv
(
    const StringId& uniformName, 
    const glm::mat4& matrix, 
    const GLuint count /* 1 */,
    const bool transpose /* false */
) const
{
    if (mShaderUniformNamesToLocations.count(uniformName) > 0)
    {
        GL_CHECK(glUniformMatrix4fv(mShaderUniformNamesToLocations.at(uniformName), count, transpose, (GLfloat*)&matrix));
        return true;
    }    
    return false;
}

///------------------------------------------------------------------------------------------------

bool ShaderResource::SetFloatVec4Array(const StringId& uniformName, const std::vector<glm::vec4>& values) const
{
    for (auto i = 0U; i < values.size(); ++i)
    {
        auto setUniformResult = SetFloatVec4(StringId(uniformName.GetString() + "[" + std::to_string(i) + "]"), values[i]);
        if (!setUniformResult)
        {
            return false;
        }
    }
    
    return true;
}

///------------------------------------------------------------------------------------------------

bool ShaderResource::SetFloatVec3Array(const StringId& uniformName, const std::vector<glm::vec3>& values) const
{
    for (auto i = 0U; i < values.size(); ++i)
    {
        auto setUniformResult = SetFloatVec3(StringId(uniformName.GetString() + "[" + std::to_string(i) + "]"), values[i]);
        if (!setUniformResult)
        {
            return false;
        }
    }
    
    return true;
}

///------------------------------------------------------------------------------------------------

bool ShaderResource::SetFloatVec4(const StringId& uniformName, const glm::vec4& vec) const
{
    if (mShaderUniformNamesToLocations.count(uniformName) > 0)
    {
        GL_CHECK(glUniform4f(mShaderUniformNamesToLocations.at(uniformName), vec.x, vec.y, vec.z, vec.w));
        return true;
    }
    return false;
}

///------------------------------------------------------------------------------------------------

bool ShaderResource::SetFloatVec3(const StringId& uniformName, const glm::vec3& vec) const
{
    if (mShaderUniformNamesToLocations.count(uniformName) > 0)
    {
        GL_CHECK(glUniform3f(mShaderUniformNamesToLocations.at(uniformName), vec.x, vec.y, vec.z));
        return true;
    }
    return false;
}

///------------------------------------------------------------------------------------------------

bool ShaderResource::SetFloat(const StringId& uniformName, const float value) const
{
    if (mShaderUniformNamesToLocations.count(uniformName) > 0)
    {
        GL_CHECK(glUniform1f(mShaderUniformNamesToLocations.at(uniformName), value));
        return true;
    }
    return false;
}

///------------------------------------------------------------------------------------------------

bool ShaderResource::SetFloatArray(const StringId& uniformName, const std::vector<float>& values) const
{
    for (auto i = 0U; i < values.size(); ++i)
    {
        auto setUniformResult = SetFloat(StringId(uniformName.GetString() + "[" + std::to_string(i) + "]"), values[i]);
        if (!setUniformResult)
        {
            return false;
        }
    }
    
    return true;
}

///------------------------------------------------------------------------------------------------

bool ShaderResource::SetInt(const StringId& uniformName, const int value) const
{
    if (mShaderUniformNamesToLocations.count(uniformName) > 0)
    {
        GL_CHECK(glUniform1i(mShaderUniformNamesToLocations.at(uniformName), value));
        return true;
    }
    return false;
}

///------------------------------------------------------------------------------------------------

GLuint ShaderResource::GetProgramId() const
{
    return mProgramId;
}

///------------------------------------------------------------------------------------------------

const tsl::robin_map<StringId, GLuint, StringIdHasher>& ShaderResource::GetUniformNamesToLocations() const
{
    return mShaderUniformNamesToLocations;
}

///------------------------------------------------------------------------------------------------

void ShaderResource::CopyConstruction(const ShaderResource& rhs)
{
    mProgramId = rhs.GetProgramId();
    for (const auto& uniformEntry: rhs.GetUniformNamesToLocations())
    {
        mShaderUniformNamesToLocations[uniformEntry.first] = uniformEntry.second;
    }
}

///------------------------------------------------------------------------------------------------

}

}

