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

    static const std::vector<float> PARTICLE_VERTEX_POSITIONS =
    {
        0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f
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
        { ParticleEmitterType::SMOKE, StringId("particle_smoke") }
    };
}

///------------------------------------------------------------------------------------------------

void SpawnParticleAtIndex
(
    const size_t index,
    const TransformComponent& transformComponent,
    ParticleEmitterComponent& emitterComponent
)
{
    const auto lifeTime = genesis::math::RandomFloat(emitterComponent.mParticleLifetimeRange.s, emitterComponent.mParticleLifetimeRange.t);
    const auto xOffset = genesis::math::RandomFloat(emitterComponent.mParticlePositionXOffsetRange.s, emitterComponent.mParticlePositionXOffsetRange.t);
    const auto yOffset = genesis::math::RandomFloat(emitterComponent.mParticlePositionYOffsetRange.s, emitterComponent.mParticlePositionYOffsetRange.t);
    
    emitterComponent.mParticleLifetimes[index] = lifeTime;
    emitterComponent.mParticlePositions[index] = transformComponent.mPosition;
    emitterComponent.mParticlePositions[index].x += xOffset;
    emitterComponent.mParticlePositions[index].y += yOffset;
}

///------------------------------------------------------------------------------------------------

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
)
{
    auto transformComponent = std::make_unique<TransformComponent>();
    transformComponent->mPosition = emitterOriginPosition;
    transformComponent->mScale = glm::vec3(particleSize, particleSize, particleSize);
    
    auto emitterComponent = std::make_unique<ParticleEmitterComponent>();
    emitterComponent->mParticlePositions.resize(particleCount);
    emitterComponent->mParticleLifetimes.resize(particleCount);
    emitterComponent->mEmitterType = emitterType;
    emitterComponent->mEmitterOriginPosition = emitterOriginPosition;
    emitterComponent->mParticleLifetimeRange = particleLifetimeRange;
    emitterComponent->mParticlePositionXOffsetRange = particlePositionXOffsetRange;
    emitterComponent->mParticlePositionYOffsetRange = particlePositionYOffsetRange;
    
    for (size_t i = 0U; i < particleCount; ++i)
    {
        SpawnParticleAtIndex(i, *transformComponent, *emitterComponent);
    }
 
    GL_CHECK(glGenVertexArrays(1, &emitterComponent->mParticleVertexArrayObject));
    GL_CHECK(glGenBuffers(1, &emitterComponent->mParticleVertexBuffer));
    GL_CHECK(glGenBuffers(1, &emitterComponent->mParticleUVBuffer));
    GL_CHECK(glGenBuffers(1, &emitterComponent->mParticlePositionsBuffer));
    GL_CHECK(glGenBuffers(1, &emitterComponent->mParticleLifetimesBuffer));
    
    GL_CHECK(glBindVertexArray(emitterComponent->mParticleVertexArrayObject));
    
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, emitterComponent->mParticleVertexBuffer));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, PARTICLE_VERTEX_POSITIONS.size() * sizeof(float) , PARTICLE_VERTEX_POSITIONS.data(), GL_STATIC_DRAW));
    
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, emitterComponent->mParticleUVBuffer));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, PARTICLE_UVS.size() * sizeof(float) , PARTICLE_UVS.data(), GL_STATIC_DRAW));

    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, emitterComponent->mParticlePositionsBuffer));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, particleCount * sizeof(glm::vec3), emitterComponent->mParticlePositions.data(), GL_DYNAMIC_DRAW));
    
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, emitterComponent->mParticleLifetimesBuffer));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, particleCount * sizeof(float), emitterComponent->mParticleLifetimes.data(), GL_DYNAMIC_DRAW));
    
    GL_CHECK(glBindVertexArray(emitterComponent->mParticleVertexArrayObject));
    
    auto renderableComponent = std::make_unique<RenderableComponent>();
    renderableComponent->mTextureResourceId = genesis::resources::ResourceLoadingService::GetInstance().LoadResource(genesis::resources::ResourceLoadingService::RES_TEXTURES_ROOT + particleTextureName + ".png");
    renderableComponent->mShaderNameId = PARTICLE_TYPE_TO_SHADER_NAME.at(emitterType);
    
    auto& world = ecs::World::GetInstance();
    
    auto entity = world.CreateEntity();
    world.AddComponent<ParticleEmitterComponent>(entity, std::move(emitterComponent));
    world.AddComponent<TransformComponent>(entity, std::move(transformComponent));
    world.AddComponent<RenderableComponent>(entity, std::move(renderableComponent));
    
    return entity;
}

///------------------------------------------------------------------------------------------------

}

}
