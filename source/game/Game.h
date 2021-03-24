///------------------------------------------------------------------------------------------------
///  Game.h
///  AncientGreece
///
///  Created by Alex Koukoulas on 19/11/2019.
///------------------------------------------------------------------------------------------------

#ifndef App_h
#define App_h

///------------------------------------------------------------------------------------------------

#include "../engine/IGame.h"

///------------------------------------------------------------------------------------------------

class Game final: public genesis::IGame
{
public:    
    void VOnSystemsInit() override;
    void VOnGameInit() override;
    void VOnUpdate(float& dt) override;
    
private:
    void RegisterConsoleCommands() const;
};       

///------------------------------------------------------------------------------------------------

#endif /* Game_h */
