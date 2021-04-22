///------------------------------------------------------------------------------------------------
///  ParticleUtils.h
///  Genesis
///
///  Created by Alex Koukoulas on 22/04/2021.
///------------------------------------------------------------------------------------------------

#ifndef ParticleUtils_h
#define ParticleUtils_h

///------------------------------------------------------------------------------------------------

#include "../components/ParticleEmitterComponent.h"
#include "../../common/utils/MathUtils.h"
#include "../../ECS.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

class TransformComponent;

///------------------------------------------------------------------------------------------------

namespace rendering
{

///------------------------------------------------------------------------------------------------
/// Spawns a particle in an emitter component based on the emitter's parameters
///
/// @param[in] particleIndex the index at which the particle will be spawned
/// @param[in] transformComponent the emitter's transform component
/// @param[inout] emitterComponent the emitter component to which the newly created particle will be spawned in based on
/// the emitter's parameters
void SpawnParticleAtIndex
(
    const size_t index,
    const TransformComponent& transformComponent,
    ParticleEmitterComponent& emitterComponent
);

///------------------------------------------------------------------------------------------------
/// Adds a particle emitter to the world.
///
/// @param[in] emitterType the type of the emitter that modifies its update flow.
/// @param[in] particleTextureName the texture all particles will share.
/// @param[in] emitterOriginPosition the origin position where all particles will spawn from.
/// @param[in] particleLifetimeRange an inclusive range of randomness that each particle's lifetime will fall on in the form of [minLifeTime, maxLifeTime] in seconds.
/// @param[in] particlePositionXOffsetRange an inclusive range of randomness that each particle's offset x position will fall on in the form of [minX, maxX] RELATIVE to the emitter's origin position.
/// @param[in] particlePositionYOffsetRange an inclusive range of randomness that each particle's offset y position will fall on in the form of [minY, maxY] RELATIVE to the emitter's origin position.
/// @param[in] particleCount the number of particles the emitter can hold at all times
/// @param[in] particleSize the size of all particles
/// @return the entity holding the newly created particle emitter component
genesis::ecs::EntityId AddParticleEmitter
(
    const ParticleEmitterType emitterType,
    const std::string& particleTextureName,
    const glm::vec3& emitterOriginPosition,
    const glm::vec2& particleLifetimeRange,
    const glm::vec2& particlePositionXOffsetRange,
    const glm::vec2& particlePositionYOffsetRange,
    const size_t particleCount,
    const float particleSize
);

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* ParticleUtils_h */
