///------------------------------------------------------------------------------------------------
///  RenderingSystem.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef RenderingSystem_h
#define RenderingSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../common/utils/StringUtils.h"
#include "../../common/utils/MathUtils.h"
#include "../../ECS.h"

#include <set>
#include <string>
#include <unordered_set>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

class TransformComponent;

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------

class CameraSingletonComponent;
class HeightMapComponent;
class LightStoreSingletonComponent;
class RenderableComponent;
class RenderingContextSingletonComponent;
class TextStringComponent;
class ShaderStoreSingletonComponent;
class WindowSingletonComponent;

///-----------------------------------------------------------------------------------------------

class RenderingSystem final: public ecs::BaseSystem<TransformComponent, RenderableComponent>
{
public:
    RenderingSystem();
    
    void VUpdate(const float dt, const std::vector<ecs::EntityId>&) const override;

private:
    void DepthRenderingPass(const std::vector<ecs::EntityId>& applicableEntities) const;
    void FinalRenderingPass(const std::vector<ecs::EntityId>& applicableEntities) const;
    
    void RenderHeightMapInternal
    (
        const TransformComponent& entityTransformComponent,
        const RenderableComponent& entityRenderableComponent,
        const HeightMapComponent& entityHeightMapComponent,
        const CameraSingletonComponent& globalCameraComponent,
        const LightStoreSingletonComponent& lightStoreComponent,
        const ShaderStoreSingletonComponent& globalShaderStoreComponent,
        const WindowSingletonComponent& globalWindowComponent,
        RenderingContextSingletonComponent& renderingContextComponent
    ) const;
    
    void RenderStringInternal
    (
        const TransformComponent& entityTransformComponent,
        const RenderableComponent& entityRenderableComponent,
        const CameraSingletonComponent& globalCameraComponent,
        const LightStoreSingletonComponent& lightStoreComponent,
        const ShaderStoreSingletonComponent& globalShaderStoreComponent,
        const TextStringComponent& textStringComponent,
        const WindowSingletonComponent& globalWindowComponent,
        RenderingContextSingletonComponent& renderingContextComponent
    ) const;
    
    void RenderEntityInternal
    (        
        const TransformComponent& entityTransformComponent,
        const RenderableComponent& entityRenderableComponent,        
        const CameraSingletonComponent& globalCameraComponent,
        const LightStoreSingletonComponent& lightStoreComponent,
        const ShaderStoreSingletonComponent& globalShaderStoreComponent,
        const WindowSingletonComponent& globalWindowComponent,        
        RenderingContextSingletonComponent& renderingContextComponent        
    ) const;
    
    void InitializeCamera() const;
    void InitializeLights() const;
    void InitializeShadowMapTexture() const;
    void InitializeFrameBuffers() const;
    void CompileAndLoadShaders() const;

    std::set<std::string> GetAndFilterShaderNames() const;

};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* RenderingSystem_h */
