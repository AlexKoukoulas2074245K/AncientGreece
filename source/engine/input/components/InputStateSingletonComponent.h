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
#include <SDL_mouse.h>
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

enum class Key
{
    A_KEY, B_KEY, C_KEY, D_KEY, E_KEY, F_KEY, G_KEY, H_KEY, I_KEY, J_KEY,
    K_KEY, L_KEY, M_KEY, N_KEY, O_KEY, P_KEY, Q_KEY, R_KEY, S_KEY, T_KEY,
    U_KEY, V_KEY, W_KEY, X_KEY, Y_KEY, Z_KEY, SHIFT_KEY, BACKSPACE_KEY, ENTER_KEY,
    UP_ARROW_KEY, DOWN_ARROW_KEY, LEFT_ARROW_KEY, RIGHT_ARROW_KEY,
    TILDE_KEY, SPACEBAR_KEY
};

///-----------------------------------------------------------------------------------------------

enum class Button
{
    LEFT_BUTTON, MIDDLE_BUTTON, RIGHT_BUTTON
};

///-----------------------------------------------------------------------------------------------

enum class InputState
{
    RELEASED, PRESSED, TAPPED
};

///-----------------------------------------------------------------------------------------------

class InputStateSingletonComponent final: public ecs::IComponent
{
public:
    tsl::robin_map<SDL_Scancode, Key> mKeybindings =
    {
        {SDL_SCANCODE_A, Key::A_KEY}, {SDL_SCANCODE_B, Key::B_KEY},
        {SDL_SCANCODE_C, Key::C_KEY}, {SDL_SCANCODE_D, Key::D_KEY},
        {SDL_SCANCODE_E, Key::E_KEY}, {SDL_SCANCODE_F, Key::F_KEY},
        {SDL_SCANCODE_G, Key::G_KEY}, {SDL_SCANCODE_H, Key::H_KEY},
        {SDL_SCANCODE_I, Key::I_KEY}, {SDL_SCANCODE_J, Key::J_KEY},
        {SDL_SCANCODE_K, Key::K_KEY}, {SDL_SCANCODE_L, Key::L_KEY},
        {SDL_SCANCODE_M, Key::M_KEY}, {SDL_SCANCODE_N, Key::N_KEY},
        {SDL_SCANCODE_O, Key::O_KEY}, {SDL_SCANCODE_P, Key::P_KEY},
        {SDL_SCANCODE_Q, Key::Q_KEY}, {SDL_SCANCODE_R, Key::R_KEY},
        {SDL_SCANCODE_S, Key::S_KEY}, {SDL_SCANCODE_T, Key::T_KEY},
        {SDL_SCANCODE_U, Key::U_KEY}, {SDL_SCANCODE_V, Key::V_KEY},
        {SDL_SCANCODE_W, Key::W_KEY}, {SDL_SCANCODE_X, Key::X_KEY},
        {SDL_SCANCODE_Y, Key::Y_KEY}, {SDL_SCANCODE_Z, Key::Z_KEY},
        {SDL_SCANCODE_LSHIFT, Key::SHIFT_KEY},
        {SDL_SCANCODE_BACKSPACE, Key::BACKSPACE_KEY},
        {SDL_SCANCODE_RETURN, Key::ENTER_KEY},
        {SDL_SCANCODE_GRAVE, Key::TILDE_KEY},
        {SDL_SCANCODE_LEFT, Key::LEFT_ARROW_KEY},
        {SDL_SCANCODE_RIGHT, Key::RIGHT_ARROW_KEY},
        {SDL_SCANCODE_UP, Key::UP_ARROW_KEY},
        {SDL_SCANCODE_DOWN, Key::DOWN_ARROW_KEY},
        {SDL_SCANCODE_SPACE, Key::SPACEBAR_KEY}
    };
    
    tsl::robin_map<int, Button> mButtonBindings =
    {
        {SDL_BUTTON_LEFT, Button::LEFT_BUTTON},
        {SDL_BUTTON_MIDDLE, Button::MIDDLE_BUTTON},
        {SDL_BUTTON_RIGHT, Button::RIGHT_BUTTON}
    };
    
    tsl::robin_map<Key, InputState> mCurrentKeyboardState;
    std::vector<unsigned char> mPreviousRawKeyboardState;
    
    tsl::robin_map<Button, InputState> mCurrentButtonsState;
    tsl::robin_map<Button, InputState> mPreviousButtonsState;
    int mMouseX, mMouseY, mMouseWheelDelta;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* InputStateSingletonComponent_h */
