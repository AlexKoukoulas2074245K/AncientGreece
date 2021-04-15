///------------------------------------------------------------------------------------------------
///  BattleAttackTriggerHandlingSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 05/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef BattleAttackTriggerHandlingSystem_h
#define BattleAttackTriggerHandlingSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include "../../engine/rendering/components/RenderableComponent.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{
    class TransformComponent;
    
    namespace rendering
    {
        class RenderableComponent;
    }
}

///-----------------------------------------------------------------------------------------------

class CollidableComponent;
class UnitStatsComponent;

///-----------------------------------------------------------------------------------------------

namespace battle
{

///-----------------------------------------------------------------------------------------------
class BattleAttackTriggerHandlingSystem final : public genesis::ecs::BaseSystem<genesis::TransformComponent, genesis::rendering::RenderableComponent, CollidableComponent, UnitStatsComponent>
{
public:
    BattleAttackTriggerHandlingSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;
    
private:
    void CreateProjectile(const genesis::ecs::EntityId sourceEntityId) const;
    void DamageTarget(const genesis::ecs::EntityId sourceEntityId) const;
    glm::vec3 CalculateProjectileTargetOffset(const glm::vec3& targetEntityScaledDimensions) const;
    glm::vec3 CalculateProjectileOriginOffset(const glm::vec3& sourceEntityScaledDimensions, const glm::vec3& targetPosition) const;
    glm::vec3 CalculateProjectileRotation(const glm::vec3& vecToTarget) const;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* BattleAttackTriggerHandlingSystem_h */
