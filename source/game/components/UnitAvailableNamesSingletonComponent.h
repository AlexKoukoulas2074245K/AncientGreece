///------------------------------------------------------------------------------------------------
///  UnitAvailableNamesSingletonComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef UnitAvailableNamesSingletonComponent_h
#define UnitAvailableNamesSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"

#include <vector>

///-----------------------------------------------------------------------------------------------

class UnitAvailableNamesSingletonComponent final: public genesis::ecs::IComponent
{
public:
    std::vector<std::string> mAvailableUnitNamesList;
};

///-----------------------------------------------------------------------------------------------

#endif /* UnitAvailableNamesSingletonComponent_h */
