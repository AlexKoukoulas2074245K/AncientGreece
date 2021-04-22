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

//static constexpr int PARTICLE_COUNT = 1000;

///-----------------------------------------------------------------------------------------------

enum class ParticleEmitterType
{
    SMOKE
};

///-----------------------------------------------------------------------------------------------

class ParticleEmitterComponent final: public ecs::IComponent
{
public:
    glm::vec3 mEmitterOriginPosition;
    std::vector<glm::vec3> mParticlePositions;
    std::vector<float> mParticleLifetimes;
    
    glm::vec2 mParticleLifetimeRange;
    glm::vec2 mParticlePositionXOffsetRange;
    glm::vec2 mParticlePositionYOffsetRange;
    
    GLuint mParticleVertexArrayObject;
    GLuint mParticleVertexBuffer;
    GLuint mParticleUVBuffer;
    GLuint mParticlePositionsBuffer;
    GLuint mParticleLifetimesBuffer;
    
    ParticleEmitterType mEmitterType;
};

///-----------------------------------------------------------------------------------------------

}

}

#endif /* ParticleEmitterComponent_h */
