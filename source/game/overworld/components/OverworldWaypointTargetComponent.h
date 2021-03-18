///------------------------------------------------------------------------------------------------
///  OverworldWaypointTargetComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 11/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldWaypointTargetComponent_h
#define OverworldWaypointTargetComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include "../../engine/common/utils/MathUtils.h"
#include "../../engine/common/utils/ColorUtils.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{
    
///-----------------------------------------------------------------------------------------------

enum class WayPointTargetAreaType
{
    NEUTRAL, BLOCKED, FOREST, SEA, MOUNTAIN, HIGH_MOUNTAIN
};

///-----------------------------------------------------------------------------------------------

const std::map<genesis::colors::RGBTriplet, WayPointTargetAreaType> RGB_TO_AREA_TYPE =
{
    { genesis::colors::RGBTriplet(255, 255, 255), WayPointTargetAreaType::BLOCKED },
    { genesis::colors::RGBTriplet(  0,   0, 255), WayPointTargetAreaType::SEA },
    { genesis::colors::RGBTriplet(119, 119, 119), WayPointTargetAreaType::MOUNTAIN },
    { genesis::colors::RGBTriplet( 51,  51,  51), WayPointTargetAreaType::HIGH_MOUNTAIN },
    { genesis::colors::RGBTriplet(  0, 255,   0), WayPointTargetAreaType::FOREST }
};

///-----------------------------------------------------------------------------------------------

class OverworldWaypointTargetComponent final: public genesis::ecs::IComponent
{
public:
    glm::vec3 mTargetPosition;
    WayPointTargetAreaType mTargetAreaType;
};

///-----------------------------------------------------------------------------------------------
    
}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldWaypointTargetComponent_h */
