///------------------------------------------------------------------------------------------------
///  BoneAnimationSystem.h
///  Genesis
///
///  Created by Alex Koukoulas on 08/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef BoneAnimationSystem_h
#define BoneAnimationSystem_h

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

class BoneAnimationSystem final: public ecs::BaseSystem<TransformComponent, RenderableComponent>
{
public:
    BoneAnimationSystem();
    
    void VUpdate(const float dt, const std::vector<ecs::EntityId>&) const override;
    
private:
    void CalculateTransformsInHierarchy(const float animationTime, const resources::SkeletonNode* node, const glm::mat4& parentTransform, resources::MeshResource& meshResource) const;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* BoneAnimationSystem_h */
