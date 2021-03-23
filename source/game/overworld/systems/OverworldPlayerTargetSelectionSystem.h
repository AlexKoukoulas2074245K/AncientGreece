///------------------------------------------------------------------------------------------------
///  OverworldPlayerTargetSelectionSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 05/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldPlayerTargetSelectionSystem_h
#define OverworldPlayerTargetSelectionSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

class HighlightableComponent;
class OverworldTargetComponent;

///-----------------------------------------------------------------------------------------------
class OverworldPlayerTargetSelectionSystem final : public genesis::ecs::BaseSystem<HighlightableComponent>
{
public:
    OverworldPlayerTargetSelectionSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const override;

private:
    genesis::ecs::EntityId GetEntityToFollow(const std::vector<genesis::ecs::EntityId>& entitiesToProcess, const genesis::ecs::EntityId playerEntity, genesis::ecs::World&) const;
    void CalculateMapTarget(OverworldTargetComponent& targetComponent, genesis::ecs::World& world) const;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldPlayerTargetSelectionSystem_h */
