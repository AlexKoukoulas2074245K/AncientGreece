///------------------------------------------------------------------------------------------------
///  SceneStateSingletonComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
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
