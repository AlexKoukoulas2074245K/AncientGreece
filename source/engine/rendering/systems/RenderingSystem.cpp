///------------------------------------------------------------------------------------------------
///  RenderingSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#include "RenderingSystem.h"
#include "../components/CameraSingletonComponent.h"
#include "../components/LightStoreSingletonComponent.h"
#include "../components/RenderableComponent.h"
#include "../components/RenderingContextSingletonComponent.h"
#include "../components/ShaderStoreSingletonComponent.h"
#include "../components/TextStringComponent.h"
#include "../components/WindowSingletonComponent.h"
#include "../opengl/Context.h"
#include "../utils/CameraUtils.h"
#include "../../common/components/TransformComponent.h"
#include "../../common/utils/FileUtils.h"
#include "../../common/utils/Logging.h"
#include "../../common/utils/MathUtils.h"
#include "../../common/utils/OSMessageBox.h"
#include "../../resources/MeshResource.h"
#include "../../resources/ResourceLoadingService.h"
#include "../../resources/TextureResource.h"
#include "../../sound/SoundService.h"

#include <algorithm> // sort
#include <cstdlib>   // exit
#include <SDL.h> 
#include <vector>
#include <iterator>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------

namespace
{
    const StringId WORLD_MARIX_UNIFORM_NAME          = StringId("world");
    const StringId VIEW_MARIX_UNIFORM_NAME           = StringId("view");
    const StringId PROJECTION_MARIX_UNIFORM_NAME     = StringId("proj");
    const StringId NORMAL_MATRIX_UNIFORM_NAME        = StringId("norm");
    const StringId MATERIAL_AMBIENT_UNIFORM_NAME     = StringId("material_ambient");
    const StringId MATERIAL_DIFFUSE_UNIFORM_NAME     = StringId("material_diffuse");
    const StringId MATERIAL_SPECULAR_UNIFORM_NAME    = StringId("material_specular");
    const StringId MATERIAL_SHININESS_UNIFORM_NAME   = StringId("material_shininess");
    const StringId LIGHT_POSITIONS_UNIFORM_NAME      = StringId("light_positions");
    const StringId LIGHT_POWERS_UNIFORM_NAME         = StringId("light_powers");
    const StringId EYE_POSITION_UNIFORM_NAME         = StringId("eye_pos");
    const StringId IS_AFFECTED_BY_LIGHT_UNIFORM_NAME = StringId("is_affected_by_light");
}

///-----------------------------------------------------------------------------------------------

RenderingSystem::RenderingSystem()
    : BaseSystem()
{
    InitializeCamera();
    InitializeLights();
    CompileAndLoadShaders();
}

///-----------------------------------------------------------------------------------------------

void RenderingSystem::VUpdate(const float, const std::vector<ecs::EntityId>& entitiesToProcess) const
{    
    auto& world = ecs::World::GetInstance();

    // Get common rendering singleton components
    const auto& windowComponent      = world.GetSingletonComponent<WindowSingletonComponent>();
    const auto& shaderStoreComponent = world.GetSingletonComponent<ShaderStoreSingletonComponent>();
    const auto& lightStoreComponent  = world.GetSingletonComponent<LightStoreSingletonComponent>();
    auto& cameraComponent            = world.GetSingletonComponent<CameraSingletonComponent>();
    auto& renderingContextComponent  = world.GetSingletonComponent<RenderingContextSingletonComponent>();
    
    // Calculate render-constant camera view matrix
    cameraComponent.mViewMatrix = glm::lookAtLH(cameraComponent.mPosition, cameraComponent.mPosition + cameraComponent.mFrontVector, cameraComponent.mUpVector);
    cameraComponent.mViewMatrix = glm::rotate(cameraComponent.mViewMatrix, cameraComponent.mRoll, glm::vec3(0.0f, 0.0f, 1.0f));
    
    // Calculate render-constant camera projection matrix
    cameraComponent.mProjectionMatrix = glm::perspectiveFovLH
    (
        cameraComponent.mFieldOfView,
        windowComponent.mRenderableWidth,
        windowComponent.mRenderableHeight,
        cameraComponent.mZNear,
        cameraComponent.mZFar
    );
    
    // Calculate the camera frustum for this frame
    cameraComponent.mFrustum = CalculateCameraFrustum(cameraComponent.mViewMatrix, cameraComponent.mProjectionMatrix);
    
    // Collect all entities that need to be processed
    std::vector<ecs::EntityId> applicableEntities = entitiesToProcess;
    tsl::robin_map<RenderableType, std::vector<ecs::EntityId>> mGuiEntityGroups;
    
    // Set background color
    GL_CHECK(glClearColor
    (
        renderingContextComponent.mClearColor.x,
        renderingContextComponent.mClearColor.y,
        renderingContextComponent.mClearColor.z,
        renderingContextComponent.mClearColor.w
    ));
    
    // Clear buffers
    GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    // Enable depth
    GL_CHECK(glEnable(GL_DEPTH_TEST));
        
    // Sort entities based on their depth order to correct transparency
    std::sort(applicableEntities.begin(), applicableEntities.end(), [&world](const genesis::ecs::EntityId& lhs, const genesis::ecs::EntityId& rhs)
    {
        const auto& lhsTransformComponent = world.GetComponent<TransformComponent>(lhs);
        const auto& rhsTransformComponent = world.GetComponent<TransformComponent>(rhs);

        return lhsTransformComponent.mPosition.z > rhsTransformComponent.mPosition.z;
    });
    
    // Execute normal 3d model pass and save gui entities
    for (const auto& entityId : applicableEntities)
    {
        const auto& renderableComponent = world.GetComponent<RenderableComponent>(entityId);
        if (renderableComponent.mRenderableType == RenderableType::NORMAL_MODEL)
        {
            const auto& transformComponent = world.GetComponent<TransformComponent>(entityId);
            const auto& currentMesh        = resources::ResourceLoadingService::GetInstance().GetResource<resources::MeshResource>(renderableComponent.mMeshResourceIds[renderableComponent.mCurrentMeshResourceIndex]);

            // Frustum culling
            if (!math::IsMeshInsideFrustum
            (
                transformComponent.mPosition,
                transformComponent.mScale,
                currentMesh.GetDimensions(),
                cameraComponent.mFrustum
            ))
            {
                continue;
            }

            RenderEntityInternal
            (
                transformComponent,
                renderableComponent,
                cameraComponent,
                lightStoreComponent,
                shaderStoreComponent,
                windowComponent,
                renderingContextComponent
            );
        }
        else
        {
            mGuiEntityGroups[renderableComponent.mRenderableType].push_back(entityId);
        }
    }
    
    // Render 3d texts
    if (mGuiEntityGroups.count(RenderableType::TEXT_3D_MODEL))
    {
        const auto& text3dEntities = mGuiEntityGroups.at(RenderableType::TEXT_3D_MODEL);
        for (const auto& entityId : text3dEntities)
        {
            const auto& renderableComponent = world.GetComponent<RenderableComponent>(entityId);
            const auto& transformComponent = world.GetComponent<TransformComponent>(entityId);
            const auto& textStringComponent = world.GetComponent<TextStringComponent>(entityId);
            
            if (renderableComponent.mRenderableType == RenderableType::TEXT_3D_MODEL)
            {
                // Frustum culling
                if (!math::IsMeshInsideFrustum
                (
                    transformComponent.mPosition + glm::vec3(textStringComponent.mText.size() * textStringComponent.mCharacterSize/2, 0.0f, 0.0f),
                    transformComponent.mScale,
                    glm::vec3(textStringComponent.mText.size(), textStringComponent.mCharacterSize, textStringComponent.mCharacterSize),
                    cameraComponent.mFrustum
                ))
                {
                    continue;
                }
            }
            
            RenderStringInternal
            (
                transformComponent,
                renderableComponent,
                cameraComponent,
                lightStoreComponent,
                shaderStoreComponent,
                textStringComponent,
                windowComponent,
                renderingContextComponent
            );
        }
    }
    
    // Execute disabled detph test GUI pass
    GL_CHECK(glDisable(GL_DEPTH_TEST));
    
    // Execute normal gui sprite pass
    if (mGuiEntityGroups.count(RenderableType::GUI_SPRITE))
    {
        const auto& guiEntities = mGuiEntityGroups.at(RenderableType::GUI_SPRITE);
        for (const auto& entityId : guiEntities)
        {
            const auto& renderableComponent = world.GetComponent<RenderableComponent>(entityId);
            const auto& transformComponent = world.GetComponent<TransformComponent>(entityId);
            
            // If normal gui text entity render text
            if (world.HasComponent<TextStringComponent>(entityId))
            {
                const auto& textStringComponent = world.GetComponent<TextStringComponent>(entityId);
                RenderStringInternal
                (
                    transformComponent,
                    renderableComponent,
                    cameraComponent,
                    lightStoreComponent,
                    shaderStoreComponent,
                    textStringComponent,
                    windowComponent,
                    renderingContextComponent
                );
            }
            // Else render normal gui entity
            else
            {
                RenderEntityInternal
                (
                    transformComponent,
                    renderableComponent,
                    cameraComponent,
                    lightStoreComponent,
                    shaderStoreComponent,
                    windowComponent,
                    renderingContextComponent
                );
            }
        }
    }
    
    // Execute gui 3d model pass
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    
    if (mGuiEntityGroups.count(RenderableType::GUI_3D_MODEL))
    {
        const auto& gui3dEntities = mGuiEntityGroups.at(RenderableType::GUI_3D_MODEL);
        for (const auto& entityId : gui3dEntities)
        {
            const auto& renderableComponent = world.GetComponent<RenderableComponent>(entityId);
            const auto& transformComponent = world.GetComponent<TransformComponent>(entityId);
            
            RenderEntityInternal
            (
                transformComponent,
                renderableComponent,
                cameraComponent,
                lightStoreComponent,
                shaderStoreComponent,
                windowComponent,
                renderingContextComponent
            );
        }
    }
    
    // Swap window buffers
    SDL_GL_SwapWindow(windowComponent.mWindowHandle);
}

///-----------------------------------------------------------------------------------------------

void RenderingSystem::RenderStringInternal
(
    const TransformComponent& transformComponent,
    const RenderableComponent& renderableComponent,
    const CameraSingletonComponent& cameraComponent,
    const LightStoreSingletonComponent& lightStoreComponent,
    const ShaderStoreSingletonComponent& shaderStoreComponent,
    const TextStringComponent& textStringComponent,
    const WindowSingletonComponent& windowComponent,
    RenderingContextSingletonComponent& renderingContextComponent
) const
{
    if (!renderableComponent.mIsVisible)
    {
        return;
    }

    // Update Shader is necessary
    const resources::ShaderResource* currentShader = nullptr;
    if (renderableComponent.mShaderNameId != renderingContextComponent.previousShaderNameId)
    {
        currentShader = &shaderStoreComponent.mShaders.at(renderableComponent.mShaderNameId);
        GL_CHECK(glUseProgram(currentShader->GetProgramId()));

        renderingContextComponent.previousShaderNameId = renderableComponent.mShaderNameId;
        renderingContextComponent.previousShader       = currentShader;
    }
    else
    {
        currentShader = renderingContextComponent.previousShader;
    }

    auto currentTexture = &resources::ResourceLoadingService::GetInstance().GetResource<resources::TextureResource>(renderableComponent.mTextureResourceId);
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, currentTexture->GetGLTextureId()));
    
    auto positionCounter = transformComponent.mPosition;
    for (const auto& meshResourceId: renderableComponent.mMeshResourceIds)
    {
        auto currentMesh = &resources::ResourceLoadingService::GetInstance().GetResource<resources::MeshResource>(meshResourceId);
        GL_CHECK(glBindVertexArray(currentMesh->GetVertexArrayObject()));
        
        // Calculate world matrix for entity
        glm::mat4 world(1.0f);
           
        // Correct display of hud and billboard entities
        glm::vec3 position = positionCounter;
        positionCounter.x += textStringComponent.mPaddingProportionalToSize * textStringComponent.mCharacterSize;
        
        glm::vec3 scale    = transformComponent.mScale;
        glm::vec3 rotation = transformComponent.mRotation;
        
        if (renderableComponent.mRenderableType == RenderableType::GUI_SPRITE)
        {
            scale.x /= windowComponent.mAspectRatio;
        }
        
        glm::mat4 rotMatrix = glm::mat4_cast(math::EulerAnglesToQuat(rotation));
           
        world = glm::translate(world, position);
        world *= rotMatrix;
        world = glm::scale(world, scale);
        
        // Set mvp uniforms
        currentShader->SetMatrix4fv(WORLD_MARIX_UNIFORM_NAME, world);
        currentShader->SetMatrix4fv(VIEW_MARIX_UNIFORM_NAME, cameraComponent.mViewMatrix);
        currentShader->SetMatrix4fv(PROJECTION_MARIX_UNIFORM_NAME, cameraComponent.mProjectionMatrix);
        currentShader->SetMatrix4fv(NORMAL_MATRIX_UNIFORM_NAME, rotMatrix);
        currentShader->SetFloatVec4(MATERIAL_AMBIENT_UNIFORM_NAME, renderableComponent.mMaterial.mAmbient);
        currentShader->SetFloatVec4(MATERIAL_DIFFUSE_UNIFORM_NAME, renderableComponent.mMaterial.mDiffuse);
        currentShader->SetFloatVec4(MATERIAL_SPECULAR_UNIFORM_NAME, renderableComponent.mMaterial.mSpecular);
        currentShader->SetFloat(MATERIAL_SHININESS_UNIFORM_NAME, renderableComponent.mMaterial.mShininess);
        currentShader->SetFloatVec3Array(LIGHT_POSITIONS_UNIFORM_NAME, lightStoreComponent.mLightPositions);
        currentShader->SetFloatArray(LIGHT_POWERS_UNIFORM_NAME, lightStoreComponent.mLightPowers);
        currentShader->SetInt(IS_AFFECTED_BY_LIGHT_UNIFORM_NAME, renderableComponent.mIsAffectedByLight ? 1 : 0);
        currentShader->SetFloatVec3(EYE_POSITION_UNIFORM_NAME, cameraComponent.mPosition);
        
        // Set other matrix uniforms
        for (const auto& matrixUniformEntry: renderableComponent.mShaderUniforms.mShaderMatrixUniforms)
        {
            currentShader->SetMatrix4fv(matrixUniformEntry.first, matrixUniformEntry.second);
        }

        // Set other matrix array uniforms
        for (const auto& mat4arrayUniformEntry: renderableComponent.mShaderUniforms.mShaderMatrixArrayUniforms)
        {
            currentShader->SetMatrix4Array(mat4arrayUniformEntry.first, mat4arrayUniformEntry.second);
        }
        
        // Set other float vec4 array uniforms
        for (const auto& vec4arrayUniformEntry: renderableComponent.mShaderUniforms.mShaderFloatVec4ArrayUniforms)
        {
            currentShader->SetFloatVec4Array(vec4arrayUniformEntry.first, vec4arrayUniformEntry.second);
        }
        
        // Set other float vec3 array uniforms
        for (const auto& vec3arrayUniformEntry: renderableComponent.mShaderUniforms.mShaderFloatVec3ArrayUniforms)
        {
            currentShader->SetFloatVec3Array(vec3arrayUniformEntry.first, vec3arrayUniformEntry.second);
        }
        
        // Set other float vec4 uniforms
        for (const auto& floatVec4UniformEntry : renderableComponent.mShaderUniforms.mShaderFloatVec4Uniforms)
        {
            currentShader->SetFloatVec4(floatVec4UniformEntry.first, floatVec4UniformEntry.second);
        }
        
        // Set other float vec3 uniforms
        for (const auto& floatVec3UniformEntry : renderableComponent.mShaderUniforms.mShaderFloatVec3Uniforms)
        {
            currentShader->SetFloatVec3(floatVec3UniformEntry.first, floatVec3UniformEntry.second);
        }
        
        // Set other float uniforms
        for (const auto& floatUniformEntry : renderableComponent.mShaderUniforms.mShaderFloatUniforms)
        {
            currentShader->SetFloat(floatUniformEntry.first, floatUniformEntry.second);
        }
        
        // Set other int uniforms
        for (const auto& intUniformEntry : renderableComponent.mShaderUniforms.mShaderIntUniforms)
        {
            currentShader->SetInt(intUniformEntry.first, intUniformEntry.second);
        }
        
        // Perform draw call
        GL_CHECK(glDrawElements(GL_TRIANGLES, currentMesh->GetIndexCountPerMesh()[0], GL_UNSIGNED_SHORT, (void*)0));
    }
}

///-----------------------------------------------------------------------------------------------

void RenderingSystem::RenderEntityInternal
(    
    const TransformComponent& transformComponent,
    const RenderableComponent& renderableComponent,    
    const CameraSingletonComponent& cameraComponent,
    const LightStoreSingletonComponent& lightStoreComponent,
    const ShaderStoreSingletonComponent& shaderStoreComponent,
    const WindowSingletonComponent& windowComponent,    
    RenderingContextSingletonComponent& renderingContextComponent    
) const
{
    if (!renderableComponent.mIsVisible)
    {
        return;
    }

    // Update Shader is necessary
    const resources::ShaderResource* currentShader = nullptr;
    if (renderableComponent.mShaderNameId != renderingContextComponent.previousShaderNameId)
    {
        currentShader = &shaderStoreComponent.mShaders.at(renderableComponent.mShaderNameId);
        GL_CHECK(glUseProgram(currentShader->GetProgramId()));

        renderingContextComponent.previousShaderNameId = renderableComponent.mShaderNameId;
        renderingContextComponent.previousShader       = currentShader;
    }
    else
    {
        currentShader = renderingContextComponent.previousShader;
    }
        
    // Calculate world matrix for entity
    glm::mat4 world(1.0f);
        
    // Correct display of hud and billboard entities    
    glm::vec3 position = transformComponent.mPosition;
    glm::vec3 scale    = transformComponent.mScale;
    glm::vec3 rotation = transformComponent.mRotation;

    if (renderableComponent.mRenderableType == RenderableType::GUI_SPRITE)
    {        
        scale.x /= windowComponent.mAspectRatio;        
    }  
    
    glm::mat4 rotMatrix = glm::mat4_cast(math::EulerAnglesToQuat(rotation));
    
    world = glm::translate(world, position);
    world *= rotMatrix;
    world = glm::scale(world, scale);
       

    // Update texture if necessary
    const resources::TextureResource* currentTexture = nullptr;
    if (renderableComponent.mTextureResourceId != renderingContextComponent.previousTextureResourceId)
    {
        currentTexture = &resources::ResourceLoadingService::GetInstance().GetResource<resources::TextureResource>(renderableComponent.mTextureResourceId);
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, currentTexture->GetGLTextureId()));

        renderingContextComponent.previousTexture = currentTexture;
        renderingContextComponent.previousTextureResourceId = renderableComponent.mTextureResourceId;
    }
    else
    {
        currentTexture = renderingContextComponent.previousTexture;
    }      

    // Set mvp uniforms    
    currentShader->SetMatrix4fv(WORLD_MARIX_UNIFORM_NAME, world);
    currentShader->SetMatrix4fv(VIEW_MARIX_UNIFORM_NAME, cameraComponent.mViewMatrix);
    currentShader->SetMatrix4fv(PROJECTION_MARIX_UNIFORM_NAME, cameraComponent.mProjectionMatrix);    
    currentShader->SetMatrix4fv(NORMAL_MATRIX_UNIFORM_NAME, rotMatrix);
    currentShader->SetFloatVec4(MATERIAL_AMBIENT_UNIFORM_NAME, renderableComponent.mMaterial.mAmbient);
    currentShader->SetFloatVec4(MATERIAL_DIFFUSE_UNIFORM_NAME, renderableComponent.mMaterial.mDiffuse);
    currentShader->SetFloatVec4(MATERIAL_SPECULAR_UNIFORM_NAME, renderableComponent.mMaterial.mSpecular);
    currentShader->SetFloat(MATERIAL_SHININESS_UNIFORM_NAME, renderableComponent.mMaterial.mShininess);
    currentShader->SetFloatVec3Array(LIGHT_POSITIONS_UNIFORM_NAME, lightStoreComponent.mLightPositions);
    currentShader->SetFloatArray(LIGHT_POWERS_UNIFORM_NAME, lightStoreComponent.mLightPowers);
    currentShader->SetInt(IS_AFFECTED_BY_LIGHT_UNIFORM_NAME, renderableComponent.mIsAffectedByLight ? 1 : 0);
    currentShader->SetFloatVec3(EYE_POSITION_UNIFORM_NAME, cameraComponent.mPosition);
    
    // Set other matrix uniforms
    for (const auto& matrixUniformEntry: renderableComponent.mShaderUniforms.mShaderMatrixUniforms)
    {
        currentShader->SetMatrix4fv(matrixUniformEntry.first, matrixUniformEntry.second);
    }
    
    // Set other matrix array uniforms
    for (const auto& mat4arrayUniformEntry: renderableComponent.mShaderUniforms.mShaderMatrixArrayUniforms)
    {
        currentShader->SetMatrix4Array(mat4arrayUniformEntry.first, mat4arrayUniformEntry.second);
    }

    // Set other float vec4 array uniforms
    for (const auto& vec4arrayUniformEntry: renderableComponent.mShaderUniforms.mShaderFloatVec4ArrayUniforms)
    {
        currentShader->SetFloatVec4Array(vec4arrayUniformEntry.first, vec4arrayUniformEntry.second);
    }
    
    // Set other float vec3 array uniforms
    for (const auto& vec3arrayUniformEntry: renderableComponent.mShaderUniforms.mShaderFloatVec3ArrayUniforms)
    {
        currentShader->SetFloatVec3Array(vec3arrayUniformEntry.first, vec3arrayUniformEntry.second);
    }
    
    // Set other float vec4 uniforms
    for (const auto& floatVec4UniformEntry : renderableComponent.mShaderUniforms.mShaderFloatVec4Uniforms)
    {
        currentShader->SetFloatVec4(floatVec4UniformEntry.first, floatVec4UniformEntry.second);
    }
    
    // Set other float vec3 uniforms
    for (const auto& floatVec3UniformEntry : renderableComponent.mShaderUniforms.mShaderFloatVec3Uniforms)
    {
        currentShader->SetFloatVec3(floatVec3UniformEntry.first, floatVec3UniformEntry.second);
    }
    
    // Set other float uniforms
    for (const auto& floatUniformEntry : renderableComponent.mShaderUniforms.mShaderFloatUniforms)
    {
        currentShader->SetFloat(floatUniformEntry.first, floatUniformEntry.second);
    }
    
    // Set other int uniforms
    for (const auto& intUniformEntry : renderableComponent.mShaderUniforms.mShaderIntUniforms)
    {
        currentShader->SetInt(intUniformEntry.first, intUniformEntry.second);
    }
    
    // Update current mesh if necessary
    const resources::MeshResource* currentMesh = nullptr;
    if (renderableComponent.mMeshResourceIds[renderableComponent.mCurrentMeshResourceIndex] != renderingContextComponent.previousMeshResourceId)
    {
        currentMesh = &resources::ResourceLoadingService::GetInstance().GetResource<resources::MeshResource>(renderableComponent.mMeshResourceIds[renderableComponent.mCurrentMeshResourceIndex]);
        GL_CHECK(glBindVertexArray(currentMesh->GetVertexArrayObject()));

        renderingContextComponent.previousMesh           = currentMesh;
        renderingContextComponent.previousMeshResourceId = renderableComponent.mMeshResourceIds[renderableComponent.mCurrentMeshResourceIndex];
    }
    else
    {
        currentMesh = renderingContextComponent.previousMesh;
    }

    // Perform draw call
    const auto& indexCountPerMesh = currentMesh->GetIndexCountPerMesh();
    const auto& baseIndexPerMesh = currentMesh->GetBaseIndexPerMesh();
    const auto& baseVertexPerMesh = currentMesh->GetBaseVertexPerMesh();
    for (auto i = 0U; i < indexCountPerMesh.size(); ++i)
    {
        if (indexCountPerMesh[i] > 0)
        {
            GL_CHECK(glDrawElementsBaseVertex(GL_TRIANGLES, indexCountPerMesh.at(i), GL_UNSIGNED_SHORT, (void*)(sizeof(unsigned short) * baseIndexPerMesh.at(i)), baseVertexPerMesh.at(i)));
        }
    }
}

///-----------------------------------------------------------------------------------------------

void RenderingSystem::InitializeCamera() const
{        
    ecs::World::GetInstance().SetSingletonComponent<CameraSingletonComponent>(std::make_unique<CameraSingletonComponent>());
}

///-----------------------------------------------------------------------------------------------

void RenderingSystem::InitializeLights() const
{
    ecs::World::GetInstance().SetSingletonComponent<LightStoreSingletonComponent>(std::make_unique<LightStoreSingletonComponent>());
}

///-----------------------------------------------------------------------------------------------

void RenderingSystem::CompileAndLoadShaders() const
{
    auto& renderingContextComponent = ecs::World::GetInstance().GetSingletonComponent<RenderingContextSingletonComponent>();
    
    // Bind default VAO for correct shader compilation
    GL_CHECK(glGenVertexArrays(1, &renderingContextComponent.mDefaultVertexArrayObject));
    GL_CHECK(glBindVertexArray(renderingContextComponent.mDefaultVertexArrayObject));
    
    const auto shaderNames    = GetAndFilterShaderNames();
    auto shaderStoreComponent = std::make_unique<ShaderStoreSingletonComponent>();
    
    for (const auto& shaderName: shaderNames)
    {
        // By signaling to load either a .vs or a .fs, the ShaderLoader will load the pair automatically,
        // hence why the addition of the .vs here
        auto shaderResourceId = resources::ResourceLoadingService::GetInstance().LoadResource(resources::ResourceLoadingService::RES_SHADERS_ROOT + shaderName + ".vs");
        auto& shaderResource  = resources::ResourceLoadingService::GetInstance().GetResource<resources::ShaderResource>(shaderResourceId);
        
        // Save a copy of the shader to the ShaderStoreComponent
        shaderStoreComponent->mShaders[StringId(shaderName)] = shaderResource;
        
        // And unload the resource
        resources::ResourceLoadingService::GetInstance().UnloadResource(shaderResourceId);
    }
    
    // Unbind any VAO currently bound
    GL_CHECK(glBindVertexArray(0));
    
    ecs::World::GetInstance().SetSingletonComponent<ShaderStoreSingletonComponent>(std::move(shaderStoreComponent));
}

///-----------------------------------------------------------------------------------------------

std::set<std::string> RenderingSystem::GetAndFilterShaderNames() const
{
    const auto vertexAndFragmentShaderFilenames = GetAllFilenamesInDirectory(resources::ResourceLoadingService::RES_SHADERS_ROOT);

    std::set<std::string> shaderNames;
    for (const auto& shaderFilename : vertexAndFragmentShaderFilenames)
    {
        shaderNames.insert(GetFileNameWithoutExtension(shaderFilename));
    }
    return shaderNames;
}

///-----------------------------------------------------------------------------------------------

}

}

