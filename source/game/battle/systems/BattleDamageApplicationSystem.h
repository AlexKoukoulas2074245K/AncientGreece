///------------------------------------------------------------------------------------------------
///  BattleDamageApplicationSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 07/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef BattleDamageApplicationSystem_h
#define BattleDamageApplicationSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

class UnitStatsComponent;

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------

class BattleDamageComponent;

///-----------------------------------------------------------------------------------------------
class BattleDamageApplicationSystem final : public genesis::ecs::BaseSystem<UnitStatsComponent, BattleDamageComponent>
{
public:
    BattleDamageApplicationSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;

private:
    void AddBloodDropsToUnit(const genesis::ecs::EntityId unitEntityId) const;
    
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* BattleDamageApplicationSystem_h */
