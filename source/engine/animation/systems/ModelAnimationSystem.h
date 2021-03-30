///------------------------------------------------------------------------------------------------
///  ModelAnimationSystem.h
///  Genesis
///
///  Created by Alex Koukoulas on 08/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef ModelAnimationSystem_h
#define ModelAnimationSystem_h

///-----------------------------------------------------------------------------------------------

#include "../../common/utils/MathUtils.h"
#include "../../resources/MeshResource.h"
#include "../../ECS.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

class TransformComponent;

///-----------------------------------------------------------------------------------------------

namespace rendering
{
    class RenderableComponent;
}

///-----------------------------------------------------------------------------------------------

namespace animation
{

///-----------------------------------------------------------------------------------------------

class ModelAnimationSystem final: public ecs::BaseSystem<TransformComponent, rendering::RenderableComponent>
{
public:
    ModelAnimationSystem();
    
    void VUpdate(const float dt, const std::vector<ecs::EntityId>&) const override;
    
private:
    void CalculateTransitionalTransformsInHierarchy(const float previousAnimationTime, const float transitionAnimationTime, const resources::SkeletonNode* node, const glm::mat4& parentTransform, const resources::MeshResource& previousMeshResource, const resources::MeshResource& currentMeshResource, rendering::RenderableComponent& renderableComponent) const;
    void CalculateTransformsInHierarchy(const float animationTime, const resources::SkeletonNode* node, const glm::mat4& parentTransform, const resources::MeshResource& meshResource, rendering::RenderableComponent& renderableComponent) const;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* ModelAnimationSystem_h */
