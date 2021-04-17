///------------------------------------------------------------------------------------------------
///  BattleEndHandlingSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 15/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef BattleEndHandlingSystem_h
#define BattleEndHandlingSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

class UnitStatsComponent;

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------
class BattleEndHandlingSystem final : public genesis::ecs::BaseSystem<UnitStatsComponent>
{
public:
    BattleEndHandlingSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;
    
private:
    void PrepareAndShowResultsView() const;
    void ModifyPartiesByCasualties(const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* BattleEndHandlingSystem_h */
