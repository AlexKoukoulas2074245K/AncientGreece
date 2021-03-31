///------------------------------------------------------------------------------------------------
///  CollidableComponent.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 31/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef CollidableComponent_h
#define CollidableComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../engine/ECS.h"
#include "../../engine/common/utils/MathUtils.h"

///-----------------------------------------------------------------------------------------------

class CollidableComponent final: public genesis::ecs::IComponent
{
public:
    glm::vec3 mCollidableDimensions;
};

///-----------------------------------------------------------------------------------------------

#endif /* CollidableComponent_h */
