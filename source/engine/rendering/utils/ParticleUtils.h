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
/// @param[inout] emitterComponent the emitter component to which the newly created particle will be spawned in based on
/// the emitter's parameters
void SpawnParticleAtIndex
(
    const size_t index,
    ParticleEmitterComponent& emitterComponent
);


///------------------------------------------------------------------------------------------------
/// Spawns a number of particles in an emitter component at the next available slots
///
/// @param[in] particlesToSpawnCount the index at which the particle will be spawned
/// @param[in] transformComponent the emitter's transform component
/// @param[inout] emitterComponent the emitter component to which the newly created particle will be spawned in based on
/// the emitter's parameters
void SpawnParticlesAtFirstAvailableSlot
(
    const int particlesToSpawnCount,
    ParticleEmitterComponent& emitterComponent
);

///------------------------------------------------------------------------------------------------
/// Adds a particle emitter to the world.
///
/// @param[in] emitterType the type of the emitter that modifies its update flow.
/// @param[in] emitterOriginPosition the origin position where all particles will spawn from.
/// @param[in] particleLifetimeRange an inclusive range of randomness that each particle's lifetime will fall on in the form of [minLifeTime, maxLifeTime] in seconds.
/// @param[in] particlePositionXOffsetRange an inclusive range of randomness that each particle's offset x position will fall on in the form of [minX, maxX] RELATIVE to the emitter's origin position.
/// @param[in] particlePositionYOffsetRange an inclusive range of randomness that each particle's offset y position will fall on in the form of [minY, maxY] RELATIVE to the emitter's origin position.
/// @param[in] particleSizeRange an inclusive range of randomness that each particle's size will fall on in the form of [minSize, maxSize] 
/// @param[in] particleCount the number of particles the emitter can hold at all times
/// @param[in] preFillParticles specifies whether or not the emitter will have particles initialised initially
/// @param[in] parentEntityIdToAttachTo (optional) specifies whether or not the emitter will be attached to an existing entity

genesis::ecs::EntityId AddParticleEmitter
(
    const ParticleEmitterType emitterType,
    const glm::vec3& emitterOriginPosition,
    const glm::vec2& particleLifetimeRange,
    const glm::vec2& particlePositionXOffsetRange,
    const glm::vec2& particlePositionYOffsetRange,
    const glm::vec2& particleSizeRange,
    const size_t particleCount,
    const bool preFillParticles,
    const ecs::EntityId parentEntityIdToAttachTo = ecs::NULL_ENTITY_ID
);

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* ParticleUtils_h */
