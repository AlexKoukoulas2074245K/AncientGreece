///------------------------------------------------------------------------------------------------
///  ShaderLoader.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///------------------------------------------------------------------------------------------------

#include "ShaderLoader.h"
#include "../common/utils/Logging.h"
#include "../common/utils/OSMessageBox.h"
#include "../common/utils/StringUtils.h"
#include "../resources/ShaderResource.h"
#include "../rendering/opengl/Context.h"

#include <fstream>   // ifstream
#include <streambuf> // istreambuf_iterator

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace resources
{

///------------------------------------------------------------------------------------------------

const std::string ShaderLoader::VERTEX_SHADER_FILE_EXTENSION = ".vs";
const std::string ShaderLoader::FRAGMENT_SHADER_FILE_EXTENSION = ".fs";

///------------------------------------------------------------------------------------------------

static void ExtractUniformFromLine(const std::string& line, const GLuint programId, tsl::robin_map<StringId, GLuint, StringIdHasher>& outUniformNamesToLocations);

///------------------------------------------------------------------------------------------------

void ShaderLoader::VInitialize()
{
}

///------------------------------------------------------------------------------------------------

std::unique_ptr<IResource> ShaderLoader::VCreateAndLoadResource(const std::string& resourcePathWithExtension) const
{
        // Since the shader loading is signalled by the .vs or .fs extension, we need to trim it here after
    // being added by the ResourceLoadingService prior to this call
    const auto resourcePath = resourcePathWithExtension.substr(0, resourcePathWithExtension.size() - 3);

    // Generate vertex shader id
    const auto vertexShaderId = GL_NO_CHECK(glCreateShader(GL_VERTEX_SHADER));
    
    // Read vertex shader source
    const auto vertexShaderFileContents = ReadFileContents(resourcePath + VERTEX_SHADER_FILE_EXTENSION);
    const char* vertexShaderFileContentsPtr = vertexShaderFileContents.c_str();

    // Compile vertex shader
    GL_CHECK(glShaderSource(vertexShaderId, 1, &vertexShaderFileContentsPtr, nullptr));
    GL_CHECK(glCompileShader(vertexShaderId));
    
    // Check vertex shader compilation
    std::string vertexShaderInfoLog;
    GLint vertexShaderInfoLogLength;
    GL_CHECK(glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &vertexShaderInfoLogLength));
    if (vertexShaderInfoLogLength > 0)
    {
        vertexShaderInfoLog.clear();
        vertexShaderInfoLog.reserve(vertexShaderInfoLogLength);
        GL_CHECK(glGetShaderInfoLog(vertexShaderId, vertexShaderInfoLogLength, nullptr, &vertexShaderInfoLog[0]));
        Log(LogType::INFO, "While compiling vertex shader %s%s:\n%s", resourcePath.c_str(), ".vs", vertexShaderInfoLog.c_str());
    }
    
    // Generate fragment shader id
    const auto fragmentShaderId = GL_NO_CHECK(glCreateShader(GL_FRAGMENT_SHADER));
    
    // Read vertex shader source
    const auto fragmentShaderFileContents = ReadFileContents(resourcePath + FRAGMENT_SHADER_FILE_EXTENSION);
    const char* fragmentShaderFileContentsPtr = fragmentShaderFileContents.c_str();
    
    GL_CHECK(glShaderSource(fragmentShaderId, 1, &fragmentShaderFileContentsPtr, nullptr));
    GL_CHECK(glCompileShader(fragmentShaderId));
    
    std::string fragmentShaderInfoLog;
    GLint fragmentShaderInfoLogLength;
    GL_CHECK(glGetShaderiv(fragmentShaderId, GL_INFO_LOG_LENGTH, &fragmentShaderInfoLogLength));
    if (fragmentShaderInfoLogLength > 0)
    {
        fragmentShaderInfoLog.clear();
        fragmentShaderInfoLog.reserve(fragmentShaderInfoLogLength);
        GL_CHECK(glGetShaderInfoLog(fragmentShaderId, fragmentShaderInfoLogLength, nullptr, &fragmentShaderInfoLog[0]));
        Log(LogType::INFO, "While compiling fragment shader %s%s:\n%s", resourcePath.c_str(), ".fs", fragmentShaderInfoLog.c_str());
    }

    // Link shader program
    const auto programId = GL_NO_CHECK(glCreateProgram());
    GL_CHECK(glAttachShader(programId, vertexShaderId));
    GL_CHECK(glAttachShader(programId, fragmentShaderId));
    GL_CHECK(glLinkProgram(programId));
    
#ifndef _WIN32
    std::string linkingInfoLog;
    GLint linkingInfoLogLength;
    
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &linkingInfoLogLength);
    if (linkingInfoLogLength > 0)
    {
        linkingInfoLog.clear();
        linkingInfoLog.reserve(linkingInfoLogLength);
        GL_CHECK(glGetProgramInfoLog(programId, linkingInfoLogLength, NULL, &linkingInfoLog[0]));
        Log(LogType::INFO, "While linking shader %s:\n%s", resourcePath.c_str(), linkingInfoLog.c_str());
    }
#endif
    
    GL_CHECK(glValidateProgram(programId));
    
#ifndef _WIN32
    GLint status;
    std::string validateInfoLog;
    GLint validateInfoLogLength;
    
    GL_CHECK(glGetProgramiv(programId, GL_VALIDATE_STATUS, &status));
    glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &validateInfoLogLength);
    if (validateInfoLogLength > 0)
    {
        validateInfoLog.clear();
        validateInfoLog.reserve(validateInfoLogLength);
        GL_CHECK(glGetProgramInfoLog(programId, validateInfoLogLength, NULL, &validateInfoLog[0]));
        Log(LogType::INFO, "While validating shader %s:\n%s", resourcePath.c_str(), validateInfoLog.c_str());
    }
#endif
    
    // Destroy intermediate compiled shaders
    GL_CHECK(glDetachShader(programId, vertexShaderId));
    GL_CHECK(glDetachShader(programId, fragmentShaderId));
    GL_CHECK(glDeleteShader(vertexShaderId));
    GL_CHECK(glDeleteShader(fragmentShaderId));
    
    Log(LogType::INFO, "Parsing uniforms in shader %s", resourcePath.c_str());
    const auto uniformNamesToLocations = GetUniformNamesToLocationsMap(programId, vertexShaderFileContents, fragmentShaderFileContents);
    return std::make_unique<ShaderResource>(uniformNamesToLocations, programId);
}

///------------------------------------------------------------------------------------------------

std::string ShaderLoader::ReadFileContents(const std::string& filePath) const
{
    std::ifstream file(filePath);
    
    if (!file.good())
    {
        ShowMessageBox(MessageBoxType::ERROR, "File could not be found", filePath.c_str());
        return nullptr;
    }
    
    std::string contents;
    
    file.seekg(0, std::ios::end);
    contents.reserve(static_cast<size_t>(file.tellg()));
    file.seekg(0, std::ios::beg);
    
    contents.assign((std::istreambuf_iterator<char>(file)),
               std::istreambuf_iterator<char>());
    
    return contents;
}

///------------------------------------------------------------------------------------------------

tsl::robin_map<StringId, GLuint, StringIdHasher> ShaderLoader::GetUniformNamesToLocationsMap
(
    const GLuint programId,
    const std::string& vertexShaderFileContents,
    const std::string& fragmentShaderFileContents
) const
{
    tsl::robin_map<StringId, GLuint, StringIdHasher> uniformNamesToLocationsMap;
    
    const auto vertexShaderContentSplitByNewline = StringSplit(vertexShaderFileContents, '\n');
    for (const auto& vertexShaderLine: vertexShaderContentSplitByNewline)
    {
        if (StringStartsWith(vertexShaderLine, "uniform"))
        {
            ExtractUniformFromLine(vertexShaderLine, programId, uniformNamesToLocationsMap);
        }
    }
    
    const auto fragmentShaderContentSplitByNewline = StringSplit(fragmentShaderFileContents, '\n');
    for (const auto& fragmentShaderLine: fragmentShaderContentSplitByNewline)
    {
        if (StringStartsWith(fragmentShaderLine, "uniform"))
        {
            ExtractUniformFromLine(fragmentShaderLine, programId, uniformNamesToLocationsMap);
        }
    }
    
    return uniformNamesToLocationsMap;
}

///------------------------------------------------------------------------------------------------

void ExtractUniformFromLine(const std::string& line, const GLuint programId, tsl::robin_map<StringId, GLuint, StringIdHasher>& outUniformNamesToLocations)
{
    const auto uniformLineSplitBySpace = StringSplit(line, ' ');
    
    // Uniform names will always be the third components in the line
    // e.g. uniform bool foo
    auto uniformName = uniformLineSplitBySpace[2].substr(0, uniformLineSplitBySpace[2].size() - 1);
    
    // Check for uniform array
    if (uniformName.at(uniformName.size() - 1) == ']')
    {
        uniformName = uniformName.substr(0, uniformName.size() - 1);
        const auto uniformNameSplitByLeftSquareBracket = StringSplit(uniformName, '[');
        
        uniformName = uniformNameSplitByLeftSquareBracket[0];
        
        if (StringIsInt(uniformNameSplitByLeftSquareBracket[1]) == false)
        {
            ShowMessageBox(MessageBoxType::ERROR, "Error Extracting Uniform", "Could not parse array element count for uniform: " + uniformName);
        }
        
        const auto numberOfElements = std::stoi(uniformNameSplitByLeftSquareBracket[1]);
        
        for (int i = 0; i < numberOfElements; ++i)
        {
            const auto indexedUniformName = uniformName + "[" + std::to_string(i) + "]";
            const auto uniformLocation = GL_NO_CHECK(glGetUniformLocation(programId, indexedUniformName.c_str()));
            outUniformNamesToLocations[StringId(indexedUniformName)] = uniformLocation;
            
            if (uniformLocation == -1)
            {
                Log(LogType::WARNING, "Unused uniform at location -1: %s", indexedUniformName.c_str());
            }
        }
    }
    // Normal uniform
    else
    {
        auto uniformLocation = GL_NO_CHECK(glGetUniformLocation(programId, uniformName.c_str()));
        outUniformNamesToLocations[StringId(uniformName)] = uniformLocation;
        
        if (uniformLocation == -1)
        {
            Log(LogType::WARNING, "Unused uniform at location -1: %s", uniformName.c_str());
        }
    }
}

///------------------------------------------------------------------------------------------------
}

}
