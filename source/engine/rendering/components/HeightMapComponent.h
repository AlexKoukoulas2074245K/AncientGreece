///------------------------------------------------------------------------------------------------
///  HeightMapComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 08/04/2021.
///-----------------------------------------------------------------------------------------------

#ifndef HeightMapComponent_h
#define HeightMapComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../common/utils/MathUtils.h"
#include "../../common/utils/StringUtils.h"

#include <vector>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------

using GLuint = unsigned int;
using ResourceId = size_t;

///-----------------------------------------------------------------------------------------------

class HeightMapComponent final: public ecs::IComponent
{
public:
    GLuint mVertexArrayObject = 0;
    glm::vec2 mHeightMapTextureDimensions;
    std::vector<ResourceId> mHeightMapTextureResourceIds;
    std::vector<std::vector<float>> mHeightMapTileHeights;
    float mHeightMapScale = 0.0f;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* HeightMapComponent_h */
