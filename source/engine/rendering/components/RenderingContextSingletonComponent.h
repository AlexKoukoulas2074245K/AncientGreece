///------------------------------------------------------------------------------------------------
///  RenderingContextSingletonComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef RenderingContextSingletonComponent_h
#define RenderingContextSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../common/utils/MathUtils.h"
#include "../../common/utils/StringUtils.h"
#include "../../resources/MeshResource.h"
#include "../../resources/ResourceLoadingService.h"
#include "../../resources/ShaderResource.h"
#include "../../resources/TextureResource.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------

using SDL_GLContext = void*;
using GLuint        = unsigned int;

///-----------------------------------------------------------------------------------------------

class RenderingContextSingletonComponent final : public ecs::IComponent
{
public:        
    // Core state
    SDL_GLContext mGLContext          = nullptr;
    GLuint mDefaultVertexArrayObject  = 0;
    GLuint mDepthMapFrameBufferObject = 0;
    GLuint mShadowMapTexture          = 0;
    glm::vec4 mClearColor             = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
    float mDtAccumulator              = 0.0f;
    bool mShadowsEnabled              = true;
    bool mParticlesEnabled            = true;

    // Previous render call resource ids
    StringId previousShaderNameId             = StringId();
    resources::ResourceId previousTextureResourceId = resources::ResourceId();
    resources::ResourceId previousMeshResourceId    = resources::ResourceId();
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* RenderingContextSingletonComponent_h */
