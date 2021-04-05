///------------------------------------------------------------------------------------------------
///  BattleCollisionHandlingSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 02/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef BattleCollisionHandlingSystem_h
#define BattleCollisionHandlingSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include "../../engine/common/utils/MathUtils.h"

#include <tsl/robin_map.h>

///-----------------------------------------------------------------------------------------------

namespace genesis
{
    class TransformComponent;
}

///-----------------------------------------------------------------------------------------------

class CollidableComponent;

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------
class BattleCollisionHandlingSystem final : public genesis::ecs::BaseSystem<genesis::TransformComponent, CollidableComponent>
{
public:
    BattleCollisionHandlingSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;

private:
    void BuildCollisionDisplacementMap(const std::vector<genesis::ecs::EntityId>& entitiesToProcess, tsl::robin_map<genesis::ecs::EntityId, glm::vec3>& collisionDisplacementMap) const;
    void ApplyCollisionDisplacements(const float dt, const tsl::robin_map<genesis::ecs::EntityId, glm::vec3>& collisionDisplacementMap) const;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* BattleCollisionHandlingSystem_h */
