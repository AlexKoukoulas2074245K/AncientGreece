///------------------------------------------------------------------------------------------------
///  FontsStoreSingletonComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 09/01/2020.
///-----------------------------------------------------------------------------------------------

#ifndef FontsStoreSingletonComponent_h
#define FontsStoreSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../resources/ResourceLoadingService.h"

#include <tsl/robin_map.h>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------

class FontsStoreSingletonComponent final: public ecs::IComponent
{
public:
    tsl::robin_map<StringId, tsl::robin_map<char, resources::ResourceId>, StringIdHasher> mLoadedFonts;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* FontsStoreSingletonComponent_h */
