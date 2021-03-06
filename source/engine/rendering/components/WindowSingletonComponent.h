///------------------------------------------------------------------------------------------------
///  WindowSingletonComponent.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef WindowSingletonComponent_h
#define WindowSingletonComponent_h

///-----------------------------------------------------------------------------------------------

#include "../../ECS.h"

#include <string>

///-----------------------------------------------------------------------------------------------

struct SDL_Window;

///-----------------------------------------------------------------------------------------------

namespace genesis
{

///-----------------------------------------------------------------------------------------------

namespace rendering
{

///-----------------------------------------------------------------------------------------------

class WindowSingletonComponent final: public ecs::IComponent
{
public:
    SDL_Window* mWindowHandle  = nullptr;
    std::string mWindowTitle   = "";
    float mRenderableWidth     = 0.0f;
    float mRenderableHeight    = 0.0f;
    float mAspectRatio         = 0.0f;
};

///-----------------------------------------------------------------------------------------------

}

}

///-----------------------------------------------------------------------------------------------

#endif /* WindowSingletonComponent_h */
