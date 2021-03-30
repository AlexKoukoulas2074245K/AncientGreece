///------------------------------------------------------------------------------------------------
///  OverworldMovementControllerSystem.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 12/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldMovementControllerSystem_h
#define OverworldMovementControllerSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include "../../../engine/common/utils/MathUtils.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{
    class TransformComponent;

    namespace rendering
    {
        class RenderableComponent;
    }

    namespace resources
    {
        class TextureResource;
    }
}

///-----------------------------------------------------------------------------------------------

class UnitStatsComponent;

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

class OverworldTargetComponent;

///-----------------------------------------------------------------------------------------------
class OverworldMovementControllerSystem final : public genesis::ecs::BaseSystem<genesis::TransformComponent, genesis::rendering::RenderableComponent, OverworldTargetComponent, UnitStatsComponent>
{
public:
    OverworldMovementControllerSystem();

    void VUpdate(const float dt, const std::vector<genesis::ecs::EntityId>&) const override;

private:
    void UpdatePosition(const float dt, const float speed, const glm::vec3& targetPosition, glm::vec3& entityPosition) const;
    void UpdateRotation(const float dt, const float targetRotation, glm::vec3& entityRotation) const;
    
    float GetTerrainSpeedMultiplier(const glm::vec3& unitPosition, const glm::vec3& mapDimensions, const genesis::resources::TextureResource& navmapTexture) const;
    
    bool HasCollidedWithTargetEntity(const genesis::ecs::EntityId entity, const genesis::ecs::EntityId targetEntity) const;
};

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldMovementControllerSystem_h */
