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
    
    for (const auto& keybindingEntry: inputStateComponent->mKeybindings)
    {
        inputStateComponent->mCurrentKeyboardState[keybindingEntry.second] = InputState::RELEASED;
    }
    
    for (const auto& buttonBindingEntry: inputStateComponent->mButtonBindings)
    {
        inputStateComponent->mCurrentButtonsState[buttonBindingEntry.second] = InputState::RELEASED;
        inputStateComponent->mPreviousButtonsState[buttonBindingEntry.second] = InputState::RELEASED;
    }
    
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
            inputStateComponent.mCurrentKeyboardState[mappedVirtualAction] = InputState::TAPPED;
        }
        // Key down this frame and last frame (pressed)
        else if (currentKeyboardState[sdlScancode] && inputStateComponent.mPreviousRawKeyboardState[sdlScancode])
        {
            inputStateComponent.mCurrentKeyboardState[mappedVirtualAction] = InputState::PRESSED;
        }
        // Key up this frame, but down last frame (released)
        else
        {
            inputStateComponent.mCurrentKeyboardState[mappedVirtualAction] = InputState::RELEASED;
        }
    }
    
    inputStateComponent.mPreviousRawKeyboardState.assign(currentKeyboardState, currentKeyboardState + keyboardStateLength);
    
    
    auto currentMouseState = SDL_GetMouseState(&inputStateComponent.mMouseX, &inputStateComponent.mMouseY);
    
    for (const auto& buttonBindingEntry: inputStateComponent.mButtonBindings)
    {
        const auto& sdlButton = buttonBindingEntry.first;
        const auto& mappedButton = buttonBindingEntry.second;
        
        // Button down this frame but not last frame (tap)
        if ((currentMouseState & SDL_BUTTON(sdlButton)) != 0 && inputStateComponent.mPreviousButtonsState[mappedButton] == InputState::RELEASED)
        {
            inputStateComponent.mCurrentButtonsState[mappedButton] = InputState::TAPPED;
        }
        // Button down this frame and last frame (pressed)
        else if ((currentMouseState & SDL_BUTTON(sdlButton)) != 0 && inputStateComponent.mPreviousButtonsState[mappedButton] != InputState::RELEASED)
        {
            inputStateComponent.mCurrentButtonsState[mappedButton] = InputState::PRESSED;
        }
        // Button up this frame, but down last frame (released)
        else
        {
            inputStateComponent.mCurrentButtonsState[mappedButton] = InputState::RELEASED;
        }
    }
    
    inputStateComponent.mPreviousButtonsState = inputStateComponent.mCurrentButtonsState;
}

///-----------------------------------------------------------------------------------------------

}

}
