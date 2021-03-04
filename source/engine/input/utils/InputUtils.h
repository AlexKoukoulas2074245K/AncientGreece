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
#include "../../common/utils/MathUtils.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace input
{

///-----------------------------------------------------------------------------------------------
/// Returns the state of the given key.
/// @param[in] key the key to check for its state.
/// @returns the state of the specified key.
inline InputState GetKeyState(const Key key)
{
    auto& world = ecs::World::GetInstance();
    const auto& inputStateComponent = world.GetSingletonComponent<InputStateSingletonComponent>();
    return inputStateComponent.mCurrentKeyboardState.at(key);
}

///-----------------------------------------------------------------------------------------------
/// Returns the state of the given button.
/// @param[in] button the button to check for its state.
/// @returns the state of the specified button.
inline InputState GetButtonState(const Button button)
{
    auto& world = ecs::World::GetInstance();
    const auto& inputStateComponent = world.GetSingletonComponent<InputStateSingletonComponent>();
    return inputStateComponent.mCurrentButtonsState.at(button);
}

///-----------------------------------------------------------------------------------------------
/// Returns current delta of the mouse wheel (> 0 scroll up, < 0 scroll down).
/// @returns the delta of the mouse wheel.
inline int GetMouseWheelDelta()
{
    auto& world = ecs::World::GetInstance();
    const auto& inputStateComponent = world.GetSingletonComponent<InputStateSingletonComponent>();
    return inputStateComponent.mMouseWheelDelta;
}

///-----------------------------------------------------------------------------------------------
/// Returns current mouse position relative to the focused window.
/// @returns the mouse position in window space wrapped in an ivec2
inline glm::ivec2 GetMousePosition()
{
    auto& world = ecs::World::GetInstance();
    const auto& inputStateComponent = world.GetSingletonComponent<InputStateSingletonComponent>();
    return glm::ivec2(inputStateComponent.mMouseX, inputStateComponent.mMouseY);
}

}

}

///-----------------------------------------------------------------------------------------------

#endif /* InputUtils_h */
