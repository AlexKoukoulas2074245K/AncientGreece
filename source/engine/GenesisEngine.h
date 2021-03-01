///------------------------------------------------------------------------------------------------
///  GenesisEngine.h
///  Genesis
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef GenesisEngine_h
#define GenesisEngine_h

///------------------------------------------------------------------------------------------------

#include "ECS.h"
#include "GameStartupParameters.h"
#include "IGame.h"

///------------------------------------------------------------------------------------------------

namespace genesis
{

///------------------------------------------------------------------------------------------------
/// The central Engine class. Should be used to start the game
class GenesisEngine final
{
public:
    GenesisEngine();

    /// Main game startup method
    /// @param[in] startupParameters the parameters that determine the startup configuration of the engine.
    /// @param[in] game the reference to the IGame implementation to run.
    void RunGame(const GameStartupParameters& startupParameters, IGame& game);

private:
    void Initialize(const GameStartupParameters& startupParameters);
    void InitializeSdlContextAndWindow(const GameStartupParameters& startupParameters);    
    void InitializeServices() const;
    void InitializeDefaultConsoleFont() const;
    void UpdateFrameStatistics(float& dt, float& elapsedTicks, float& dtAccumulator, long long& framesAccumulator) const;
};

}

///------------------------------------------------------------------------------------------------

#endif /* GenesisEngine_h */
