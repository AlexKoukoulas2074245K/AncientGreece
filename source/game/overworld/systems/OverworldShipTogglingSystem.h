///------------------------------------------------------------------------------------------------
///  OverworldShipTogglingSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 27/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldShipTogglingSystem_h
#define OverworldShipTogglingSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

class UnitStatsComponent;

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------
class OverworldShipTogglingSystem final : public genesis::ecs::BaseSystem<UnitStatsComponent>
{
public:
    OverworldShipTogglingSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const override;
    
private:
    void CreateSmokeRevealParticleEffectForEntity(const genesis::ecs::EntityId entityId) const;
    
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldShipTogglingSystem_h */
