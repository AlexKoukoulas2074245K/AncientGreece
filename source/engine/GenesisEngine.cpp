///------------------------------------------------------------------------------------------------
///  GenesisEngine.cpp
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#include "GenesisEngine.h"
#include "common/components/TransformComponent.h"
#include "common/utils/Logging.h"
#include "common/utils/MathUtils.h"
#include "common/utils/OSMessageBox.h"
#include "debug/DefaultEngineConsoleCommands.h"
#include "debug/utils/ConsoleCommandUtils.h"
#include "debug/components/DebugViewStateSingletonComponent.h"
#include "input/components/InputStateSingletonComponent.h"
#include "input/components/InputStateSingletonComponent.h"
#include "input/systems/RawInputHandlingSystem.h"
#include "input/utils/InputUtils.h"
#include "rendering/components/RenderingContextSingletonComponent.h"
#include "rendering/components/WindowSingletonComponent.h"
#include "rendering/opengl/Context.h"
#include "rendering/systems/RenderingSystem.h"
#include "rendering/utils/FontUtils.h"
#include "resources/ResourceLoadingService.h"
#include "scripting/service/LuaScriptingService.h"
#include "scripting/DefaultEngineExportableFunctions.h"
#include "sound/SoundService.h"

#include <SDL.h> 
#include <SDL_events.h> 
#include <SDL_timer.h>

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------

namespace
{
    const StringId CONSOLE_FONT_NAME  = StringId("console_font");
    const int CONSOLE_FONT_ATLAS_COLS = 16;
    const int CONSOLE_FONT_ATLAS_ROWS = 16;
}

///------------------------------------------------------------------------------------------------

static bool AppShouldQuit();

///------------------------------------------------------------------------------------------------
    
GenesisEngine::GenesisEngine()
{            
}

///------------------------------------------------------------------------------------------------

void GenesisEngine::RunGame(const GameStartupParameters& startupParameters, IGame& game)
{
    Initialize(startupParameters);

    game.VOnSystemsInit();
    InitializeDefaultConsoleFont();
    debug::RegisterDefaultEngineConsoleCommands();
    game.VOnGameInit();

    auto dt                = 0.0f;
    auto elapsedTicks      = 0.0f;
    auto dtAccumulator     = 0.0f;
    auto framesAccumulator = 0LL; 

    while (!AppShouldQuit())
    {        
        UpdateFrameStatistics(dt, elapsedTicks, dtAccumulator, framesAccumulator);
        game.VOnUpdate(dt);
        ecs::World::GetInstance().Update(dt);
    }
}

///------------------------------------------------------------------------------------------------

void GenesisEngine::Initialize(const GameStartupParameters& startupParameters)
{       
    InitializeSdlContextAndWindow(startupParameters);
    InitializeServices();    
}

///------------------------------------------------------------------------------------------------

void GenesisEngine::InitializeSdlContextAndWindow(const GameStartupParameters& startupParameters)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        ShowMessageBox(MessageBoxType::ERROR, "Error initializing SDL", "An error has occurred while trying to initialize SDL");
        exit(1);
    }

    // Set SDL GL attributes        
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);    
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    const auto screenAspectRatio = displayMode.w / static_cast<float>(displayMode.h);

    auto requestedGameWidth = startupParameters.mGameWindowWidth;
    auto requestedGameHeight = startupParameters.mGameWindowHeight;

    // Check whether fraction constructor used
    if (requestedGameWidth == 0)
    {
        requestedGameWidth  = static_cast<int>(static_cast<float>(displayMode.w) * startupParameters.mGameWindowScreenFraction);
        requestedGameHeight = static_cast<int>(requestedGameWidth / screenAspectRatio);
    }
    
    // Create SDL window
    auto windowComponent = std::make_unique<rendering::WindowSingletonComponent>();
    windowComponent->mWindowTitle = startupParameters.mGameTitle;
    windowComponent->mWindowHandle = SDL_CreateWindow
    (
        windowComponent->mWindowTitle.c_str(),
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        requestedGameWidth,
        requestedGameHeight,
        SDL_WINDOW_OPENGL
    );

    if (windowComponent->mWindowHandle == nullptr)
    {
        ShowMessageBox(MessageBoxType::ERROR, "Error creating SDL window", "An error has occurred while trying to create an SDL_Window");
        exit(1);
    }

    // Make window non-resizable and display
    SDL_SetWindowResizable(windowComponent->mWindowHandle, SDL_FALSE);
    SDL_ShowWindow(windowComponent->mWindowHandle);

    // Create SDL GL context
    auto renderingContextComponent = std::make_unique<rendering::RenderingContextSingletonComponent>();
    renderingContextComponent->mGLContext = SDL_GL_CreateContext(windowComponent->mWindowHandle);
    if (renderingContextComponent->mGLContext == nullptr)
    {
        ShowMessageBox(MessageBoxType::ERROR, "Error creating SDL context", "An error has occurred while trying to create an SDL_Context");
        exit(1);
    }

    // Commit context
    SDL_GL_MakeCurrent(windowComponent->mWindowHandle, renderingContextComponent->mGLContext);
    SDL_GL_SetSwapInterval(0);

#ifdef _WIN32
    // Initialize GLES2 function table
    glFuncTable.initialize();
#endif

    // Get actual render buffer width/height
    auto renderableWidth  = 0;
    auto renderableHeight = 0;
    SDL_GL_GetDrawableSize(windowComponent->mWindowHandle, &renderableWidth, &renderableHeight);

    windowComponent->mRenderableWidth  = static_cast<float>(renderableWidth);
    windowComponent->mRenderableHeight = static_cast<float>(renderableHeight);
    windowComponent->mAspectRatio      = windowComponent->mRenderableWidth/windowComponent->mRenderableHeight;
    
    // Log GL driver info
    Log(LogType::INFO, "Vendor     : %s", GL_NO_CHECK(glGetString(GL_VENDOR)));
    Log(LogType::INFO, "Renderer   : %s", GL_NO_CHECK(glGetString(GL_RENDERER)));
    Log(LogType::INFO, "Version    : %s", GL_NO_CHECK(glGetString(GL_VERSION)));

    // Configure Blending
    GL_CHECK(glEnable(GL_BLEND));
    GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    
    // Configure Depth
    GL_CHECK(glEnable(GL_DEPTH_TEST));
    GL_CHECK(glDepthFunc(GL_LESS));
    
    // Transfer ownership of singleton components to world
    ecs::World::GetInstance().SetSingletonComponent<rendering::RenderingContextSingletonComponent>(std::move(renderingContextComponent));
    ecs::World::GetInstance().SetSingletonComponent<rendering::WindowSingletonComponent>(std::move(windowComponent));
    
}

///-----------------------------------------------------------------------------------------------

void GenesisEngine::InitializeServices() const
{
    resources::ResourceLoadingService::GetInstance().Initialize();
    sound::SoundService::GetInstance().Initialize();
    scripting::LuaScriptingService::GetInstance().Initialize();
    scripting::BindDefaultEngineFunctionsToLua();
}

///-----------------------------------------------------------------------------------------------

void GenesisEngine::InitializeDefaultConsoleFont() const
{
    rendering::LoadFont(CONSOLE_FONT_NAME, CONSOLE_FONT_ATLAS_COLS, CONSOLE_FONT_ATLAS_ROWS);
}

///-----------------------------------------------------------------------------------------------

void GenesisEngine::UpdateFrameStatistics(float& dt, float& elapsedTicks, float& dtAccumulator, long long& framesAccumulator) const
{
    // Calculate frame delta
    const auto currentTicks = static_cast<float>(SDL_GetTicks());
    const auto lastFrameTicks = currentTicks - elapsedTicks;

    elapsedTicks = currentTicks;
    dt = lastFrameTicks * 0.001f;

    framesAccumulator++;
    dtAccumulator += dt;

    if (dtAccumulator > 1.0f)
    {     
#if !defined(NDEBUG) || defined(CONSOLE_ENABLED_ON_RELEASE)
        auto& world = ecs::World::GetInstance();
        if (world.HasSingletonComponent<debug::DebugViewStateSingletonComponent>())
        {
            world.GetSingletonComponent<debug::DebugViewStateSingletonComponent>().mCurrentFps = static_cast<int>(framesAccumulator);
        }            
        Log(LogType::INFO, (std::string("FPS: ") + std::to_string(framesAccumulator) + " | Entities: " + std::to_string(world.GetEntityCount())).c_str());
#endif
        
        framesAccumulator = 0;
        dtAccumulator = 0.0f;
    }        
    
    // Make sure the console system has been registered at all
    if (ecs::World::GetInstance().HasSingletonComponent<debug::ConsoleStateSingletonComponent>() == false)
    {
        return;
    }

    auto& consoleStateComponent = ecs::World::GetInstance().GetSingletonComponent<debug::ConsoleStateSingletonComponent>();

    // Freeze dt in case the console is opened
    if (consoleStateComponent.mEnabled)
    {
        dt = 0.0f;
    }
}

///------------------------------------------------------------------------------------------------

bool AppShouldQuit()
{
    auto& inputStateComponent = ecs::World::GetInstance().GetSingletonComponent<input::InputStateSingletonComponent>();
    inputStateComponent.mMouseWheelDelta = 0;
    
    // Poll events
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT: return true;
            case SDL_KEYDOWN:
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
                    return true;
                }
            } break;
            case SDL_MOUSEWHEEL:
            {
                inputStateComponent.mMouseWheelDelta = event.wheel.y;
            } break;
                
#if !defined(NDEBUG) || defined(CONSOLE_ENABLED_ON_RELEASE)
            case SDL_TEXTINPUT:
            {
                auto& world = ecs::World::GetInstance();
                if (world.HasSingletonComponent<debug::ConsoleStateSingletonComponent>())
                {
                    auto& consoleStateComponent = world.GetSingletonComponent<debug::ConsoleStateSingletonComponent>();
                    if (consoleStateComponent.mEnabled)
                    {                        
                        consoleStateComponent.mCurrentCommandTextBuffer += std::string(event.text.text);
                    }                    
                }                
            } 
#endif
        }
    }

    return false;
}

///------------------------------------------------------------------------------------------------

}

///------------------------------------------------------------------------------------------------
