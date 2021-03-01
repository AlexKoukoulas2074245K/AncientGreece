///------------------------------------------------------------------------------------------------
///  ISceneGraph.h
///  Genesis
///
///  Created by Alex Koukoulas on 24/07/2020.
///-----------------------------------------------------------------------------------------------

#ifndef ISceneGraph_h
#define ISceneGraph_h

///-----------------------------------------------------------------------------------------------

#include "../../../engine/ECS.h"
#include "../../../engine/common/utils/MathUtils.h"

#include <vector>
#include <list>
#include <utility>

///-----------------------------------------------------------------------------------------------

namespace scene
{

///-----------------------------------------------------------------------------------------------

class ISceneGraph
{
public:
    virtual ~ISceneGraph() = default;
    ISceneGraph(const ISceneGraph&) = delete;
    ISceneGraph& operator = (const ISceneGraph&) = delete;
    
    virtual const glm::vec3& VGetSceneOrigin() const = 0;
    virtual const glm::vec3& VGetSceneDimensions() const = 0;
    virtual std::list<genesis::ecs::EntityId> VGetCollisionCandidates(const genesis::ecs::EntityId referenceEntityId) const = 0;
    virtual void VPopulateSceneGraph(const std::vector<genesis::ecs::EntityId>& physicallySimulatedEntities) = 0;
    virtual void VClear() = 0;
    virtual std::list<std::pair<glm::vec3, glm::vec3>> VGetDebugRenderRectangles() const = 0;
    
protected:
    ISceneGraph() = default;
};

///-----------------------------------------------------------------------------------------------

}

#endif /* ISceneGraph_h */
