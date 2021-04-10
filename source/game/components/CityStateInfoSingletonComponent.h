///------------------------------------------------------------------------------------------------
///  CityStateInfoSingletonComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 25/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef CityStateInfoSingletonComponent_h
#define CityStateInfoSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include "../../engine/common/utils/MathUtils.h"

#include <tsl/robin_map.h>

///-----------------------------------------------------------------------------------------------

struct CityStateInfo
{
    std::string mDescription;
    glm::vec3 mPosition;
    glm::vec3 mRotation;
    int mRenown;
    int mGarisson;
};

///-----------------------------------------------------------------------------------------------

class CityStateInfoSingletonComponent final: public genesis::ecs::IComponent
{
public:
    tsl::robin_map<StringId, CityStateInfo, StringIdHasher> mCityStateNameToInfo;
};

///-----------------------------------------------------------------------------------------------

#endif /* CityStateInfoSingletonComponent_h */
