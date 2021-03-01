///------------------------------------------------------------------------------------------------
///  QuadtreeSceneGraph.h
///  Genesis
///
///  Created by Alex Koukoulas on 24/07/2020.
///-----------------------------------------------------------------------------------------------

#include "QuadtreeSceneGraph.h"
#include "../../physics/components/PhysicsComponent.h"
#include "../../../engine/common/components/TransformComponent.h"

///-----------------------------------------------------------------------------------------------

namespace scene
{

///-----------------------------------------------------------------------------------------------

static constexpr int MAX_OBJECTS_PER_NODE = 6;
static constexpr int MAX_DEPTH = 5;

///-----------------------------------------------------------------------------------------------

QuadtreeSceneGraph::QuadtreeSceneGraph(const glm::vec3& position, const glm::vec3& dimensions, const int depth /* 0 */)
    : mOrigin(position)
    , mDimensions(dimensions)
    , mDepth(depth)
{
    for (int i = 0; i < 4; ++i)
    {
        mNodes[i] = nullptr;
    }
    
    InternalClear();
}

///-----------------------------------------------------------------------------------------------

QuadtreeSceneGraph::~QuadtreeSceneGraph()
{
    InternalClear();
}

///-----------------------------------------------------------------------------------------------

const glm::vec3& QuadtreeSceneGraph::VGetSceneOrigin() const
{
    return mOrigin;
}

///-----------------------------------------------------------------------------------------------

const glm::vec3& QuadtreeSceneGraph::VGetSceneDimensions() const
{
    return mDimensions;
}

///-----------------------------------------------------------------------------------------------

std::list<genesis::ecs::EntityId> QuadtreeSceneGraph::VGetCollisionCandidates(const genesis::ecs::EntityId referenceEntityId) const
{
    const auto& world = genesis::ecs::World::GetInstance();
    
    const auto& entityPosition = world.GetComponent<genesis::TransformComponent>(referenceEntityId).mPosition;
    const auto& entityCollidableDimensions = world.GetComponent<physics::PhysicsComponent>(referenceEntityId).mCollidableDimensions;
    
    std::list<genesis::ecs::EntityId> collisionCandidates;
    InternalGetCollisionCandidates(referenceEntityId, entityPosition, entityCollidableDimensions, collisionCandidates);
    
    return collisionCandidates;
}

///-----------------------------------------------------------------------------------------------

void QuadtreeSceneGraph::VPopulateSceneGraph(const std::vector<genesis::ecs::EntityId>& physicallySimulatedEntities)
{
    const auto& world = genesis::ecs::World::GetInstance();
    
    for (const auto& entityId: physicallySimulatedEntities)
    {
        const auto& entityPosition = world.GetComponent<genesis::TransformComponent>(entityId).mPosition;
        const auto& entityCollidableDimensions = world.GetComponent<physics::PhysicsComponent>(entityId).mCollidableDimensions;
        
        InsertObject(entityId, entityPosition, entityCollidableDimensions);
    }
}

///-----------------------------------------------------------------------------------------------

void QuadtreeSceneGraph::VClear()
{
    InternalClear();
}

///-----------------------------------------------------------------------------------------------

std::list<std::pair<glm::vec3, glm::vec3>> QuadtreeSceneGraph::VGetDebugRenderRectangles() const
{
    std::list<std::pair<glm::vec3, glm::vec3>> debugRectangles;
    InternalGetDebugRenderRectangles(debugRectangles);
    return debugRectangles;
}

///-----------------------------------------------------------------------------------------------

void QuadtreeSceneGraph::InternalClear()
{
    mObjectsInNode.clear();
    for (int i = 0; i < 4; ++i)
    {
        if (mNodes[i] != nullptr)
        {
            mNodes[i]->InternalClear();            
        }
        mNodes[i] = nullptr;
    }
}

///-----------------------------------------------------------------------------------------------

void QuadtreeSceneGraph::InternalGetCollisionCandidates(const genesis::ecs::EntityId referenceEntityId, const glm::vec3& objectPosition, const glm::vec3& objectDimensions, std::list<genesis::ecs::EntityId>& collisionCandidates) const
{
    if (mNodes[0] != nullptr)
    {
        const auto quadrantIndex = GetMatchedQuadrant(objectPosition, objectDimensions);
        if (quadrantIndex != -1)
        {
            mNodes[quadrantIndex]->InternalGetCollisionCandidates(referenceEntityId, objectPosition, objectDimensions, collisionCandidates);
        }
        else
        {
            if (mNodes[0] != nullptr)
            {
                for (int i = 0; i < 4; ++i)
                {
                    mNodes[i]->InternalGetCollisionCandidates(referenceEntityId, objectPosition, objectDimensions, collisionCandidates);
                }
            }
        }
    }

    auto objectsIter = mObjectsInNode.begin();
    while (objectsIter != mObjectsInNode.end())
    {
        if (objectsIter->mEntityId != referenceEntityId)
        {
            collisionCandidates.push_back(objectsIter->mEntityId);
        }
        objectsIter++;
    }    
}

///-----------------------------------------------------------------------------------------------

void QuadtreeSceneGraph::InternalGetDebugRenderRectangles(std::list<std::pair<glm::vec3, glm::vec3>>& debugRectangles) const
{
    debugRectangles.push_back(std::make_pair(mOrigin, mDimensions));
    if (mNodes[0] != nullptr)
    {
        for (int i = 0; i < 4; ++i)
        {
            mNodes[i]->InternalGetDebugRenderRectangles(debugRectangles);
        }
    }
}

///-----------------------------------------------------------------------------------------------

void QuadtreeSceneGraph::Split()
{
    const auto halfWidth  = mDimensions.x * 0.5f;
    const auto halfHeight = mDimensions.y * 0.5f;
    const auto quadWidth  = halfWidth * 0.5f;
    const auto quadHeight = halfHeight * 0.5f;
    
    // Push inner quads a bit towards viewer to avoid z-fighting in debugging
    const auto zFrontPush = (mDepth + 1) * 0.01f;
    
    mNodes[0] = std::make_unique<QuadtreeSceneGraph>(glm::vec3(mOrigin.x - quadWidth, mOrigin.y + quadHeight, mOrigin.z + zFrontPush), glm::vec3(halfWidth, halfHeight, 0.0f), mDepth + 1);
    mNodes[1] = std::make_unique<QuadtreeSceneGraph>(glm::vec3(mOrigin.x + quadWidth, mOrigin.y + quadHeight, mOrigin.z + zFrontPush), glm::vec3(halfWidth, halfHeight, 0.0f), mDepth + 1);
    mNodes[2] = std::make_unique<QuadtreeSceneGraph>(glm::vec3(mOrigin.x - quadWidth, mOrigin.y - quadHeight, mOrigin.z + zFrontPush), glm::vec3(halfWidth, halfHeight, 0.0f), mDepth + 1);
    mNodes[3] = std::make_unique<QuadtreeSceneGraph>(glm::vec3(mOrigin.x + quadWidth, mOrigin.y - quadHeight, mOrigin.z + zFrontPush), glm::vec3(halfWidth, halfHeight, 0.0f), mDepth + 1);
}

///-----------------------------------------------------------------------------------------------

int QuadtreeSceneGraph::GetMatchedQuadrant(const glm::vec3& objectPosition, const glm::vec3& objectDimensions) const
{
    int quadrant = -1;
    
    const auto objectHalfWidth = objectDimensions.x * 0.5f;
    const auto objectHalfHeight = objectDimensions.y * 0.5f;
    const auto halfSpaceVerticalLinePoint = mOrigin.x;
    const auto halfSpaceHorizontalLinePoint = mOrigin.y;
    
    // completely fits in the left halfspace region of the current node
    if (objectPosition.x + objectHalfWidth < halfSpaceVerticalLinePoint)
    {
        // completely fits in the bottom left quadrant
        if (objectPosition.y + objectHalfHeight < halfSpaceHorizontalLinePoint)
        {
            quadrant = 2;
        }
        // completly fits in the top left quadrant
        else if (objectPosition.y - objectHalfHeight > halfSpaceHorizontalLinePoint)
        {
            quadrant = 0;
        }
    }
    // completely fits in the right halfspace region of the current node
    else if (objectPosition.x - objectHalfWidth > halfSpaceVerticalLinePoint)
    {
        // completely fits in the bottom right quadrant
        if (objectPosition.y + objectHalfHeight < halfSpaceHorizontalLinePoint)
        {
            quadrant = 3;
        }
        // completly fits in the top right quadrant
        else if (objectPosition.y - objectHalfHeight > halfSpaceHorizontalLinePoint)
        {
            quadrant = 1;
        }
    }
    
    return quadrant;
}

///-----------------------------------------------------------------------------------------------

void QuadtreeSceneGraph::InsertObject(const genesis::ecs::EntityId entityId, const glm::vec3& objectPosition, const glm::vec3& objectDimensions)
{
    const auto isQuadtreeSplit = mNodes[0] != nullptr;
    
    if (isQuadtreeSplit)
    {
        const auto quadrantIndex = GetMatchedQuadrant(objectPosition, objectDimensions);
        
        if (quadrantIndex != -1)
        {
            mNodes[quadrantIndex]->InsertObject(entityId, objectPosition, objectDimensions);
            return;
        }
    }
    
    mObjectsInNode.push_back(QuadtreeEntityEntry(entityId, objectPosition, objectDimensions));
    
    if (mObjectsInNode.size() > MAX_OBJECTS_PER_NODE && mDepth < MAX_DEPTH)
    {
        if (!isQuadtreeSplit)
        {
            Split();
        }
        
        auto objectsIter = mObjectsInNode.begin();
        while (objectsIter != mObjectsInNode.end())
        {
            const auto objectQuadrantIndex = GetMatchedQuadrant(objectsIter->mObjectPosition, objectsIter->mObjectDimensions);
            if (objectQuadrantIndex != -1)
            {
                mNodes[objectQuadrantIndex]->InsertObject(objectsIter->mEntityId, objectsIter->mObjectPosition, objectsIter->mObjectDimensions);
                objectsIter = mObjectsInNode.erase(objectsIter);
            }
            else
            {
                objectsIter++;
            }
        }
    }
}

///-----------------------------------------------------------------------------------------------

}

///-----------------------------------------------------------------------------------------------
