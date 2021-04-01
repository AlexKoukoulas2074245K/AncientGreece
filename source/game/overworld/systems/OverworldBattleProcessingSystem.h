///------------------------------------------------------------------------------------------------
///  OverworldBattleProcessingSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldBattleProcessingSystem_h
#define OverworldBattleProcessingSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

struct UnitStats;

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------
class OverworldBattleProcessingSystem final : public genesis::ecs::BaseSystem<genesis::ecs::NullComponent>
{
public:
    OverworldBattleProcessingSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;
    
private:
    void PrepareLiveBattle() const;
    std::vector<UnitStats> PrepareBattleParty(const genesis::ecs::EntityId partyLeaderEntityId) const;
    
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldBattleProcessingSystem_h */
