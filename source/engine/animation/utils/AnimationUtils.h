///------------------------------------------------------------------------------------------------
///  AnimationUtils.h
///  Genesis
///
///  Created by Alex Koukoulas on 13/03/2021.
///-----------------------------------------------------------------------------------------------

#ifndef AnimationUtils_h
#define AnimationUtils_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"
#include "../../common/utils/StringUtils.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace animation
{

///------------------------------------------------------------------------------------------------
/// Sets the current playing animation of an animated model
///
/// @param[in] entityId the entity hodling the animated model.
/// @param[in] animationName the animation that will be set.
/// @param[in] randomizeStartTime (optional) randomizes the animation time for the anim.
void SetAnimation(const ecs::EntityId entityId, const StringId animationName, const bool randomizeStartTime = false);

///------------------------------------------------------------------------------------------------
/// Changes the current playing animation of an animated model IF ITS NOT CURRENTLY PLAYING already
///
/// @param[in] entityId the entity hodling the animated model.
/// @param[in] animationName the animation that will be changed to.
/// @param[in] randomizeStartTime (optional) randomizes the animation time for the new anim.
void ChangeAnimation(const ecs::EntityId entityId, const StringId animationName);

///------------------------------------------------------------------------------------------------
/// Returns the mesh index for the given animation name.
///
/// @param[in] entityId the entity hodling the animated model.
/// @param[in] animationName the animation name to search for.
/// @returns the animation index found or -1 otherwise
int GetAnimationIndex(const ecs::EntityId entityId, const StringId animationName);

///-----------------------------------------------------------------------------------------------)

}

}
                     
///-----------------------------------------------------------------------------------------------)

#endif /* AnimationUtils_h */
