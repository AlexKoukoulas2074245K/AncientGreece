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

class OverworldBattleStateComponent;

///-----------------------------------------------------------------------------------------------
class OverworldBattleProcessingSystem final : public genesis::ecs::BaseSystem<OverworldBattleStateComponent>
{
public:
    OverworldBattleProcessingSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;
    
private:
    void UpdateOverworldBattles(const float dt, const std::vector<genesis::ecs::EntityId>&) const;
    void CheckForLiveBattle() const;
    std::vector<UnitStats> PrepareBattleParty(const genesis::ecs::EntityId partyLeaderEntityId) const;
    
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldBattleProcessingSystem_h */
