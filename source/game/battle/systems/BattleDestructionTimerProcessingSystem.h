///------------------------------------------------------------------------------------------------
///  BattleDestructionTimerProcessingSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 06/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef BattleDestructionTimerProcessingSystem_h
#define BattleDestructionTimerProcessingSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------

class DestructionTimerComponent;

///-----------------------------------------------------------------------------------------------
class BattleDestructionTimerProcessingSystem final : public genesis::ecs::BaseSystem<DestructionTimerComponent>
{
public:
    BattleDestructionTimerProcessingSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* BattleDestructionTimerProcessingSystem_h */
