///------------------------------------------------------------------------------------------------
///  MeshUtils.h
///  Genesis
///
///  Created by Alex Koukoulas on 09/01/2020.
///------------------------------------------------------------------------------------------------

#ifndef MeshUtils_h
#define MeshUtils_h

///------------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../common/utils/MathUtils.h"
#include "../../common/utils/StringUtils.h"
#include "../../resources/ResourceLoadingService.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace rendering
{

///------------------------------------------------------------------------------------------------
/// Loads and creates and entity holding the loaded model based on the model name supplied.
///
/// Note: this helper function assumes that the model name and texture name are the 
/// same in their respective resource folders.
/// @param[in] modelName the model with the given name to look for in the resource models folder.
/// @param[in] world the singular world of the ECS state.
/// @param[in] initialPosition (optional) an initial position for the loaded model.
/// @param[in] entityName (optional) a string to name the entity with.
/// @returns the entity id of the loaded entity.
ecs::EntityId LoadAndCreateModelByName
(
    const std::string& modelName,    
    const glm::vec3& initialPosition = glm::vec3(0.0f, 0.0f, 0.0f),
    const glm::vec3& initialRotation = glm::vec3(0.0f, 0.0f, 0.0f),
    const glm::vec3& initialScale = glm::vec3(1.0f, 1.0f, 1.0f),
    const StringId entityName = StringId()
);

///------------------------------------------------------------------------------------------------
/// Loads and creates and entity holding the loaded Gui sprite model based on the model and texture names supplied.
///
/// @param[in] modelName the model with the given name to look for in the resource models folder.
/// @param[in] textureName the texture with the given name to look for in the resource models folder.
/// @param[in] shaderName the shader with this name will be attached to the model.
/// @param[in] world the singular world of the ECS state.
/// @param[in] initialPosition (optional) an initial position for the loaded model.
/// @param[in] entityName (optional) a string to name the entity with.
/// @returns the entity id of the loaded entity.
ecs::EntityId LoadAndCreateGuiSprite
(
    const std::string& modelName,
    const std::string& textureName,
    const StringId shaderName,    
    const glm::vec3& initialPosition = glm::vec3(0.0f, 0.0f, 0.0f),
    const StringId entityName = StringId()
);

///------------------------------------------------------------------------------------------------
/// Loads and creates a mesh holding texture coords pointing to subregion of an atlas texture.
///
/// @param[in] meshAtlasCol the atlas column occupied by the desired subimage.
/// @param[in] meshAtlasRow the atlas row occupied by the desired subimage.
/// @param[in] atlasColCount the number of columns the atlas has.
/// @param[in] atlasRowCount the number of rows the atlas has.
/// @param[in] horizontalFlip (optional) whether or not result should be horizontally flipped.
/// @returns the resource id of the newly loaded mesh.
resources::ResourceId LoadAndCreateMeshFromAtlasTexCoords
(
    const int meshAtlasCol,
    const int meshAtlasRow,
    const int atlasColCount,
    const int atlasRowCount,
    const bool horizontalFlip = false
);

///------------------------------------------------------------------------------------------------

}

}

///------------------------------------------------------------------------------------------------

#endif /* MeshUtils_h */
