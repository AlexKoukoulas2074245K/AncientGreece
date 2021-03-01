///------------------------------------------------------------------------------------------------
///  InputUtils.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef InputUtils_h
#define InputUtils_h

///-----------------------------------------------------------------------------------------------

#include "../components/InputStateSingletonComponent.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace input
{

///-----------------------------------------------------------------------------------------------
/// Tests whether the state of the given input action is TAPPED.
/// @param[in] actionType the action type to check for its state.
/// @param[in] world the singular world of the ECS state.
/// @returns whether the state of the input action type is TAPPED.
inline bool IsActionTypeKeyTapped(const InputActionType actionType)
{
    auto& world = ecs::World::GetInstance();
    const auto& inputStateComponent = world.GetSingletonComponent<InputStateSingletonComponent>();
    return inputStateComponent.mCurrentInputState.at(actionType) == InputActionState::TAPPED;
}

///-----------------------------------------------------------------------------------------------
/// Tests whether the state of the given input action is PRESSED.
/// @param[in] actionType the action type to check for its state.
/// @param[in] world the singular world of the ECS state.
/// @returns whether the state of the input action type is PRESSED.
inline bool IsActionTypeKeyPressed(const InputActionType actionType)
{
    auto& world = ecs::World::GetInstance();
    const auto& inputStateComponent = world.GetSingletonComponent<InputStateSingletonComponent>();
    return inputStateComponent.mCurrentInputState.at(actionType) == InputActionState::PRESSED;
}

/// Tests whether the state of the given input action is RELEASED.
/// @param[in] actionType the action type to check for its state.
/// @param[in] world the singular world of the ECS state.
/// @returns whether the state of the input action type is RELEASED.
inline bool IsActionTypeKeyReleased(const InputActionType actionType)
{
    auto& world = ecs::World::GetInstance();
    const auto& inputStateComponent = world.GetSingletonComponent<InputStateSingletonComponent>();
    return inputStateComponent.mCurrentInputState.at(actionType) == InputActionState::RELEASED;
}

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* InputUtils_h */
