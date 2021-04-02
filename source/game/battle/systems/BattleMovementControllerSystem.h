///------------------------------------------------------------------------------------------------
///  BattleMovementControllerSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 02/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef BattleMovementControllerSystem_h
#define BattleMovementControllerSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include "../../../engine/common/utils/MathUtils.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{
    class TransformComponent;
}

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------

class BattleTargetComponent;

///-----------------------------------------------------------------------------------------------
class BattleMovementControllerSystem final : public genesis::ecs::BaseSystem<genesis::TransformComponent, BattleTargetComponent>
{
public:
    BattleMovementControllerSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;

private:
    void UpdatePosition(const float dt, const float speed, const glm::vec3& targetPosition, glm::vec3& entityPosition) const;
    void UpdateRotation(const float dt, const float targetRotation, glm::vec3& entityRotation) const;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* BattleMovementControllerSystem_h */
