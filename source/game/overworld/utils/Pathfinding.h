///------------------------------------------------------------------------------------------------
///  Pathfinding.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 22/03/2021.
///------------------------------------------------------------------------------------------------

#ifndef Pathfinding_h
#define Pathfinding_h

///------------------------------------------------------------------------------------------------

#include "../AreaTypes.h"
#include "../../../engine/common/utils/MathUtils.h"

#include <list>

///------------------------------------------------------------------------------------------------

namespace genesis
{
    namespace resources
    {
        class TextureResource;
    }
}

///------------------------------------------------------------------------------------------------

namespace overworld
{

///------------------------------------------------------------------------------------------------

std::list<glm::vec3> FindPath(const glm::vec3& startPos, const glm::vec3& endPos, const AreaTypeMask& unitNavigableAreaMask, const glm::vec3& mapDimensions, const genesis::resources::TextureResource& navmapTexture);

///------------------------------------------------------------------------------------------------

}

///------------------------------------------------------------------------------------------------

#endif /* Pathfinding_h */
