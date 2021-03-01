///------------------------------------------------------------------------------------------------
///  SceneStateSingletonComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 24/07/2020.
///-----------------------------------------------------------------------------------------------

#ifndef SceneStateSingletonComponent_h
#define SceneStateSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../../engine/ECS.h"
#include <memory>

///-----------------------------------------------------------------------------------------------

namespace scene
{

///-----------------------------------------------------------------------------------------------

class ISceneGraph;

///-----------------------------------------------------------------------------------------------

class SceneStateSingletonComponent final: public genesis::ecs::IComponent
{
public:
    std::unique_ptr<ISceneGraph> mSceneGraph;
};

///-----------------------------------------------------------------------------------------------

}

#endif /* SceneStateSingletonComponent_h */
