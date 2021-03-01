///------------------------------------------------------------------------------------------------
///  QuadtreeSceneGraph.h
///  Genesis
///
///  Created by Alex Koukoulas on 24/07/2020.
///-----------------------------------------------------------------------------------------------

#ifndef QuadtreeSceneGraph_h
#define QuadtreeSceneGraph_h

///-----------------------------------------------------------------------------------------------

#include "ISceneGraph.h"
#include "../../../engine/common/utils/MathUtils.h"

#include <utility>
#include <memory>

///-----------------------------------------------------------------------------------------------

namespace scene
{

///-----------------------------------------------------------------------------------------------

class QuadtreeSceneGraph final: public ISceneGraph
{
public:
    QuadtreeSceneGraph(const glm::vec3& origin, const glm::vec3& dimensions, const int depth = 0);
    ~QuadtreeSceneGraph();
    
    const glm::vec3& VGetSceneOrigin() const override;
    const glm::vec3& VGetSceneDimensions() const override;
    std::list<genesis::ecs::EntityId> VGetCollisionCandidates(const genesis::ecs::EntityId referenceEntityId) const override;
    void VPopulateSceneGraph(const std::vector<genesis::ecs::EntityId>& physicallySimulatedEntities) override;
    void VClear() override;
    std::list<std::pair<glm::vec3, glm::vec3>> VGetDebugRenderRectangles() const override;
    
private:
    struct QuadtreeEntityEntry
    {
        QuadtreeEntityEntry(const genesis::ecs::EntityId entityId, const glm::vec3& objectPosition, const glm::vec3& objectDimensions)
            : mEntityId(entityId)
            , mObjectPosition(objectPosition)
            , mObjectDimensions(objectDimensions)
        {
        }
        
        const genesis::ecs::EntityId mEntityId;
        const glm::vec3 mObjectPosition;
        const glm::vec3 mObjectDimensions;
    };
    
    void InternalClear();
    void InternalGetCollisionCandidates(const genesis::ecs::EntityId referenceEntityId, const glm::vec3& objectPosition, const glm::vec3& objectDimensions, std::list<genesis::ecs::EntityId>& collisionCandidates) const;
    void InternalGetDebugRenderRectangles(std::list<std::pair<glm::vec3, glm::vec3>>& debugRectangles) const;
    void Split();
    int GetMatchedQuadrant(const glm::vec3& objectPosition, const glm::vec3& objectDimensions) const;
    void InsertObject(const genesis::ecs::EntityId entityId, const glm::vec3& objectPosition, const glm::vec3& objectDimensions);
    
private:
    const glm::vec3 mOrigin;
    const glm::vec3 mDimensions;
    const int mDepth;
    
    std::unique_ptr<QuadtreeSceneGraph> mNodes[4];
    std::list<QuadtreeEntityEntry> mObjectsInNode;
};

///-----------------------------------------------------------------------------------------------

}

#endif /* QuadtreeSceneGraph_h */
