///------------------------------------------------------------------------------------------------
///  ParticleEmitterComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 22/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef ParticleEmitterComponent_h
#define ParticleEmitterComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../common/utils/MathUtils.h"

#include <vector>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------

using GLuint = unsigned int;

///-----------------------------------------------------------------------------------------------

enum class ParticleEmitterType
{
    SMOKE,
    SMOKE_REVEAL,
    BLOOD_DROP
};

///-----------------------------------------------------------------------------------------------

class ParticleEmitterComponent final: public ecs::IComponent
{
public:
    glm::vec3 mEmitterOriginPosition;
    glm::vec3 mEmitterAttachementOffsetPosition;
    std::vector<glm::vec3> mParticlePositions;
    std::vector<glm::vec3> mParticleDirections;
    std::vector<float> mParticleLifetimes;
    std::vector<float> mParticleSizes;
    
    glm::vec2 mParticleLifetimeRange;
    glm::vec2 mParticlePositionXOffsetRange;
    glm::vec2 mParticlePositionYOffsetRange;
    glm::vec2 mParticlePositionZOffsetRange;
    glm::vec2 mParticleSizeRange;
    
    size_t mParticleTextureResourceId;
    GLuint mParticleVertexArrayObject;
    GLuint mParticleVertexBuffer;
    GLuint mParticleUVBuffer;
    GLuint mParticlePositionsBuffer;
    GLuint mParticleLifetimesBuffer;
    GLuint mParticleSizesBuffer;
    
    StringId mShaderNameId;
    
    ParticleEmitterType mEmitterType;
    
    bool mAttachedToEntity;
};

///-----------------------------------------------------------------------------------------------

}

}

#endif /* ParticleEmitterComponent_h */
