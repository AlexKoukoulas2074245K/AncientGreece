///------------------------------------------------------------------------------------------------
///  InputStateSingletonComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef InputStateSingletonComponent_h
#define InputStateSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"

#include <SDL_keyboard.h>
#include <tsl/robin_map.h>
#include <vector>

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace input
{

///-----------------------------------------------------------------------------------------------

static constexpr int DEFAULT_KEY_COUNT = 512;

///-----------------------------------------------------------------------------------------------

enum class InputActionType
{    
    CAMERA_MOVE_UP,
    CAMERA_MOVE_DOWN,
    CAMERA_MOVE_LEFT,
    CAMERA_MOVE_RIGHT,
    CAMERA_MOVE_FORWARD,
    CAMERA_MOVE_BACKWARD,       
    CAMERA_ZOOM_IN,
    CAMERA_ZOOM_OUT,
    CONSOLE_TOGGLE,
    SHIFT_KEY,
    BACKSPACE_KEY,
    ENTER_KEY,
    UP_ARROW_KEY,
    DOWN_ARROW_KEY,
    LEFT_ARROW_KEY,
    RIGHT_ARROW_KEY
};

///-----------------------------------------------------------------------------------------------

enum class InputActionState
{
    RELEASED, PRESSED, TAPPED
};

///-----------------------------------------------------------------------------------------------

class InputStateSingletonComponent final: public ecs::IComponent
{
public:
    tsl::robin_map<InputActionType, InputActionState> mCurrentInputState =
    {        
        { InputActionType::CAMERA_MOVE_UP,       InputActionState::RELEASED },
        { InputActionType::CAMERA_MOVE_DOWN,     InputActionState::RELEASED },
        { InputActionType::CAMERA_MOVE_LEFT,     InputActionState::RELEASED },
        { InputActionType::CAMERA_MOVE_RIGHT,    InputActionState::RELEASED },
        { InputActionType::CAMERA_MOVE_FORWARD,  InputActionState::RELEASED },
        { InputActionType::CAMERA_MOVE_BACKWARD, InputActionState::RELEASED },        
        { InputActionType::CAMERA_ZOOM_IN,       InputActionState::RELEASED },
        { InputActionType::CAMERA_ZOOM_OUT,      InputActionState::RELEASED },
        { InputActionType::CONSOLE_TOGGLE,       InputActionState::RELEASED },
        { InputActionType::SHIFT_KEY,            InputActionState::RELEASED },
        { InputActionType::BACKSPACE_KEY,        InputActionState::RELEASED },
        { InputActionType::ENTER_KEY,            InputActionState::RELEASED },
        { InputActionType::UP_ARROW_KEY,         InputActionState::RELEASED },
        { InputActionType::DOWN_ARROW_KEY,       InputActionState::RELEASED },
        { InputActionType::LEFT_ARROW_KEY,       InputActionState::RELEASED },
        { InputActionType::RIGHT_ARROW_KEY,      InputActionState::RELEASED }        
    };

    tsl::robin_map<SDL_Scancode, InputActionType> mKeybindings =
    {        
        { SDL_SCANCODE_Q,         InputActionType::CAMERA_MOVE_UP },
        { SDL_SCANCODE_E,         InputActionType::CAMERA_MOVE_DOWN },
        { SDL_SCANCODE_W,         InputActionType::CAMERA_MOVE_FORWARD },
        { SDL_SCANCODE_S,         InputActionType::CAMERA_MOVE_BACKWARD },
        { SDL_SCANCODE_A,         InputActionType::CAMERA_MOVE_LEFT },
        { SDL_SCANCODE_D,         InputActionType::CAMERA_MOVE_RIGHT },        
        { SDL_SCANCODE_Z,         InputActionType::CAMERA_ZOOM_IN },
        { SDL_SCANCODE_X,         InputActionType::CAMERA_ZOOM_OUT },
        { SDL_SCANCODE_GRAVE,     InputActionType::CONSOLE_TOGGLE },
        { SDL_SCANCODE_LSHIFT,    InputActionType::SHIFT_KEY },
        { SDL_SCANCODE_BACKSPACE, InputActionType::BACKSPACE_KEY },
        { SDL_SCANCODE_RETURN,    InputActionType::ENTER_KEY },
        { SDL_SCANCODE_UP,        InputActionType::UP_ARROW_KEY },
        { SDL_SCANCODE_DOWN,      InputActionType::DOWN_ARROW_KEY },
        { SDL_SCANCODE_LEFT,      InputActionType::LEFT_ARROW_KEY },
        { SDL_SCANCODE_RIGHT,     InputActionType::RIGHT_ARROW_KEY }
    };

    // Needed for tapped input checks 
    std::vector<unsigned char> mPreviousRawKeyboardState;        
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* InputStateSingletonComponent_h */
