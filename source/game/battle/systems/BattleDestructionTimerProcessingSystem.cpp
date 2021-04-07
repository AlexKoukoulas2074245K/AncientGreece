///------------------------------------------------------------------------------------------------
///  BattleDestructionTimerProcessingSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 06/04/2021.
///-----------------------------------------------------------------------------------------------

#include "BattleDestructionTimerProcessingSystem.h"
#include "../components/BattleDestructionTimerComponent.h"

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------

BattleDestructionTimerProcessingSystem::BattleDestructionTimerProcessingSystem()
    : BaseSystem()
{
}

///-----------------------------------------------------------------------------------------------

void BattleDestructionTimerProcessingSystem::VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    auto& world = genesis::ecs::World::GetInstance();
    for (const auto& entityId: entitiesToProcess)
    {
        auto& destructionTimerComponent = world.GetComponent<BattleDestructionTimerComponent>(entityId);
        destructionTimerComponent.mDestructionTimer -= dt;
        if (destructionTimerComponent.mDestructionTimer <= 0.0f)
        {
            world.DestroyEntity(entityId);
        }
    }
}

///-----------------------------------------------------------------------------------------------

}
