///------------------------------------------------------------------------------------------------
///  ViewQueueSingletonComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 18/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef ViewQueueSingletonComponent_h
#define ViewQueueSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

#include <queue>
#include <utility>

///-----------------------------------------------------------------------------------------------

namespace view
{

///-----------------------------------------------------------------------------------------------

class ViewQueueSingletonComponent final: public genesis::ecs::IComponent
{
public:
    std::queue<std::pair<std::string, StringId>> mQueuedViews;
    bool mActiveViewExists = false;
};

///-----------------------------------------------------------------------------------------------
    
}

///-----------------------------------------------------------------------------------------------

#endif /* ViewQueueSingletonComponent_h */
