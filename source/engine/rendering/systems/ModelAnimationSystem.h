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

///-----------------------------------------------------------------------------------------------

class RenderableComponent;

///-----------------------------------------------------------------------------------------------

class ModelAnimationSystem final: public ecs::BaseSystem<TransformComponent, RenderableComponent>
{
public:
    ModelAnimationSystem();
    
    void VUpdate(const float dt, const std::vector<ecs::EntityId>&) const override;
    
private:
    void CalculateTransitionalTransformsInHierarchy(const float previousAnimationTime, const float transitionAnimationTime, const resources::SkeletonNode* node, const glm::mat4& parentTransform, const resources::MeshResource& previousMeshResource, resources::MeshResource& currentMeshResource) const;
    void CalculateTransformsInHierarchy(const float animationTime, const resources::SkeletonNode* node, const glm::mat4& parentTransform, resources::MeshResource& meshResource) const;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* ModelAnimationSystem_h */
