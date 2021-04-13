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
#include "../../common/utils/MathUtils.h"

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
/// @param[in] heightMapHeightScale the height scale of the height map (i.e. highest peak height)
/// @param[in] heightMapWidthScale the width scale of the height map (i.e. x, z scale)
/// @param[in] entityName (optional) a string to name the entity with.
/// @returns the entity id of the loaded heightMap entity.
ecs::EntityId LoadAndCreateHeightMapByName
(
    const std::string& heightMapName,
    const float heightMapHeightScale,
    const float heightMapWidthScale,
    const StringId entityName = StringId()
);

///------------------------------------------------------------------------------------------------
/// Returns the z height of the entity's heighmap at the position provided
///
/// Note: this helper function assumes that the heightMap name is a folder inside textures/heightMaps and
/// where the heightMap.png is the actual heightMap texture, while the one or more textures inside heightMap_textures
/// are the textures to blend depending on vertex height.
/// @param[in] heightMapEntityId the entity holding a HeightMap component.
/// @param[in] position the position to check.
/// @returns the calculated height at position.
float GetTerrainHeightAtPosition(const genesis::ecs::EntityId heightMapEntityId, const glm::vec3& position);

///------------------------------------------------------------------------------------------------
}

}

///------------------------------------------------------------------------------------------------

#endif /* HeightMapUtils_h */
