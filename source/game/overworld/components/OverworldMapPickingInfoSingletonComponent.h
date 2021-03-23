///------------------------------------------------------------------------------------------------
///  OverworldMapPickingInfoSingletonComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 23/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldMapPickingInfoSingletonComponent_h
#define OverworldMapPickingInfoSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include "../../engine/common/utils/MathUtils.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{

///-----------------------------------------------------------------------------------------------

class OverworldMapPickingInfoSingletonComponent final: public genesis::ecs::IComponent
{
public:
    glm::vec3 mMouseRayDirection;
    glm::vec3 mMapIntersectionPoint;
};

///-----------------------------------------------------------------------------------------------
    
}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldMapPickingInfoSingletonComponent_h */
