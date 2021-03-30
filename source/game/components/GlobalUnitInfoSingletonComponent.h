///------------------------------------------------------------------------------------------------
///  GlobalUnitInfoSingletonComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 29/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef GlobalUnitInfoSingletonComponent_h
#define GlobalUnitInfoSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

#include <vector>

///-----------------------------------------------------------------------------------------------

class GlobalUnitInfoSingletonComponent final: public genesis::ecs::IComponent
{
public:
    std::vector<std::string> mAvailableUnitNamesList;
};

///-----------------------------------------------------------------------------------------------

#endif /* GlobalUnitInfoSingletonComponent_h */
