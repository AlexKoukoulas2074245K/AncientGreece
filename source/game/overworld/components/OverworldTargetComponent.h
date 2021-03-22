///------------------------------------------------------------------------------------------------
///  OverworldTargetComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 11/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef OverworldTargetComponent_h
#define OverworldTargetComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include "../../engine/common/utils/MathUtils.h"
#include "../../engine/common/utils/ColorUtils.h"

///-----------------------------------------------------------------------------------------------

namespace overworld
{
    
///-----------------------------------------------------------------------------------------------

namespace areaTypeMasks
{
    static const int NEUTRAL       = 0x01;
    static const int BLOCKED       = 0x02;
    static const int FOREST        = 0x04;
    static const int SEA           = 0x08;
    static const int MOUNTAIN      = 0x10;
    static const int HIGH_MOUNTAIN = 0x20;
};

///-----------------------------------------------------------------------------------------------

const std::map<genesis::colors::RGBTriplet, int> RGB_TO_AREA_TYPE =
{
    { genesis::colors::RGBTriplet(255, 255, 255), areaTypeMasks::BLOCKED },
    { genesis::colors::RGBTriplet(  0,   0, 255), areaTypeMasks::SEA },
    { genesis::colors::RGBTriplet(119, 119, 119), areaTypeMasks::MOUNTAIN },
    { genesis::colors::RGBTriplet( 51,  51,  51), areaTypeMasks::HIGH_MOUNTAIN },
    { genesis::colors::RGBTriplet(  0, 255,   0), areaTypeMasks::FOREST }
};

///-----------------------------------------------------------------------------------------------

class OverworldTargetComponent final: public genesis::ecs::IComponent
{
public:
    genesis::ecs::EntityId mOptionalEntityTarget = genesis::ecs::NULL_ENTITY_ID;
    glm::vec3 mTargetPosition;
    int mTargetAreaType;
};

///-----------------------------------------------------------------------------------------------
    
}

///-----------------------------------------------------------------------------------------------

#endif /* OverworldTargetComponent_h */
