///------------------------------------------------------------------------------------------------
///  RawInputHandlingSystem.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#include "RawInputHandlingSystem.h"
#include "../components/InputStateSingletonComponent.h"

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace input
{

///-----------------------------------------------------------------------------------------------

RawInputHandlingSystem::RawInputHandlingSystem()
    : BaseSystem()
{
    auto inputStateComponent = std::make_unique<InputStateSingletonComponent>();
    inputStateComponent->mPreviousRawKeyboardState.resize(DEFAULT_KEY_COUNT, 0);

    ecs::World::GetInstance().SetSingletonComponent<InputStateSingletonComponent>(std::move(inputStateComponent));
}

///-----------------------------------------------------------------------------------------------

void RawInputHandlingSystem::VUpdate(const float, const std::vector<ecs::EntityId>&) const
{   
    auto keyboardStateLength         = 0;
    const auto* currentKeyboardState = SDL_GetKeyboardState(&keyboardStateLength);
    auto& inputStateComponent        = ecs::World::GetInstance().GetSingletonComponent<InputStateSingletonComponent>();

    for (const auto& keybindingEntry: inputStateComponent.mKeybindings)
    {
        const auto& sdlScancode         = keybindingEntry.first;
        const auto& mappedVirtualAction = keybindingEntry.second;

        // Key down this frame but not last frame (tap)
        if (currentKeyboardState[sdlScancode] && !inputStateComponent.mPreviousRawKeyboardState[sdlScancode])
        {
            inputStateComponent.mCurrentInputState[mappedVirtualAction] = InputActionState::TAPPED;
        }
        // Key down this frame and last frame (pressed)
        else if (currentKeyboardState[sdlScancode] && inputStateComponent.mPreviousRawKeyboardState[sdlScancode])
        {
            inputStateComponent.mCurrentInputState[mappedVirtualAction] = InputActionState::PRESSED;
        }
        // Key up this frame, but down last frame (released)
        else
        {
            inputStateComponent.mCurrentInputState[mappedVirtualAction] = InputActionState::RELEASED;
        }
    }
    
    inputStateComponent.mPreviousRawKeyboardState.assign(currentKeyboardState, currentKeyboardState + keyboardStateLength);
}

///-----------------------------------------------------------------------------------------------

}

}
