///------------------------------------------------------------------------------------------------
///  ParticleUpdaterSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#include "ParticleUpdaterSystem.h"
#include "../components/ParticleEmitterComponent.h"
#include "../utils/ParticleUtils.h"
#include "../../common/components/TransformComponent.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------

ParticleUpdaterSystem::ParticleUpdaterSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void ParticleUpdaterSystem::VUpdate(const float dt, const std::vector<ecs::EntityId>& entitiesToProcess) const
{
    auto& world = genesis::ecs::World::GetInstance();
    for (const auto& entityId: entitiesToProcess)
    {
        const auto& emitterTransformComponent = world.GetComponent<TransformComponent>(entityId);
        auto& particleEmitterComponent = world.GetComponent<ParticleEmitterComponent>(entityId);
        
        switch (particleEmitterComponent.mEmitterType)
        {
            case ParticleEmitterType::SMOKE:
            {
                for (size_t i = 0U; i < particleEmitterComponent.mParticlePositions.size(); ++i)
                {
                    // subtract from the particles lifetime
                    particleEmitterComponent.mParticleLifetimes[i] -= dt;

                    // if the lifetime is below 0 respawn the particle
                    if (particleEmitterComponent.mParticleLifetimes[i] <= 0.0f )
                    {
                        SpawnParticleAtIndex(static_cast<size_t>(i), emitterTransformComponent, particleEmitterComponent);
                    }

                    // move the particle down depending on the delta time
                    particleEmitterComponent.mParticlePositions[i] += glm::vec3(0.0f, 0.0f, -dt/20.0f);
                }
            } break;
        }
        
        SortParticles(particleEmitterComponent);
    }
}

///-----------------------------------------------------------------------------------------------

void ParticleUpdaterSystem::SortParticles(ParticleEmitterComponent& particleEmitterComponent) const
{
    // Create permutation index vector for final positions
    const auto particleCount = particleEmitterComponent.mParticlePositions.size();
    
    std::vector<std::size_t> indexVec(particleCount);
    std::iota(indexVec.begin(), indexVec.end(), 0);
    std::sort(indexVec.begin(), indexVec.end(), [&](const size_t i, const size_t j)
    {
        return particleEmitterComponent.mParticlePositions[i].z > particleEmitterComponent.mParticlePositions[j].z;
    });
    
    // Create corrected vectors
    std::vector<glm::vec3> correctedPositions(particleCount);
    std::vector<float> correctedLifetimes(particleCount);
    for (size_t i = 0U; i < particleCount; ++i)
    {
        correctedPositions[i] = particleEmitterComponent.mParticlePositions[indexVec[i]];
        correctedLifetimes[i] = particleEmitterComponent.mParticleLifetimes[indexVec[i]];
    }
    
    particleEmitterComponent.mParticlePositions = std::move(correctedPositions);
    particleEmitterComponent.mParticleLifetimes = std::move(correctedLifetimes);
}

///-----------------------------------------------------------------------------------------------

}

}
