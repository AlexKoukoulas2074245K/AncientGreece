///------------------------------------------------------------------------------------------------
///  SceneUpdaterSystem.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///-----------------------------------------------------------------------------------------------

#include "SceneUpdaterSystem.h"
#include "../components/SceneStateSingletonComponent.h"
#include "../scenegraphs/QuadtreeSceneGraph.h"
#include "../../components/CollidableComponent.h"
#include "../../../engine/debug/components/DebugViewStateSingletonComponent.h"
#include "../../../engine/common/utils/Logging.h"
#include "../../../engine/rendering/utils/MeshUtils.h"

///-----------------------------------------------------------------------------------------------

namespace scene
{

///-----------------------------------------------------------------------------------------------

static const std::string DEBUG_SQUARE_ENTITY_NAME = "unit_xy_square";
static const glm::vec3 SCENE_DIMENSIONS = glm::vec3(1.0f, 1.0f, 0.0f);

///-----------------------------------------------------------------------------------------------

SceneUpdaterSystem::SceneUpdaterSystem()
{
    auto sceneStateComponent = std::make_unique<SceneStateSingletonComponent>();
    sceneStateComponent->mSceneGraph = std::make_unique<QuadtreeSceneGraph>(glm::vec3(0.0f), SCENE_DIMENSIONS);
    genesis::ecs::World::GetInstance().SetSingletonComponent<SceneStateSingletonComponent>(std::move(sceneStateComponent));
}

///-----------------------------------------------------------------------------------------------

void SceneUpdaterSystem::VUpdate(const float, const std::vector<genesis::ecs::EntityId>& entitiesToProcess) const
{
    auto& world = genesis::ecs::World::GetInstance();
    auto& sceneStateComponent = world.GetSingletonComponent<SceneStateSingletonComponent>();
    auto& debugViewStateComponent = world.GetSingletonComponent<genesis::debug::DebugViewStateSingletonComponent>();
    
    sceneStateComponent.mSceneGraph->VClear();
    sceneStateComponent.mSceneGraph->VPopulateSceneGraph(entitiesToProcess);
    
    world.DestroyEntities(world.FindAllEntitiesWithName(StringId(DEBUG_SQUARE_ENTITY_NAME)));
    if (debugViewStateComponent.mSceneGraphDisplayEnabled)
    {
        const auto& newDebugRectangles = sceneStateComponent.mSceneGraph->VGetDebugRenderRectangles();
        for (const auto& debugRectangleInfo: newDebugRectangles)
        {
            genesis::rendering::LoadAndCreateStaticModelByName(DEBUG_SQUARE_ENTITY_NAME, debugRectangleInfo.first, glm::vec3(), debugRectangleInfo.second, StringId(DEBUG_SQUARE_ENTITY_NAME));
        }
    }
}

///-----------------------------------------------------------------------------------------------

}
