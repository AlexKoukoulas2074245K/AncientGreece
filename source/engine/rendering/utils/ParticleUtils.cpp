///------------------------------------------------------------------------------------------------
///  ParticleUtils.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 22/04/2021.
///------------------------------------------------------------------------------------------------

#include "ParticleUtils.h"
#include "../components/RenderableComponent.h"
#include "../opengl/Context.h"
#include "../../common/components/TransformComponent.h"
#include "../../resources/ResourceLoadingService.h"

#include <tsl/robin_map.h>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace rendering
{

///------------------------------------------------------------------------------------------------

namespace
{

    static const std::vector<std::vector<float>> PARTICLE_VERTEX_POSITIONS =
    {
        {
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f
        },
        
        {
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f
        }
    };
    

    static const std::vector<float> PARTICLE_UVS =
    {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
    };

    static const tsl::robin_map<ParticleEmitterType, StringId> PARTICLE_TYPE_TO_SHADER_NAME =
    {
        { ParticleEmitterType::SMOKE, StringId("particle_smoke") },
        { ParticleEmitterType::BLOOD_DROP, StringId("particle_blood_drop") },
        { ParticleEmitterType::SMOKE_REVEAL, StringId("particle_smoke_reveal") }
    };

    static const tsl::robin_map<ParticleEmitterType, StringId> PARTICLE_TYPE_TO_TEXTURE_NAME =
    {
        { ParticleEmitterType::SMOKE, StringId("smoke") },
        { ParticleEmitterType::BLOOD_DROP, StringId("blood_drop") },
        { ParticleEmitterType::SMOKE_REVEAL, StringId("smoke") }
    };

    static const tsl::robin_map<ParticleEmitterType, size_t> PARTICLE_TYPE_TO_VERTEX_POSITIONS =
    {
        { ParticleEmitterType::SMOKE, 0U },
        { ParticleEmitterType::BLOOD_DROP, 0U },
        { ParticleEmitterType::SMOKE_REVEAL, 1U }
    };
}

///------------------------------------------------------------------------------------------------

void SpawnParticleAtIndex
(
    const size_t index,
    ParticleEmitterComponent& emitterComponent
)
{
    const auto lifeTime = genesis::math::RandomFloat(emitterComponent.mParticleLifetimeRange.s, emitterComponent.mParticleLifetimeRange.t);
    const auto xOffset = genesis::math::RandomFloat(emitterComponent.mParticlePositionXOffsetRange.s, emitterComponent.mParticlePositionXOffsetRange.t);
    const auto yOffset = genesis::math::RandomFloat(emitterComponent.mParticlePositionYOffsetRange.s, emitterComponent.mParticlePositionYOffsetRange.t);
    const auto zOffset = genesis::math::RandomFloat(emitterComponent.mParticlePositionZOffsetRange.s, emitterComponent.mParticlePositionZOffsetRange.t);
    const auto size = genesis::math::RandomFloat(emitterComponent.mParticleSizeRange.s, emitterComponent.mParticleSizeRange.t);
    
    emitterComponent.mParticleLifetimes[index] = lifeTime;
    emitterComponent.mParticlePositions[index] = emitterComponent.mEmitterOriginPosition;
    emitterComponent.mParticlePositions[index].x += xOffset;
    emitterComponent.mParticlePositions[index].y += yOffset;
    emitterComponent.mParticlePositions[index].z += zOffset;
    emitterComponent.mParticleDirections[index] = glm::normalize(glm::vec3(xOffset, yOffset, 0.0f));
    emitterComponent.mParticleSizes[index] = size;
}

///------------------------------------------------------------------------------------------------

void SpawnParticlesAtFirstAvailableSlot
(
    const int particlesToSpawnCount,
    ParticleEmitterComponent& emitterComponent
)
{
    auto particlesToSpawn = particlesToSpawnCount;
    auto particleCount = emitterComponent.mParticlePositions.size();
    
    for (size_t i = 0; i < particleCount && particlesToSpawn > 0; ++i)
    {
        if (emitterComponent.mParticleLifetimes[i] <= 0.0f)
        {
            SpawnParticleAtIndex(i, emitterComponent);
            particlesToSpawn--;
        }
    }
}

///------------------------------------------------------------------------------------------------

genesis::ecs::EntityId AddParticleEmitter
(
    const ParticleEmitterType emitterType,
    const glm::vec3& emitterOriginPosition,
    const glm::vec2& particleLifetimeRange,
    const glm::vec2& particlePositionXOffsetRange,
    const glm::vec2& particlePositionYOffsetRange,
    const glm::vec2& particlePositionZOffsetRange,
    const glm::vec2& particleSizeRange,
    const size_t particleCount,
    const bool preFillParticles,
    const ecs::EntityId parentEntityIdToAttachTo /* ecs::NULL_ENTITY_ID */
)
{
    auto& world = ecs::World::GetInstance();
    
    auto emitterComponent = std::make_unique<ParticleEmitterComponent>();
    emitterComponent->mParticlePositions.resize(particleCount);
    emitterComponent->mParticleDirections.resize(particleCount);
    emitterComponent->mParticleLifetimes.resize(particleCount);
    emitterComponent->mParticleSizes.resize(particleCount);
    emitterComponent->mEmitterType = emitterType;
    emitterComponent->mEmitterOriginPosition = emitterOriginPosition;
    emitterComponent->mParticleLifetimeRange = particleLifetimeRange;
    emitterComponent->mParticlePositionXOffsetRange = particlePositionXOffsetRange;
    emitterComponent->mParticlePositionYOffsetRange = particlePositionYOffsetRange;
    emitterComponent->mParticlePositionZOffsetRange = particlePositionZOffsetRange;
    emitterComponent->mParticleSizeRange = particleSizeRange;
    emitterComponent->mParticleTextureResourceId = genesis::resources::ResourceLoadingService::GetInstance().LoadResource(genesis::resources::ResourceLoadingService::RES_TEXTURES_ROOT + PARTICLE_TYPE_TO_TEXTURE_NAME.at(emitterType).GetString() + ".png");
    emitterComponent->mShaderNameId = PARTICLE_TYPE_TO_SHADER_NAME.at(emitterType);
    emitterComponent->mAttachedToEntity = parentEntityIdToAttachTo != genesis::ecs::NULL_ENTITY_ID;
    
    if (emitterComponent->mAttachedToEntity)
    {
        emitterComponent->mEmitterAttachementOffsetPosition = emitterOriginPosition - world.GetComponent<TransformComponent>(parentEntityIdToAttachTo).mPosition;
    }
    
    for (size_t i = 0U; i < particleCount; ++i)
    {
        emitterComponent->mParticleLifetimes[i] = 0.0f;
        if (preFillParticles)
        {
            SpawnParticleAtIndex(i, *emitterComponent);
        }
    }
 
    GL_CHECK(glGenVertexArrays(1, &emitterComponent->mParticleVertexArrayObject));
    GL_CHECK(glGenBuffers(1, &emitterComponent->mParticleVertexBuffer));
    GL_CHECK(glGenBuffers(1, &emitterComponent->mParticleUVBuffer));
    GL_CHECK(glGenBuffers(1, &emitterComponent->mParticlePositionsBuffer));
    GL_CHECK(glGenBuffers(1, &emitterComponent->mParticleLifetimesBuffer));
    GL_CHECK(glGenBuffers(1, &emitterComponent->mParticleSizesBuffer));
    
    GL_CHECK(glBindVertexArray(emitterComponent->mParticleVertexArrayObject));
    
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, emitterComponent->mParticleVertexBuffer));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, PARTICLE_VERTEX_POSITIONS[PARTICLE_TYPE_TO_VERTEX_POSITIONS.at(emitterType)].size() * sizeof(float) , PARTICLE_VERTEX_POSITIONS[PARTICLE_TYPE_TO_VERTEX_POSITIONS.at(emitterType)].data(), GL_STATIC_DRAW));
    
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, emitterComponent->mParticleUVBuffer));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, PARTICLE_UVS.size() * sizeof(float) , PARTICLE_UVS.data(), GL_STATIC_DRAW));

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, emitterComponent->mParticlePositionsBuffer));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, particleCount * sizeof(glm::vec3), emitterComponent->mParticlePositions.data(), GL_DYNAMIC_DRAW));
    
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, emitterComponent->mParticleLifetimesBuffer));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, particleCount * sizeof(float), emitterComponent->mParticleLifetimes.data(), GL_DYNAMIC_DRAW));
    
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, emitterComponent->mParticleSizesBuffer));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, particleCount * sizeof(float), emitterComponent->mParticleSizes.data(), GL_DYNAMIC_DRAW));
    
    GL_CHECK(glBindVertexArray(emitterComponent->mParticleVertexArrayObject));
    
    auto entity = parentEntityIdToAttachTo;
    if (entity == genesis::ecs::NULL_ENTITY_ID)
    {
        entity = world.CreateEntity();
        world.AddComponent<TransformComponent>(entity, std::make_unique<TransformComponent>());
        world.AddComponent<RenderableComponent>(entity, std::make_unique<RenderableComponent>());
    }
    
    world.AddComponent<ParticleEmitterComponent>(entity, std::move(emitterComponent));
    
    return entity;
}

///------------------------------------------------------------------------------------------------

}

}
