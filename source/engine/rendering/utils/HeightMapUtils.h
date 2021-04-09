///------------------------------------------------------------------------------------------------
///  HeightMapUtils.h
///  Genesis
///
///  Created by Alex Koukoulas on 08/04/2021.
///------------------------------------------------------------------------------------------------

#ifndef HeightMapUtils_h
#define HeightMapUtils_h

///------------------------------------------------------------------------------------------------

#include "../../ECS.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace rendering
{

///------------------------------------------------------------------------------------------------
/// Returns an entity holding the loaded heightMap based on the model name supplied.
///
/// Note: this helper function assumes that the heightMap name is a folder inside textures/heightMaps and
/// where the heightMap.png is the actual heightMap texture, while the one or more textures inside heightMap_textures
/// are the textures to blend depending on vertex height.
/// @param[in] heightMapName the model with the given name to look for in the resource models folder.
/// @param[in] entityName (optional) a string to name the entity with.
/// @returns the entity id of the loaded heightMap entity.
ecs::EntityId LoadAndCreateHeightMapByName
(
    const std::string& heightMapName,
    const StringId entityName = StringId()
);

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* HeightMapUtils_h */
