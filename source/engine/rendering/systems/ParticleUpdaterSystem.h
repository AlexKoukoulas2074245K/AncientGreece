///------------------------------------------------------------------------------------------------
///  ParticleUpdaterSystem.h
///  Genesis
///
///  Created by Alex Koukoulas on 22/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef ParticleUpdaterSystem_h
#define ParticleUpdaterSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------

class ParticleEmitterComponent;

///-----------------------------------------------------------------------------------------------

class ParticleUpdaterSystem final: public ecs::BaseSystem<ParticleEmitterComponent>
{
public:
    ParticleUpdaterSystem();
    
    void VUpdate(const float dt, const std::vector<ecs::EntityId>&) const override;

private:
    void SortParticles(ParticleEmitterComponent&) const;
    
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* ParticleUpdaterSystem_h */
