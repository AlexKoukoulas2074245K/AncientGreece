///------------------------------------------------------------------------------------------------
///  BattleTargetAcquisitionSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 02/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef BattleTargetAcquisitionSystem_h
#define BattleTargetAcquisitionSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{
    class TransformComponent;
}

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------

class BattleSideComponent;

///-----------------------------------------------------------------------------------------------
class BattleTargetAcquisitionSystem final : public genesis::ecs::BaseSystem<BattleSideComponent>
{
public:
    BattleTargetAcquisitionSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;

private:
    void ValidateCurrentTargetComponent(const genesis::ecs::EntityId entityId) const;
    void PickOptimalTargetForEntity(const genesis::ecs::EntityId entityId, const std::vector<genesis::ecs::EntityId> entities) const;
    genesis::ecs::EntityId FindClosestTargetUnit(const StringId currentEntityBattleLeader, const genesis::TransformComponent& currentEntityTransformComponent, const std::vector<genesis::ecs::EntityId> entities) const;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* BattleTargetAcquisitionSystem_h */
