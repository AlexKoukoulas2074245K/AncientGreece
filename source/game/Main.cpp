///------------------------------------------------------------------------------------------------
///  Main.cpp
///  AncientGreece
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#include "Game.h"
#include "../engine/GenesisEngine.h"

#if defined(_WIN32) && !defined(NDEBUG)
//#include <vld.h>
#endif

///------------------------------------------------------------------------------------------------

int main(int, char**)
{
    genesis::GenesisEngine engine;
    genesis::GameStartupParameters startupParameters("AncientGreece", 0.8f);
    
    Game game;
    engine.RunGame(startupParameters, game);
}

///------------------------------------------------------------------------------------------------


